#pragma once

#include <string>
#include <memory>

#include "socket.h"

namespace gortsp {

class TcpConnection {
public:
  enum class TcpStatus : int {
    TcpUninit,
    TcpConnected,
    TcpClosed,
  };

  friend class Socket;

public:
  std::shared_ptr<TcpConnection> create();

private:
  TcpConnection();

  ~TcpConnection();

public:
  RtspStatus accept(Socket& socket);

  RtspStatus connect(const std::string& remote_ip, uint16_t remote_port);

  RtspStatus disconnect();

  RtspStatus send(const std::string& data);

  RtspStatus recv();

  inline int fd() const { return socket_.fd(); };

  inline bool connected() const { return socket_.connected(); }

private:
  std::string remote_ip_;
  uint16_t remote_port_;

  std::string local_ip_;
  uint16_t local_port_;

  std::shared_ptr<Socket> socket_;
};

}