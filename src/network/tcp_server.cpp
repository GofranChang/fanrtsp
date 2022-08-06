#include "tcp_server.h"
#include "common/logger.h"

#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"

namespace gortsp {

TcpServer::~TcpServer() {}

RtspStatus TcpServer::init(uint16_t port) {
  server_socket_.create();
  server_socket_.set_non_block();
  server_socket_.bind("0.0.0.0", port);
  server_socket_.listen(0);

  task_scheduler_.register_task(
      server_socket_.fd(),
      std::bind(
          &TcpServer::on_connect,
          this,
          std::placeholders::_1,
          std::placeholders::_2,
          std::placeholders::_3));

  return RtspStatus::SUCCESS;
}

void TcpServer::on_connect(int fd, short events, void* args) {
  if (fd != server_socket_.fd()) return;

  GLOGD("On connect...");
  Socket clientsock;
  server_socket_.accept(clientsock);
}

}