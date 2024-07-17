#pragma once
#include <filesystem>
#include <memory>
#include <string>

#include "device.h"

namespace xdaq
{

struct DevicePlugin : public std::enable_shared_from_this<DevicePlugin> {
    struct device_deleter {
        void operator()(Device *d) const { delete d; }
        std::shared_ptr<DevicePlugin> plugin;
    };

    using PluginOwnedDevice = typename std::unique_ptr<Device, device_deleter>;

    virtual ~DevicePlugin() = default;
    virtual std::string info() const noexcept = 0;
    virtual std::filesystem::path location() const = 0;

    virtual std::string list_devices() const = 0;
    virtual std::string get_device_options() const noexcept = 0;
    virtual PluginOwnedDevice create_device(const std::string &config) = 0;
};

// C++23: use std::expected instead of throwing exceptions
std::shared_ptr<DevicePlugin> get_plugin(const std::filesystem::path &path);

}  // namespace xdaq