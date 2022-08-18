#pragma once

#include "net_state.hpp"

#include <memory>
#include "event_handler.hpp"
#include "event_target.hpp"

namespace fannetwork {

class Socket;

class TcpConnection : public EventTarget {
public:
  static std::shared_ptr<TcpConnection> create();

  static std::shared_ptr<TcpConnection> create(const std::shared_ptr<Socket>& socket);

private:
  TcpConnection();

  explicit TcpConnection(const std::shared_ptr<Socket>& socket);

public:
  ~TcpConnection();

public:
  NetState set_conneted_socket(const std::shared_ptr<Socket>& socket);

  NetState read(std::string& content);

  NetState write(const std::string& content);

  void disconnect();

  inline bool is_connected() const { return connecting_; }

  int fd();

private:
  std::shared_ptr<Socket> socket_;

  bool connecting_;
};

class DefaultTcpConnectionHandler : public EventHandler {
public:
  DefaultTcpConnectionHandler();

public:
  void set_connection(TcpConnection* srv);

  virtual void on_event(int32_t fd, int16_t evt) override;

  virtual void on_read(int32_t fd, const std::string & msg) override;

private:
  TcpConnection* connection_;
};

}