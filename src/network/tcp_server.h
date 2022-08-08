#pragma once

#include <set>
#include <functional>

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

  void start();

private:
  virtual RtspStatus on_connect(int fd, short events);

  // virtual void on_read_internal(int fd, short events, void* args);

private:
  Socket server_socket_;

  uint16_t max_connections_;
  std::set<TcpConnection> connections_;

  TaskScheduler task_scheduler_;

  std::function<RtspStatus(int fd, short event)> on_connect_func_;
};

}