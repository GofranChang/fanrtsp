#pragma once

#include "common/GORTSP_API.h"
#include <cstdint>
#include <memory>
#include <functional>

#include "net_state.hpp"
#include "event_handler.hpp"
#include "event_target.hpp"

namespace fannetwork {

class DefaultAcceptHandler;

class Socket;

class TcpConnection;

class FANNETWORK_PUBLIC TcpServer : public EventTarget {
public:
  friend class DefaultAcceptHandler;

public:
  TcpServer();

  explicit TcpServer(const std::shared_ptr<EventHandler>& handler);

  explicit TcpServer(EventHandlerFactory& connection_handler_factory);

  TcpServer(const std::shared_ptr<EventHandler>& handler, EventHandlerFactory& connection_handler_factory);

  virtual ~TcpServer() {}

  virtual NetState init(int16_t port, int32_t thread_num);

  virtual void start();

private:
  NetState on_connect(int fd);

private:
  std::shared_ptr<Socket> accept_socket_;

  std::vector<std::shared_ptr<TcpConnection>> connetions_;

  EventHandlerFactory connect_handler_factory_;
};

class FANNETWORK_PUBLIC DefaultAcceptHandler : public EventHandler {
public:
  DefaultAcceptHandler();

public:
  void set_server(TcpServer* srv);

  virtual void on_event(int32_t fd, int16_t evt) override;

  virtual void on_read(int32_t fd, const std::string & msg) override;

private:
  TcpServer* server_;
};

}  // namespace fannetwork
