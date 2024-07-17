
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################
include(CMakeFindDependencyMacro)

include("${CMAKE_CURRENT_LIST_DIR}/xdaq-targets.cmake")

set(XDAQ_PLUGIN_TARGETS "")
file(GLOB plugin_found "${CMAKE_CURRENT_LIST_DIR}/plugin/*_plugin-targets.cmake")
foreach(plugin ${plugin_found})
    include("${plugin}")
    get_filename_component(plugin_target ${plugin} NAME_WE)
    string(REPLACE "-targets" "" plugin_name ${plugin_target})
    list(APPEND XDAQ_PLUGIN_TARGETS ${plugin_name})
endforeach()
set(XDAQ_RESOURCES_DIR "${CMAKE_CURRENT_LIST_DIR}/../../xdaq/resources")

check_required_components(xdaq)
find_dependency(Boost)
find_dependency(spdlog)
