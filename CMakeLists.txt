cmake_minimum_required(VERSION 3.0)

project(glApi)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
add_definitions(-DTEST_ENABLED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
target_include_directories(glApi_Test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)