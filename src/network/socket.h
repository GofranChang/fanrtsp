#pragma once

#include <string>
#include <stdint.h>
#include <memory>

#include "rtsp_status.hpp"

namespace gortsp {

class TcpConnection;

class Socket {
public:
  enum class SocketType : int {
    TCP_SOCKET,
    UDP_SOCKET,
  };

  enum class SocketStat : int {
    UNINITIALIZED,
    CREATED,
    BINDED,
    LISTENED,
    ACCEPTING,
    CONNETED,
  };

public:
  static std::shared_ptr<Socket> create();

  static std::shared_ptr<Socket> create(SocketType type);

private:
  static std::shared_ptr<Socket> create_from_accept(int fd);

private:
  Socket();

  explicit Socket(SocketType type);

  explicit Socket(int fd);

  ~Socket();

private:
  Socket(Socket&&) = default;

  Socket(const Socket&) = default;

  Socket operator=(const Socket&) = default;

public:
  RtspStatus set_type(SocketType type);

  RtspStatus create();

  RtspStatus close();

  RtspStatus bind(const std::string& ip, uint16_t port);

  RtspStatus listen(int backlog);

  RtspStatus accept(TcpConnection& conn);

  RtspStatus connect(const std::string& ip,
                     uint16_t port,
                     int timeout);

  RtspStatus disconnect();

  RtspStatus send(const std::string& data);

  RtspStatus set_non_block();

  RtspStatus set_block(int timeout);

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

} // namespace gortsp
