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

  task_scheduler_.register_task(
      server_socket_.fd(),
      TcpServer::on_connect,
      this);

  return RtspStatus::SUCCESS;
}

void TcpServer::on_connect_internal(int fd, short events, void* args) {
  if (fd != server_socket_.fd()) return;

  GLOGD("On connect...");
  TcpConnection conn;
  server_socket_.accept(conn);

  // connections_.insert(conn);

  conn.send("Welcome to my server");
}

void TcpServer::start() {
  task_scheduler_.start();
}

void TcpServer::on_connect(int fd, short events, void* p) {
  if (nullptr == p) return;

  TcpServer* srv = static_cast<TcpServer*>(p);
  srv->on_connect_internal(fd, events, p);
}

// void TcpServer::on_read_internal() {}

}