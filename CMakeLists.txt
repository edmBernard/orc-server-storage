project(orc-server-storage)

#**************************************************************************************************
# General cMake settings
#**************************************************************************************************
cmake_minimum_required(VERSION 3.5)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0007 NEW)
endif(COMMAND cmake_policy)

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
  set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

#**************************************************************************************************
# Find Package ************************************************************************************

# Orc dependencies
find_library(ORC_LIBRARY NAMES orc liborc)

# Protobuf dependencies
find_package(protobuf CONFIG REQUIRED)
find_library(LZ4_LIBRARY NAMES lz4 liblz4)
find_library(ZSTD_LIBRARY NAMES zstd)
find_package (ZLIB REQUIRED)
find_package(Snappy CONFIG REQUIRED)

# uWebsocket dependencies
find_library(UWEBSOCKET_LIBRARY uWS)
find_package(OpenSSL REQUIRED)

# cpr dependencies
find_library(CPR_LIBRARIES cpr)
find_package(CURL REQUIRED)

# everything else
find_package(Catch2 CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)


# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()


#**************************************************************************************************
# Include *****************************************************************************************
include_directories(${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stryke/include)

#**************************************************************************************************
# Set variable ************************************************************************************
SET(SOURCES
)

#**************************************************************************************************
# Set compiler ************************************************************************************
add_compile_options(-std=c++17 -Wall -fPIC) # for all targets in current directory

#**************************************************************************************************
# Linker ******************************************************************************************


#**************************************************************************************************
# Build Executable
#**************************************************************************************************
set(clients
  client_thread
  client_basic
  client_cpr
)

foreach(client ${clients})
  add_executable("stryke_${client}" ${CMAKE_CURRENT_SOURCE_DIR}/src/${client}.cpp)
  target_compile_options("stryke_${client}" PRIVATE -O3)
  target_link_libraries("stryke_${client}" PRIVATE -lstdc++fs -pthread ${CPR_LIBRARIES} ${CURL_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)
endforeach(client)

set(servers
  server_thread
)

foreach(server ${servers})
  add_executable("stryke_${server}" ${CMAKE_CURRENT_SOURCE_DIR}/src/${server}.cpp)
  target_compile_options("stryke_${server}" PRIVATE -O3)
  target_link_libraries("stryke_${server}" PRIVATE -lstdc++fs ${UWEBSOCKET_LIBRARY} OpenSSL::SSL OpenSSL::Crypto ${ORC_LIBRARY} Snappy::snappy ZLIB::ZLIB protobuf::libprotoc protobuf::libprotobuf ${LZ4_LIBRARY} ${ZSTD_LIBRARY})
endforeach(server)

#**************************************************************************************************
# Documentation generation
#**************************************************************************************************
add_custom_target(
  docs
  COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/docs/config_file
)
