#include "tcp_server.hpp"

#include "common/logger.h"
#include "net_state.hpp"
#include "socket.h"
#include "task_scheduler.hpp"
#include "event_handler.hpp"
#include "tcp_connection.hpp"

namespace fannetwork {

DefaultAcceptHandler::DefaultAcceptHandler() : server_(nullptr) {
  INITLOGGER(spdlog::level::trace, "", true);
}

void DefaultAcceptHandler::set_server(TcpServer* srv) {
  if (server_ != nullptr)
    return;

  server_ = srv;
}

void DefaultAcceptHandler::on_event(int32_t fd, int16_t evt) {
  if (server_)
    server_->on_connect(fd);
}

void DefaultAcceptHandler::on_read(int32_t fd, const std::string & msg) {
}

TcpServer::TcpServer() : EventTarget(std::make_shared<DefaultAcceptHandler>()),
                         connect_handler_factory_(nullptr) {
  auto handler = static_cast<DefaultAcceptHandler*>(handler_.get());
  handler->set_server(this);
}

TcpServer::TcpServer(const std::shared_ptr<EventHandler>& handler) : EventTarget(handler),
                                                                     connect_handler_factory_(nullptr) {
}

TcpServer::TcpServer(EventHandlerFactory& connection_handler_factory) : EventTarget(std::make_shared<DefaultAcceptHandler>()),
                                                                        connect_handler_factory_(nullptr) {
}

TcpServer::TcpServer(const std::shared_ptr<EventHandler>& handler,
                     EventHandlerFactory& connection_handler_factory) : EventTarget(handler),
                                                                        connect_handler_factory_(connection_handler_factory) {
}

NetState TcpServer::init(int16_t port, int32_t thread_num) {
  auto scheduler = TaskScheduler::instance();
  scheduler->init(thread_num, {"connection"});

  if (accept_socket_) {
    GLOGE("Init tcp server failed, cur accpet socket exist, fd : {}", accept_socket_->fd());
    return NetState::MULTI_OPERATOR;
  }

  accept_socket_ = Socket::create(Socket::SocketType::TCP_SOCKET);
  if (!accept_socket_) {
    GLOGE("Create accept socket failed, reason {}", strerror(errno));
    return NetState::IO_OPERATOR_ERR;
  }

  if (accept_socket_->open() != NetState::SUCCESS) {
    GLOGE("TCP server err : create socket failed, reason {}", strerror(errno));
    return NetState::INTERNAL_ERR;
  }

  if (accept_socket_->set_non_block() != NetState::SUCCESS) {
    GLOGE("TCP server err : set socket non block failed");
    return NetState::INTERNAL_ERR;
  }

  if (accept_socket_->bind("0.0.0.0", port) != NetState::SUCCESS) {
    GLOGE("TCP server err : bind local addr failed");
    return NetState::INTERNAL_ERR;
  }

  if (accept_socket_->listen(10) != NetState::SUCCESS) {
    GLOGE("TCP server err : bind local addr failed");
    return NetState::INTERNAL_ERR;
  }

  scheduler->register_main_event(accept_socket_->fd(), handler_);
  GLOGD("TCP : server init success");

  return NetState::SUCCESS;
}

void TcpServer::start() {
  auto scheduler = TaskScheduler::instance();
  scheduler->run();
}

NetState TcpServer::on_connect(int fd) {
  if (fd != accept_socket_->fd()) {
    GLOGE("Connect failed, cur fd {}, incoming fd {}", accept_socket_->fd(), fd);
    return NetState::INTERNAL_ERR;
  }

  auto conn = std::shared_ptr<TcpConnection>(nullptr);
  accept_socket_->accept(conn);

  std::shared_ptr<EventHandler> connect_handler(nullptr);
  if (connect_handler_factory_) {
    connect_handler = connect_handler_factory_();
  } else {
    connect_handler = std::make_shared<DefaultTcpConnectionHandler>();
  }

  conn->set_handler(connect_handler);
  connetions_.push_back(conn);

  auto scheduler = TaskScheduler::instance();
  scheduler->register_sub_event("connection", conn->fd(), connect_handler);

  return NetState::SUCCESS;
}

}