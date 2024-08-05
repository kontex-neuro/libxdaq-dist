cmake_minimum_required(VERSION 3.26)

project(libxdaq-dist)

find_package(Catch2 REQUIRED)
find_package(xdaq REQUIRED)
find_package(nlohmann_json REQUIRED)

include(CTest)
include(Catch)


list(GET XDAQ_PLUGIN_TARGETS 0 first_xdaq_plugin)
get_property(PLUGIN_PATH TARGET xdaq::${first_xdaq_plugin} PROPERTY LOCATION)
get_filename_component(PLUGIN_PATH ${PLUGIN_PATH} DIRECTORY)

add_executable(test_plugin test_plugin.cc)
target_compile_features(test_plugin PRIVATE cxx_std_23)
target_compile_definitions(test_plugin PRIVATE PLUGIN_PATH="${PLUGIN_PATH}")

target_link_libraries(test_plugin PRIVATE
    xdaq::xdaq_device
    Catch2::Catch2WithMain
    nlohmann_json::nlohmann_json
)

catch_discover_tests(test_plugin)