#include <xdaq/device_plugin.h>

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>


#if defined(_WIN32)
#include <windows.h>
#endif

using json = nlohmann::json;
namespace fs = std::filesystem;

auto get_plugin_paths()
{
    const auto plugin_dir = std::filesystem::path(PLUGIN_PATH);
#if defined(_WIN32)
    auto res = SetDllDirectoryA(plugin_dir.generic_string().c_str());
    if (res == 0) throw std::runtime_error("Failed to set DLL directory");
#endif
    std::vector<fs::path> plugin_paths;
    for (const auto &entry : fs::directory_iterator(plugin_dir)) {
        if (fs::is_directory(entry)) continue;
        if (!entry.path().filename().generic_string().contains("device_plugin")) continue;
        plugin_paths.push_back(fs::canonical(entry.path()));
    }
    // remove duplicates
    std::ranges::sort(plugin_paths);
    plugin_paths.erase(std::unique(plugin_paths.begin(), plugin_paths.end()), plugin_paths.end());
    return plugin_paths;
}

TEST_CASE("Plugin Loading", "[plugin]")
{
    const auto plugin_paths = get_plugin_paths();
    for (auto &plugin_path : plugin_paths) {
        auto plugin = xdaq::get_plugin(plugin_path);
        REQUIRE(plugin != nullptr);
    }
}

TEST_CASE("Plugin Info Parsing", "[plugin]")
{
    const auto plugin_paths = get_plugin_paths();
    for (auto &plugin_path : plugin_paths) {
        auto plugin = xdaq::get_plugin(plugin_path);
        REQUIRE(plugin != nullptr);
        auto info = plugin->info();
        REQUIRE(info.length() > 0);
        auto j = json::parse(info);
        REQUIRE_FALSE(j.empty());
        REQUIRE(j.contains("name"));
        REQUIRE(j.contains("description"));
    }
}

TEST_CASE("Plugin Options Schema Parsing", "[plugin]")
{
    const auto plugin_paths = get_plugin_paths();
    for (auto &plugin_path : plugin_paths) {
        auto plugin = xdaq::get_plugin(plugin_path);
        REQUIRE(plugin != nullptr);
        auto options = plugin->get_device_options();
        REQUIRE(options.length() > 0);
        auto j = json::parse(options);
        REQUIRE_FALSE(j.empty());
        REQUIRE(j.contains("properties"));
        REQUIRE(j.contains("type"));
    }
}

TEST_CASE("Plugin List Devices", "[plugin]")
{
    const auto plugin_paths = get_plugin_paths();
    for (auto &plugin_path : plugin_paths) {
        auto plugin = xdaq::get_plugin(plugin_path);
        REQUIRE(plugin != nullptr);
        REQUIRE_NOTHROW(plugin->list_devices());
    }
}