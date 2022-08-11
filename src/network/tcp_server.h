#pragma once

#include <unordered_map>
#include <string>
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

  virtual RtspStatus on_read(int fd, std::string buff);

  virtual RtspStatus on_diconnect(int fd, short events);

private:
  std::shared_ptr<Socket> server_socket_;

  uint16_t max_connections_;
  std::unordered_map<int, std::shared_ptr<TcpConnection>> connections_;

  TaskScheduler task_scheduler_;

  std::function<RtspStatus(int, short)> on_connect_func_;
  std::function<RtspStatus(int, std::string)> on_read_func_;
  std::function<RtspStatus(int, short)> on_disconnect_func_;
};

}