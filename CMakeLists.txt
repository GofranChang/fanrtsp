cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 11)

project(gortsp)

set(NETWORK_SRC
  src/network/socket.h
  src/network/socket.cpp
  src/network/tcp_connection.h
  src/network/tcp_connection.cpp
  
  src/network/tcp_server.h
  src/network/tcp_server.cpp
)

set(TASK_SRC
  src/event/event_handler.h
  src/event/event_handler.cpp
  src/event/task_scheduler.h
  src/event/task_scheduler.cpp
)

add_subdirectory(third_party/libevent libevent.out EXCLUDE_FROM_ALL)
include_directories(third_party/libevent/include)
include_directories(${PROJECT_BINARY_DIR}/libevent.out/include)

include_directories(src)
include_directories(src/common)

add_library(gortsp SHARED
    ${NETWORK_SRC}
    ${TASK_SRC})

target_link_libraries(gortsp event)

add_executable(tcp_server_test test/tcp_server_test.cpp)
target_link_libraries(tcp_server_test gortsp)
