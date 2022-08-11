#include "tcp_connection.h"

#include "logger.h"

namespace gortsp {

std::shared_ptr<TcpConnection> TcpConnection::create() {
  TcpConnection* p = new TcpConnection;
  return std::shared_ptr<TcpConnection>(p);
}

TcpConnection::TcpConnection() :
    remote_ip_(""),
    remote_port_(0),
    local_ip_(""),
    local_port_(0),
    socket_(nullptr) {
}

TcpConnection::~TcpConnection() {
  disconnect();
}

RtspStatus TcpConnection::accept(std::shared_ptr<Socket> socket) {
  if (socket_) {
    GLOGE("Accept tcp connection failed, current socket already init");
    return RtspStatus::MULTI_OPERATOR;
  }

  socket_ = socket;
  return RtspStatus::SUCCESS;
}

RtspStatus TcpConnection::connect(const std::string& remote_ip, uint16_t remote_port) {
  if (socket_) {
    GLOGE("Accept tcp connection failed, current socket already init");
    return RtspStatus::MULTI_OPERATOR;
  }

#if 0
  socket_ = Socket::create(Socket::SocketType::TCP_SOCKET);
  if (socket_->connect(remote_ip, remote_port, 0) != RtspStatus::SUCCESS) {
    GLOGE("TCP connect failed");
    // return 
  }
  
  remote_ip_ = remote_ip;
  remote_port_ = remote_port;
#endif

  return RtspStatus::SUCCESS;
}

RtspStatus TcpConnection::disconnect() {
  if (!connected()) {
    // GLOGE();
    // 
  }

  return socket_->disconnect();
}

RtspStatus TcpConnection::send(const std::string& data) {
  return socket_->send(data);
}

}