cmake_minimum_required(VERSION 3.26)

project(libxdaq-dist)

if(MSVC)
    add_compile_definitions(NOMINMAX)
endif()

find_package(Catch2 REQUIRED)
find_package(xdaq REQUIRED)
find_package(nlohmann_json REQUIRED)

include(CTest)
include(Catch)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(test_device_manager test_device_manager.cc)
target_compile_features(test_device_manager PRIVATE cxx_std_23)

# Count the number of device managers for the test
target_compile_definitions(test_device_manager PRIVATE
    NUM_MANAGERS=${XDAQ_NUM_DEVICE_MANAGERS}
)

# Copy device managers from package to build directory
file(COPY ${XDAQ_DEVICE_MANAGER_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(test_device_manager PRIVATE
    xdaq::xdaq_device
    Catch2::Catch2WithMain
    nlohmann_json::nlohmann_json
)

catch_discover_tests(test_device_manager)