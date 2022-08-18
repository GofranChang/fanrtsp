#pragma once

#include "event_handler.hpp"

namespace fannetwork {

class EventTarget {
public:
  EventTarget() : handler_(nullptr) {}

  explicit EventTarget(const std::shared_ptr<EventHandler>& handler) : handler_(handler) {}

  ~EventTarget() = default;

public:
  void set_handler(const std::shared_ptr<EventHandler>& handler) {
    if (handler_) return;
    handler_ = handler;
  }

protected:
  std::shared_ptr<EventHandler> handler_;
};

}