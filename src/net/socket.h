#pragma once

#include <string>
#include <memory>

#include "net_state.hpp"

namespace fannetwork {

class TcpConnection;

class Socket {
public:
  enum class SocketType : int {
    TCP_SOCKET,
    UDP_SOCKET,
  };

  enum class SocketStat : int {
    UNINITIALIZED,
    OPENED,
    BINDED,
    LISTENING,
    ACCEPTING,
    CONNETED,
  };

public:
  static std::shared_ptr<Socket> create(SocketType type);

private:
  static std::shared_ptr<Socket> create_from_accept(int fd);

private:
  explicit Socket(SocketType type);

  explicit Socket(int fd);

public:
  ~Socket();

public:
  NetState open();

  NetState close();

  NetState bind(const std::string& ip, uint16_t port);

  NetState listen(int backlog);

  NetState accept(std::shared_ptr<TcpConnection>& connection);

  NetState connect(const std::string& ip,
                   uint16_t port,
                   int timeout,
                   std::shared_ptr<TcpConnection>& connection);

  NetState disconnect();

  NetState set_non_block();

  NetState set_block(int timeout);

  inline SocketType type() const { return type_; }

  inline int fd() const { return fd_; }

  inline bool connected() const { return connected_; }

  SocketStat stat() const { return status_; }

private:
  inline void change_status(SocketStat stat) { status_ = stat; }

private:
  SocketType type_;

  int fd_;

  SocketStat status_;

  bool connected_;
};

}