#pragma once

#include <string>
#include <stdint.h>

#include "rtsp_status.hpp"

namespace gortsp {

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

  Socket() :
      type_(SocketType::UDP_SOCKET),
      fd_(-1),
      status_(SocketStat::UNINITIALIZED),
      connected_(false) {}

  explicit Socket(SocketType type) :
      type_(type),
      fd_(-1),
      status_(SocketStat::UNINITIALIZED),
      connected_(false) {}

  ~Socket();

public:
  RtspStatus set_type(SocketType type);

  RtspStatus create();

  RtspStatus close();

  RtspStatus bind(const std::string& ip, uint16_t port);

  RtspStatus listen(int backlog);

  RtspStatus accept(Socket& connsock);

  RtspStatus connect(const std::string& ip,
                     uint16_t port,
                     int timeout);

  RtspStatus disconnect();

  RtspStatus set_non_block() {}

  RtspStatus set_block(int timeout) {}

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
