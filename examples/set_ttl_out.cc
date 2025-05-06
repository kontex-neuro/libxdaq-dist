#include <fmt/format.h>
#include <xdaq/data_streams.h>
#include <xdaq/device.h>
#include <xdaq/device_manager.h>

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <unordered_set>
#include <variant>


namespace fs = std::filesystem;
using namespace std::chrono_literals;
using json = nlohmann::json;

std::optional<xdaq::DeviceManager::OwnedDevice> get_device(std::string mode, int device_index)
{
    const auto device_manager_dir = std::filesystem::path("managers");
    std::unordered_set<fs::path> manager_pathes;
    for (const auto &entry : fs::directory_iterator(device_manager_dir)) try {
            auto manager = xdaq::get_device_manager(fs::canonical(entry.path()));
            manager_pathes.insert(fs::canonical(entry.path()));
        } catch (...) {
            continue;
        }
    int i = 0;
    for (auto &manager_path : manager_pathes) {
        auto manager = xdaq::get_device_manager(manager_path);
        fmt::println("Found manager: {}", manager_path.string());
        for (auto d : json::parse(manager->list_devices())) {
            fmt::println("Found device: {}", d.dump(4));
            if (i < device_index) {
                ++i;
                continue;
            }
            d["mode"] = mode;
            fmt::println("Creating device: {}", d.dump(4));
            return manager->create_device(d.dump());
        }
    }
    return std::nullopt;
}

auto run(xdaq::DeviceManager::OwnedDevice device, std::size_t frame_size)
{
    // Set frequency
    while ((*device->get_register_sync(0x24) & 3u) != 3) std::this_thread::sleep_for(1ms);
    device->set_register_sync(0x03, 0x2A19, 0xFFFFFFFF);
    device->trigger(0x40, 0);
    while ((*device->get_register_sync(0x24) & 3u) != 3) std::this_thread::sleep_for(1ms);

    // Set run mode
    device->set_register_sync(0x00, 2, 2);
    device->set_register_sync(0x01, 0);


    {
        std::atomic_uint32_t ttl_out_read_back{0};
        using namespace xdaq::DataStream;
        auto ds = device->start_read_stream(
            0xA0,
            aligned_read_stream(
                [frame_size, &ttl_out_read_back](Event e) {
                    std::visit(
                        [frame_size, &ttl_out_read_back](auto &&e) {
                            using T = std::decay_t<decltype(e)>;
                            if constexpr (std::is_same_v<T, Events::Error>) {
                            } else if constexpr (std::is_same_v<T, Events::Stop>) {
                            } else if constexpr (std::is_same_v<T, Events::OwnedData>) {
                            } else if constexpr (std::is_same_v<T, Events::DataView>) {
                                if (e.data.size() < frame_size) return;
                                std::uint32_t ts, ttl_in, ttl_out;
                                std::memcpy(&ts, &e.data[8], sizeof(ts));
                                std::memcpy(&ttl_in, &e.data[frame_size - 8], sizeof(ttl_in));
                                std::memcpy(&ttl_out, &e.data[frame_size - 4], sizeof(ttl_out));
                                if (ttl_out == ttl_out_read_back) return;
                                ttl_out_read_back = ttl_out;
                                fmt::println("TS {:08X} DI {:08X} DO {:08X}", ts, ttl_in, ttl_out);
                            } else {
                                static_assert(xdaq::always_false_v<T>, "non-exhaustive visitor");
                            }
                        },
                        e
                    );
                },
                frame_size
            ),
            frame_size * 30000 / 1000  // target 1kHz readout rate if possible
        );


        // Set ttl override, Only required for Gen 2 XDAQ in RHS mode
        device->set_register_sync(0x1400, 0xFFFFFFFFu);

        // Start
        device->trigger(0x41, 0);

        // Set TTL out
        for (int i = 0; i < 32; ++i) {
            auto ttl_value = 1 << (i % 32);
            device->set_register_sync(0x15, ttl_value);
            while (ttl_value != ttl_out_read_back) {
                std::this_thread::sleep_for(1ms);
            }
        }

        // Set ttl override, Only required for Gen 2 XDAQ in RHS mode
        device->set_register_sync(0x1400, 0);
    }

    // Stop
    device->set_register_sync(0x00, 0, 2);

    // Wait for stop
    while ((*device->get_register_sync(0x22) & 1u) > 0) {
        std::this_thread::sleep_for(1ms);
    }

    // Flush
    device->read(0xA0, 0, nullptr);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fmt::println(
            "Usage: {} <device_index> <mode>\n"
            "device_index: index of the device to test\n"
            "mode: rhd or rhs\n",
            argv[0]
        );
        return 1;
    }
    int device_index = std::stoi(argv[1]);
    if (auto device = get_device(argv[2], device_index)) {
        fmt::println("Testing ttl out in {} mode", argv[2]);
        run(std::move(device.value()), (std::string(argv[2]) == "rhd") ? 40 : 56);
    } else {
        fmt::println("No device found");
        return 0;
    }
}