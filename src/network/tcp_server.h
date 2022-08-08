#pragma once

#include <set>

#include "event/task_scheduler.h"
#include "tcp_connection.h"

namespace gortsp {

class TcpServer {
public:
  TcpServer();

  explicit TcpServer(uint16_t max_connections);

  ~TcpServer();

public:
  RtspStatus init(uint16_t port);

  static void on_connect(int fd, short events, void* p);

  void start();

private:
  virtual void on_connect_internal(int fd, short events, void* args);

  // virtual void on_read_internal(int fd, short events, void* args);

private:
  Socket server_socket_;

  uint16_t max_connections_;
  std::set<TcpConnection> connections_;

  TaskScheduler task_scheduler_;
};

}