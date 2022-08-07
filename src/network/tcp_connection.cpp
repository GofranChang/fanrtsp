#include "tcp_connection.h"

#include "logger.h"

namespace gortsp {

TcpConnection::TcpConnection() :
    remote_ip_(""),
    remote_port_(0),
    local_ip_(""),
    local_port_(0),
    socket_(Socket::SocketType::TCP_SOCKET) {
}

TcpConnection::~TcpConnection() {
  disconnect();
}

RtspStatus TcpConnection::connect(const std::string& remote_ip, uint16_t remote_port) {
  if (socket_.stat() != Socket::SocketStat::CREATED ||
          socket_.stat() != Socket::SocketStat::BINDED) {
    // 
  }

  if (socket_.connect(remote_ip, remote_port, 0) != RtspStatus::SUCCESS) {
    GLOGE("TCP connect failed");
    // return 
  }
  
  remote_ip_ = remote_ip;
  remote_port_ = remote_port;

  return RtspStatus::SUCCESS;
}

RtspStatus TcpConnection::disconnect() {
  if (!connected()) {
    // GLOGE();
    // 
  }

  return socket_.disconnect();
}

}