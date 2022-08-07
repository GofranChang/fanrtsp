#pragma once

#include <vector>
#include <string>
#include <functional>

#include "network/socket.h"
#include "common/logger.h"

#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"

void xxx(evutil_socket_t fd, short event, void *arg);

namespace gortsp {

// class Event;

using TaskFunction = std::function<void(int, short, void*)>;

class EventBase {
public:
  EventBase() : ptr_(event_base_new()) {}

  ~EventBase() {
    if (ptr_ != nullptr)
      event_base_free(ptr_);

    ptr_ = nullptr;
  }

public:
  inline std::string method() const {
    return event_base_get_method(ptr_);
  }

  inline event_base* get() {
    return ptr_;
  }

  void register_event(int fd, TaskFunction& cb, void* argc) {
    // if (socket.stat() < Socket::SocketStat::CREATED) {
      // TODO:
    // }

    event *ev;
    cb(1, 1, nullptr);
    typedef void (*event_cb)(evutil_socket_t, short, void *);

    ev = event_new(
        ptr_,
        fd,
        EV_TIMEOUT | EV_READ | EV_PERSIST,
        *cb.target<event_cb>(),
        argc);

    event_add(ev, nullptr);
    evts_.push_back(ev);
  }

  void start() {
    // event_base_dispatch(ptr_);
    int res = event_base_loop(ptr_, 0);
    GLOGE("{}", res);
  }

  // void add_event(Event* evt) {}

private:
  event_base* ptr_;

  std::vector<event*> evts_;
};

}
