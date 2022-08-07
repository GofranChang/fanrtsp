#pragma once

#include <string>

#include "socket.h"

namespace gortsp {

class TcpConnection {
public:
  enum class TcpStatus : int {
    TcpUninit,
    TcpConnected,
    TcpClosed,
  };

public:
  TcpConnection();

  ~TcpConnection();

public:
  RtspStatus connect(const std::string& remote_ip, uint16_t remote_port);

  RtspStatus disconnect();

  inline bool connected() const { return socket_.connected(); }

private:
  std::string remote_ip_;
  uint16_t remote_port_;

  std::string local_ip_;
  uint16_t local_port_;

  Socket socket_;
};

}