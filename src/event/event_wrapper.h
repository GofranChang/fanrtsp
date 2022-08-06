#include <vector>
#include <string>
#include <functional>

#include "network/socket.h"

#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"

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
    ev = event_new(
        ptr_,
        fd,
        EV_TIMEOUT | EV_READ | EV_PERSIST,
        cb.target<void(int, short, void*)>(),
        argc);

    evts_.push_back(ev);
  }

  // void add_event(Event* evt) {}

private:
  event_base* ptr_;

  std::vector<event*> evts_;
};

}
