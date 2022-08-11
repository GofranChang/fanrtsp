#include <iostream>

#include "common/logger.h"
#include "network/tcp_server.h"

int main(int argc, const char* argv[]) {
  INITLOGGER(spdlog::level::level_enum::trace, "", false);
  gortsp::TcpServer server;

  server.init(atoi(argv[1]));
  server.start();
  return 0;
}