#pragma once

#include <memory>
#include <functional>
#include "task_scheduler.hpp"

namespace fannetwork {

class ReactorImpl;

class Reactor {
public:
  friend class ReactorImpl;

  static std::shared_ptr<Reactor> create();

private:
  Reactor();

public:
  virtual ~Reactor();

public:
  virtual void register_event(int fd, const std::shared_ptr<EventHandler>& handler) = 0;

  virtual void register_ioevent(int fd, const std::shared_ptr<EventHandler>& handler) = 0;

  virtual void register_timer_event() = 0;

  virtual void start() = 0;

  virtual void wake_up() = 0;

  virtual bool is_in_loop() const = 0;
};

}