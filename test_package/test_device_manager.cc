#include <xdaq/device_manager.h>

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <unordered_set>


using json = nlohmann::json;
namespace fs = std::filesystem;

auto get_possible_manager_pathes()
{
    const auto device_manager_dir = std::filesystem::path("managers");
    std::unordered_set<fs::path> manager_pathes;
    for (const auto &entry : fs::directory_iterator(device_manager_dir))
        manager_pathes.insert(fs::canonical(entry.path()));
    return manager_pathes;
}

TEST_CASE("Device Manager Loading", "[manager]")
{
    const auto manager_pathes = get_possible_manager_pathes();
    int found_managers = 0;
    for (auto &manager_path : manager_pathes) {
        try {
            auto manager = xdaq::get_device_manager(manager_path);
            REQUIRE(manager != nullptr);
            ++found_managers;
        } catch (...) {
        }
    }
    REQUIRE(found_managers == NUM_MANAGERS);
}

auto get_device_manager_pathes()
{
    const auto device_manager_dir = std::filesystem::path("managers");
    std::unordered_set<fs::path> manager_pathes;
    for (const auto &entry : fs::directory_iterator(device_manager_dir)) try {
            auto manager = xdaq::get_device_manager(fs::canonical(entry.path()));
            manager_pathes.insert(fs::canonical(entry.path()));
        } catch (...) {
            continue;
        }
    return manager_pathes;
}

TEST_CASE("Device Manager Info Parsing", "[manager]")
{
    const auto manager_pathes = get_device_manager_pathes();
    for (auto &manager_path : manager_pathes) {
        auto manager = xdaq::get_device_manager(manager_path);
        REQUIRE(manager != nullptr);
        auto info = manager->info();
        REQUIRE(info.length() > 0);
        auto j = json::parse(info);
        REQUIRE_FALSE(j.empty());
        REQUIRE(j.contains("name"));
        REQUIRE(j.contains("description"));
    }
}

TEST_CASE("Device Manager Options Schema Parsing", "[manager]")
{
    const auto manager_pathes = get_device_manager_pathes();
    for (auto &manager_path : manager_pathes) {
        auto manager = xdaq::get_device_manager(manager_path);
        REQUIRE(manager != nullptr);
        auto options = manager->get_device_options();
        REQUIRE(options.length() > 0);
        auto j = json::parse(options);
        REQUIRE_FALSE(j.empty());
        REQUIRE(j.contains("properties"));
        REQUIRE(j.contains("type"));
    }
}

TEST_CASE("Device Manager List Devices", "[manager]")
{
    const auto manager_pathes = get_device_manager_pathes();
    for (auto &manager_path : manager_pathes) {
        auto manager = xdaq::get_device_manager(manager_path);
        REQUIRE(manager != nullptr);
        REQUIRE_NOTHROW(manager->list_devices());
    }
}