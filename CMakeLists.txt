project(orc-server-storage)

#**************************************************************************************************
# General cMake settings
#**************************************************************************************************
cmake_minimum_required(VERSION 3.5)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

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
  main.cpp
)

#**************************************************************************************************
# Set compiler ************************************************************************************
add_compile_options(-std=c++17 -Wall -fPIC) # for all targets in current directory

#**************************************************************************************************
# Linker ******************************************************************************************


#**************************************************************************************************
# Build Executable
#**************************************************************************************************
add_executable("stryke_server" ${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp)
target_compile_options("stryke_server" PRIVATE -O3)
target_link_libraries("stryke_server" PRIVATE -lstdc++fs ${UWEBSOCKET_LIBRARY} OpenSSL::SSL OpenSSL::Crypto ${ORC_LIBRARY} Snappy::snappy ZLIB::ZLIB protobuf::libprotoc protobuf::libprotobuf ${LZ4_LIBRARY} ${ZSTD_LIBRARY})

#**************************************************************************************************
# Build Tests
#**************************************************************************************************
option(BUILD_UNIT_TESTS "Build the unit tests" OFF)
if(BUILD_UNIT_TESTS)
  enable_testing()

  SET(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp/unit-main.cpp
  )

  add_executable(unit-test ${TEST_SOURCES} ${SOURCES} ${TEST_HEADERS})
  target_link_libraries(unit-test PRIVATE -lstdc++fs ${UWEBSOCKET_LIBRARY} OpenSSL::SSL OpenSSL::Crypto ${ORC_LIBRARY} Snappy::snappy ZLIB::ZLIB protobuf::libprotoc protobuf::libprotobuf ${LZ4_LIBRARY} ${ZSTD_LIBRARY})

  add_test(NAME "unit_test_default"
    COMMAND unit-test
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif()

#**************************************************************************************************
# Documentation generation
#**************************************************************************************************
add_custom_target(
  docs
  COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/docs/config_file
)
