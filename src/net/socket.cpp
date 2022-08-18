#include "socket.h"

#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "common/logger.h"
#include "tcp_connection.hpp"

namespace fannetwork {

std::shared_ptr<Socket> Socket::create(SocketType type) {
  auto p = new Socket(type);
  return std::shared_ptr<Socket>(p);
}

std::shared_ptr<Socket> Socket::create_from_accept(int fd) {
  auto p = new Socket(fd);
  return std::shared_ptr<Socket>(p);
}

Socket::Socket(SocketType type) :
    type_(type),
    fd_(-1),
    status_(SocketStat::UNINITIALIZED),
    connected_(false) {
}

Socket::Socket(int fd) :
    type_(SocketType::TCP_SOCKET),
    fd_(fd),
    status_(SocketStat::CONNETED),
    connected_(false) {
}

Socket::~Socket() {
  close();
}

NetState Socket::open() {
  if (status_ > SocketStat::UNINITIALIZED || fd_ >= 0) {
    GLOGE("Create socket failed, current fd {}, already created", fd_);
    return NetState::MULTI_OPERATOR;
  }

  int socket_domain = AF_INET;
  int socket_type = (SocketType::TCP_SOCKET == type_) ? SOCK_STREAM : SOCK_DGRAM;
  int socket_protocal = (SocketType::TCP_SOCKET == type_) ? IPPROTO_TCP : IPPROTO_UDP;

  fd_ = ::socket(socket_domain, socket_type, socket_protocal);
  if (fd_ < 0) {
    GLOGE("Create socket error : {}", strerror(errno));
    return NetState::IO_OPERATOR_ERR;
  }

  change_status(SocketStat::OPENED);
  GLOGT("Create {} socket succes, fd {}", ((SocketType::TCP_SOCKET == type_) ? "TCP" : "UDP"), fd_);
  return NetState::SUCCESS;
}

NetState Socket::close() {
  GLOGT("Close socket {}", fd_);
  if (fd_ >= 0)
    ::close(fd_);

  fd_ = -1;
  connected_ = false;
  change_status(SocketStat::UNINITIALIZED);
  return NetState::SUCCESS;
}

NetState Socket::set_non_block() {
  if (status_ < SocketStat::OPENED) {
    GLOGE("Please create socket at firsrt");
    return NetState::UNINITIALIZED;
  }

#ifdef _WIN32
  unsigned long nonblocking = 1;
  if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR) {
    event_sock_warn(fd, "fcntl(%d, F_GETFL)", (int)fd);
    return -1;
  }

#else
  int flags;
  if ((flags = fcntl(fd_, F_GETFL, NULL)) < 0) {
    GLOGW("fcntl({}, F_GETFL)", fd_);
    return NetState::IO_OPERATOR_ERR;
  }
  if (!(flags & O_NONBLOCK)) {
    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
      GLOGW("fcntl({}, F_SETFL)", fd_);
      // NetState::IO_OPERATOR_ERR;
    }
  }
#endif

  return NetState::SUCCESS;
}

NetState Socket::bind(const std::string& ip, uint16_t port) {
  if (status_ != SocketStat::OPENED) {
    GLOGE("Bind to addr {}:{} failed, please create socket at first", ip, port);
    return NetState::UNINITIALIZED;
  }

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (::bind(fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    GLOGE("Bind fd error : {}", strerror(errno));
    return NetState::IO_OPERATOR_ERR;
  }

  change_status(SocketStat::BINDED);
  GLOGT("Bind fd {} to addr {}:{} success", fd_, ip, port);
  return NetState::SUCCESS;
}

NetState Socket::listen(int backlog) {
  if (type_ != SocketType::TCP_SOCKET || status_ != SocketStat::BINDED) {
    GLOGE("Listen failed, socket type {}, cur status {}", type_, status_);
    return NetState::ILLEGAL_PARAMS;
  }

  if (::listen(fd_, backlog)) {
    GLOGE("Listen socket {} failed, {}", fd_, strerror(errno));
    return NetState::IO_OPERATOR_ERR;
  }

  change_status(SocketStat::LISTENING);
  return NetState::SUCCESS;
}

NetState Socket::accept(std::shared_ptr<TcpConnection>& connection) {
  if (type_ != SocketType::TCP_SOCKET || status_ != SocketStat::LISTENING) {
    GLOGE("Listen failed, socket type {}, cur status {}", type_, status_);
    return NetState::ILLEGAL_PARAMS;
  }

  struct sockaddr_in addr = { 0 };
  socklen_t addrlen = sizeof(struct sockaddr_in);

  int fd = ::accept(fd_, (struct sockaddr*)&addr, &addrlen);
  auto clisock = Socket::create_from_accept(fd);

  connection = TcpConnection::create(clisock);
  GLOGT("Start accept, local fd {}, accepted fd {}", fd_, clisock->fd());

  return NetState::SUCCESS;
}

NetState Socket::connect(const std::string& ip,
                         uint16_t port,
                         int timeout,
                         std::shared_ptr<TcpConnection>& connection) {
#if 0
  if (type_ != SocketType::TCP_SOCKET || status_ != SocketStat::BINDED) {
    GLOGE("Connect failed, socket type {}, cur status {}", type_, status_);
    return NetState::ILLEGAL_PARAMS;
  }

  if (connected()) {
    GLOGE("Connect failed, already connected");
    return NetState::MULTI_OPERATOR;
  }

  if (timeout > 0) {
    set_non_block();
  }

  struct sockaddr_in addr = { 0 };
  socklen_t addrlen = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip.c_str());

  if (::connect(fd_, (struct sockaddr*)&addr, addrlen) < 0) {
    if (timeout > 0) {
      fd_set fd_wrirte;
      FD_ZERO(&fd_wrirte);
      FD_SET(fd_, &fd_wrirte);
      struct timeval tv = { timeout / 1000, timeout % 1000 * 1000 };
      select(fd_ + 1, nullptr, &fd_wrirte, nullptr, &tv);
      if (FD_ISSET(fd_, &fd_wrirte)) {
        connected_ = true;
      }

      set_block(timeout);
    } else {
      connected_ = false;
    }
  }
#endif

  return NetState::SUCCESS;
}

NetState Socket::disconnect() {
  if (!connected()) {
    // TODO:
  }

  return this->close();
}

}