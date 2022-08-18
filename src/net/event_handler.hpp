#pragma once

#include "common/GORTSP_API.h"
#include <cstdint>
#include <vector>

namespace fannetwork {

class EventHandler;

using EventHandlerFactory = std::function<std::shared_ptr<EventHandler>()>;

class FANNETWORK_PUBLIC EventHandler {
public:
  virtual ~EventHandler() {}

  virtual void on_event(int32_t fd, int16_t evt) = 0;

  virtual void on_read(int32_t fd, const std::string & msg) = 0;
};

}  // namespace fannetwork
