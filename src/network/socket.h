#pragma once

#include <string>
#include <stdint.h>

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

  Socket();

  explicit Socket(SocketType type);

  Socket(Socket&&);

  ~Socket();

  RtspStatus move(Socket& other);

private:
  Socket(const Socket&) = delete;

  Socket operator=(const Socket&) = delete;

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

  RtspStatus set_fd(int fd);

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
