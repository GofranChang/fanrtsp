#pragma once

#include <set>

#include "tcp_connection.h"

namespace gortsp {

class TcpServer {
public:
TcpServer() :
    server_socket_(Socket::SocketType::TCP_SOCKET),
    max_connections_(16) {
}

explicit TcpServer(uint16_t max_connections) :
    server_socket_(Socket::SocketType::TCP_SOCKET),
    max_connections_(max_connections) {
}

~TcpServer();

public:
  RtspStatus init(uint16_t port);
  
private:
  Socket server_socket_;

  uint16_t max_connections_;
  std::set<TcpConnection> connections_;
};

}