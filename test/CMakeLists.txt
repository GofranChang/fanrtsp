cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

add_definitions(-std=c++11)

project(GortspTest)

set(TEST_ROOT ${PROJECT_SOURCE_DIR})
set(SRC
    ${TEST_ROOT}/../src/network/socket.h
    ${TEST_ROOT}/../src/network/socket.cpp
    ${TEST_ROOT}/../src/network/tcp_connection.h
    ${TEST_ROOT}/../src/network/tcp_connection.cpp
    ${TEST_ROOT}/../src/network/tcp_server.h
    ${TEST_ROOT}/../src/network/tcp_server.cpp
)

include_directories(${TEST_ROOT}/../src/)
include_directories(${TEST_ROOT}/../src/common/)
include_directories(${TEST_ROOT}/../src/network/)
add_executable(server tcp_server_test.cpp ${SRC})