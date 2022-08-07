#include "socket.h"

#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "common/logger.h"

namespace gortsp {

Socket::~Socket() {
  close();
}

RtspStatus Socket::set_type(SocketType type) {
  if (status_ != SocketStat::UNINITIALIZED || fd_ >= 0) {
    GLOGE("Set type failed, fd already initialize");
    return RtspStatus::MULTI_OPERATOR;
  }

  type_ = type;
  return RtspStatus::SUCCESS;
}

RtspStatus Socket::create() {
  if (status_ > SocketStat::UNINITIALIZED) {
    GLOGE("Create socket failed, current fd {}, already created", fd_);
    return RtspStatus::MULTI_OPERATOR;
  }

  int socket_domain = AF_INET;
  int SocketType = (SocketType::TCP_SOCKET == type_) ? SOCK_STREAM : SOCK_DGRAM;
  int socket_protocal = (SocketType::TCP_SOCKET == type_) ? IPPROTO_TCP : IPPROTO_UDP;

  fd_ = ::socket(socket_domain, SocketType, socket_protocal);
  if (fd_ < 0) {
    GLOGE("Create socket error : {}", strerror(errno));
    return RtspStatus::IO_OPERATOR_ERR;
  }

  change_status(SocketStat::CREATED);
  GLOGT("Create {} socket succes, fd {}", ((SocketType::TCP_SOCKET == type_) ? "TCP" : "UDP"), fd_);
  return RtspStatus::SUCCESS;
}

RtspStatus Socket::close() {
  if (fd_ >= 0) {
    ::close(fd_);
  }

  fd_ = -1;
  connected_ = false;
  change_status(SocketStat::UNINITIALIZED);
  return RtspStatus::SUCCESS;
}

RtspStatus Socket::bind(const std::string& ip, uint16_t port) {
  if (status_ != SocketStat::CREATED) {
    GLOGE("Bind to addr {}:{} failed, please create socket at first", ip, port);
    return RtspStatus::UNINITIALIZED;
  }

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip.c_str());
  addr.sin_port = htons(port);

  if (::bind(fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    GLOGE("Bind fd error : {}", strerror(errno));
    return RtspStatus::IO_OPERATOR_ERR;
  }

  change_status(SocketStat::BINDED);
  GLOGT("Bind fd {} to addr {}:{} success", fd_, ip, port);
  return RtspStatus::SUCCESS;
}

RtspStatus Socket::listen(int backlog) {
  if (type_ != SocketType::TCP_SOCKET || status_ != SocketStat::BINDED) {
    GLOGE("Listen failed, socket type {}, cur status {}", type_, status_);
    return RtspStatus::ILLEGAL_PARAMS;
  }

  if (::listen(fd_, backlog)) {
    GLOGE("Listen socket {} failed, {}", fd_, strerror(errno));
    return RtspStatus::IO_OPERATOR_ERR;
  }

  change_status(SocketStat::LISTENED);
  return RtspStatus::SUCCESS;
}

RtspStatus Socket::accept(Socket& connsock) {
  if (type_ != SocketType::TCP_SOCKET || status_ != SocketStat::LISTENED) {
    GLOGE("Listen failed, socket type {}, cur status {}", type_, status_);
    return RtspStatus::ILLEGAL_PARAMS;
  }

  // if (connsock.type() != SocketType::TCP_SOCKET) {
  //   GLOGE("Accept failed, input socket is UDP socket");
  //   return RtspStatus::ILLEGAL_PARAMS;
  // }

  change_status(SocketStat::ACCEPTING);

  struct sockaddr_in addr = { 0 };
  socklen_t addrlen = sizeof(struct sockaddr_in);

  int connected_fd = ::accept(fd_, (struct sockaddr*)&addr, &addrlen);
  connsock.set_fd(connected_fd);

  // TODO: Set none block fd;
  // setNonBlockAndCloseOnExec(connfd);
  // ignoreSigPipeOnSocket(connfd);

  return RtspStatus::SUCCESS;
}

RtspStatus Socket::connect(const std::string& ip,
                           uint16_t port,
                           int timeout) {
  if (type_ != SocketType::TCP_SOCKET || status_ != SocketStat::BINDED) {
    GLOGE("Connect failed, socket type {}, cur status {}", type_, status_);
    return RtspStatus::ILLEGAL_PARAMS;
  }

  if (connected()) {
    GLOGE("Connect failed, already connected");
    return RtspStatus::MULTI_OPERATOR;
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

  return RtspStatus::SUCCESS;
}

RtspStatus Socket::disconnect() {
  if (!connected()) {
    // TODO:
  }

  return this->close();
}

RtspStatus Socket::set_non_block() {
  if (status_ < SocketStat::CREATED) {
    GLOGE("Please create socket at firsrt");
    return RtspStatus::UNINITIALIZED;
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
    return RtspStatus::IO_OPERATOR_ERR;
  }
  if (!(flags & O_NONBLOCK)) {
    if (fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
      GLOGW("fcntl({}, F_SETFL)", fd_);
      RtspStatus::IO_OPERATOR_ERR;
    }
  }
#endif

  return RtspStatus::SUCCESS;
}

RtspStatus Socket::set_block(int timeout) {
  return RtspStatus::UNINITIALIZED;
}

RtspStatus Socket::set_fd(int fd) {
  if (fd_ > 0) {
    GLOGE("Set fd failed, cur fd {}", fd_);
    return RtspStatus::MULTI_OPERATOR;
  }

  fd_ = fd;
  return RtspStatus::SUCCESS;
}

}