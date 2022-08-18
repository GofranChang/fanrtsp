#include "tcp_connection.hpp"

#include "socket.h"
#include "common/logger.h"

namespace fannetwork {

std::shared_ptr<TcpConnection> TcpConnection::create() {
  auto p = new TcpConnection();
  return std::shared_ptr<TcpConnection>(p);
}

std::shared_ptr<TcpConnection> TcpConnection::create(const std::shared_ptr<Socket>& socket) {
  auto p = new TcpConnection(socket);
  return std::shared_ptr<TcpConnection>(p);
}

TcpConnection::TcpConnection() : socket_(nullptr),
                                 connecting_(false) {
}

TcpConnection::TcpConnection(const std::shared_ptr<Socket>& socket) : socket_(socket),
                                                                      connecting_(true) {
}

TcpConnection::~TcpConnection() {
}

NetState TcpConnection::set_conneted_socket(const std::shared_ptr<Socket>& socket) {
  if (connecting_ || socket_) {
    GLOGE("Set connection socket failed, already connected");
    return NetState::MULTI_OPERATOR;
  }

  if (!socket || 0 > socket->fd()) {
    GLOGE("Set connection socket failed, input socket illegal");
    return NetState::ILLEGAL_PARAMS;
  }

  socket_ = socket;
  connecting_ = true;
  return NetState::SUCCESS;
}

void TcpConnection::disconnect() {
  if (socket_)
    socket_.reset();

  connecting_ = false;
}

int TcpConnection::fd() {
  return socket_ ? socket_->fd() : -1;
}

DefaultTcpConnectionHandler::DefaultTcpConnectionHandler() {}

void DefaultTcpConnectionHandler::set_connection(TcpConnection* srv) {
  connection_ = srv;
}

void DefaultTcpConnectionHandler::on_event(int32_t fd, int16_t evt) {}

void DefaultTcpConnectionHandler::on_read(int32_t fd, const std::string & msg) {
  GLOGT("On read : {}", msg);
}

}