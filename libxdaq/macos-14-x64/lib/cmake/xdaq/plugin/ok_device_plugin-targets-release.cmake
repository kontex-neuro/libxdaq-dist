#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "xdaq::ok_device_plugin" for configuration "Release"
set_property(TARGET xdaq::ok_device_plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xdaq::ok_device_plugin PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/xdaq/plugin/libok_device_plugin.0.0.1.dylib"
  IMPORTED_SONAME_RELEASE "@rpath/libok_device_plugin.0.dylib"
  )

list(APPEND _cmake_import_check_targets xdaq::ok_device_plugin )
list(APPEND _cmake_import_check_files_for_xdaq::ok_device_plugin "${_IMPORT_PREFIX}/lib/xdaq/plugin/libok_device_plugin.0.0.1.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
