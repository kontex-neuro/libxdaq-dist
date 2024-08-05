#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <string>


namespace xdaq
{

enum class BasicDeviceStatus : int {
    Success,
    Failure,
};

template <typename Addr, typename Value, typename Return>
class BasicDevice
{
public:
    using addr_t = Addr;
    using value_t = Value;
    using return_t = Return;
    constexpr static value_t value_mask = std::numeric_limits<value_t>::max();

    BasicDevice() = default;
    BasicDevice(const BasicDevice &) = delete;
    BasicDevice &operator=(const BasicDevice &) = delete;
    BasicDevice(BasicDevice &&) = default;
    BasicDevice &operator=(BasicDevice &&) = default;

    virtual ~BasicDevice() = default;

    virtual return_t set_register_sync(
        addr_t addr, value_t value, value_t mask = value_mask
    ) noexcept
    {
        set_register(addr, value, mask);
        return send_registers();
    }

    virtual return_t set_register(
        addr_t addr, value_t value, value_t mask = value_mask
    ) noexcept = 0;

    virtual return_t send_registers() noexcept = 0;

    virtual std::optional<value_t> get_register_sync(addr_t addr) noexcept
    {
        if (read_registers() != return_t::Success) return std::nullopt;
        return {get_register(addr)};
    }

    virtual value_t get_register(addr_t addr) noexcept = 0;

    virtual return_t read_registers() noexcept = 0;

    virtual return_t trigger(addr_t addr, int bit) noexcept = 0;

    virtual std::size_t write(addr_t addr, std::size_t length, const unsigned char *data) = 0;

    // synchronous stream read
    virtual std::size_t read(addr_t addr, std::size_t length, unsigned char *data) = 0;

    // asynchronous stream read
    struct DataStream {
        // The usage of a unique_ptr for buffer is to allow custom memory management
        // in the implementation of the device. See specific device implementation for details.
        using receive_event_t = std::function<void(
            std::unique_ptr<unsigned char[], void (*)(unsigned char[])> buffer, std::size_t length
        )>;
        DataStream(receive_event_t receive_event) : receive_event(receive_event) {}
        virtual ~DataStream() = default;
        DataStream(const DataStream &) = delete;
        DataStream &operator=(const DataStream &) = delete;

        virtual void stop() = 0;
        receive_event_t receive_event;
    };

    // TODO: take ownership of the Device when creating DataStream to ensure exclusive access
    virtual std::optional<std::unique_ptr<DataStream>> start_read_stream(
        addr_t addr, typename DataStream::receive_event_t receive_event
    )
    {
        // it's up to the implementation to decide if it supports asynchronous stream read
        return std::nullopt;
    }

    virtual std::optional<std::string> get_status() { return std::nullopt; }
    virtual std::optional<std::string> get_info() { return std::nullopt; }
};

using Device = BasicDevice<std::uint32_t, std::uint32_t, BasicDeviceStatus>;

}  // namespace xdaq
