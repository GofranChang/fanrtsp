#include <iostream>

#include "logger.h"
#include "tcp_server.h"

int main(int argc, const char* argv[]) {
  INITLOGGER(spdlog::level::level_enum::trace, "", false);
  gortsp::TcpServer server;

  server.init(8554);
  return 0;
}