cmake_minimum_required(VERSION 3.26)

project(libxdaq-dist-example)

find_package(xdaq REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)

add_executable(set_ttl_out set_ttl_out.cc)
target_compile_features(set_ttl_out PRIVATE cxx_std_23)
target_link_libraries(set_ttl_out
PRIVATE
    xdaq::xdaq_device
    nlohmann_json::nlohmann_json
    fmt::fmt
)

# Copy device managers and resources from conan package to build directory
file(COPY ${XDAQ_DEVICE_MANAGER_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${XDAQ_RESOURCES_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
