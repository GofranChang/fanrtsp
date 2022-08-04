#include "tcp_server.h"

namespace gortsp {

TcpServer::~TcpServer() {}

RtspStatus TcpServer::init(uint16_t port) {
  server_socket_.create();
  server_socket_.bind("0.0.0.0", port);
  server_socket_.listen(0);

  Socket clientsock;
  server_socket_.accept(clientsock);
}

}