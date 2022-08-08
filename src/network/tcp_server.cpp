#include "tcp_server.h"
#include "common/logger.h"

#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"

namespace gortsp {

TcpServer::TcpServer() :
    server_socket_(Socket::SocketType::TCP_SOCKET),
    max_connections_(16),
    task_scheduler_(17) {
}

TcpServer::TcpServer(uint16_t max_connections) :
    server_socket_(Socket::SocketType::TCP_SOCKET),
    max_connections_(max_connections),
    task_scheduler_(max_connections + 1) {
}

TcpServer::~TcpServer() {
  // TODO: Do uninit
}

RtspStatus TcpServer::init(uint16_t port) {
  if (server_socket_.create() != RtspStatus::SUCCESS) {
    GLOGE("TCP server err : create server socket failed");
    return RtspStatus::INTERNAL_ERR;
  }

  if (server_socket_.set_non_block() != RtspStatus::SUCCESS) {
    GLOGE("TCP server err : set socket non block failed");
    return RtspStatus::INTERNAL_ERR;
  }

  if (server_socket_.bind("0.0.0.0", port) != RtspStatus::SUCCESS) {
    GLOGE("TCP server err : bind local addr failed");
    return RtspStatus::INTERNAL_ERR;
  }

  if (server_socket_.listen(10) != RtspStatus::SUCCESS) {
    GLOGE("TCP server err : bind local addr failed");
    return RtspStatus::INTERNAL_ERR;
  }

  GLOGD("TCP : server socket init success");

  on_connect_func_ = std::bind(&TcpServer::on_connect, this, std::placeholders::_1, std::placeholders::_2);
  task_scheduler_.register_task(server_socket_.fd(), &on_connect_func_);

  return RtspStatus::SUCCESS;
}

void TcpServer::start() {
  task_scheduler_.start();
}

RtspStatus TcpServer::on_connect(int fd, short events) {
  GLOGT("On connection, event {}", events);

  TcpConnection conn;
  server_socket_.accept(conn);
  conn.send("Welcome to my server");

  return RtspStatus::SUCCESS;
}

// void TcpServer::on_read_internal() {}

}