#include "tcp_connection.h"

#include "logger.h"

namespace gortsp {

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
    GLOGE();
    // 
  }

  socket_.disconnect();
}

}