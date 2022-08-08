#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>

#include "network/socket.h"
#include "common/logger.h"

#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"

namespace gortsp {

using TaskFunction = std::function<void(int, short, void*)>;

class EventBase;

class Event {
public:
  friend class EventBase;

private:
  Event(event_base* base,
        int fd,
        TaskFunction& cb,
        void* argc) :
      ptr_(event_new(base, fd, EV_TIMEOUT | EV_READ | EV_PERSIST, *cb.target<event_callback_fn>(), argc)) {}

  Event(const Event&) = delete;

  Event operator=(const Event&) = delete;

public:
  ~Event() {
    if (ptr_ != nullptr)
      event_free(ptr_);

    ptr_ = nullptr;
  }

public:
  inline event* get() { return ptr_; }

private:
  event* ptr_;
}; // class Event

class EventBase {
public:
  EventBase() :
      ptr_(event_base_new()) {}

  ~EventBase() {
    if (ptr_ != nullptr)
      event_base_free(ptr_);

    ptr_ = nullptr;
  }

private:
  EventBase(const EventBase&) = delete;

  EventBase operator=(const EventBase&) = delete;

public:
  inline std::string method() const { return event_base_get_method(ptr_); }

  inline event_base* get() { return ptr_; }

  void register_event(int fd, TaskFunction& cb, void* argc) {
    std::unique_ptr<Event> ev(new Event(ptr_, fd, cb, argc));
    event_add(ev->get(), nullptr);
    events_.push_back(std::move(ev));
  }

  void register_bufferevent(int fd,
                            TaskFunction& read_cb,
                            TaskFunction& write_cb,
                            TaskFunction& err_cb) {}

  void start() {
    int res = event_base_loop(ptr_, 0);
    GLOGE("{}", res);
  }

private:
  event_base* ptr_;

  std::vector<std::unique_ptr<Event>> events_;
}; // class EventBase

}
