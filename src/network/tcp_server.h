#pragma once

#include <set>

#include "event/task_scheduler.h"
#include "tcp_connection.h"

namespace gortsp {

class TcpServer {
public:
TcpServer() :
    server_socket_(Socket::SocketType::TCP_SOCKET),
    max_connections_(16),
    task_scheduler_(17) {
}

explicit TcpServer(uint16_t max_connections) :
    server_socket_(Socket::SocketType::TCP_SOCKET),
    max_connections_(max_connections),
    task_scheduler_(max_connections + 1) {
}

~TcpServer();

public:
  RtspStatus init(uint16_t port);

  void on_connect(int fd, short events, void* args);

  void on_read(int fd, short events, void* args);

private:
  Socket server_socket_;

  uint16_t max_connections_;
  std::set<TcpConnection> connections_;

  TaskScheduler task_scheduler_;
};

}