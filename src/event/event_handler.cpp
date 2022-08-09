#include "event_handler.h"

#include "network/socket.h"
#include "common/logger.h"

#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"

namespace gortsp {

class Event {
public:
  friend class EventBase;

private:
  Event(event_base* base,
        int fd,
        EventCb* cb) :
      ptr_(event_new(base, fd, EV_TIMEOUT | EV_READ | EV_PERSIST, Event::event_cb, this)),
      callback_(cb) {}

  Event(const Event&) = delete;

  Event operator=(const Event&) = delete;

public:
  ~Event() {
    if (ptr_ != nullptr)
      event_free(ptr_);

    ptr_ = nullptr;
  }

public:
  static void event_cb(evutil_socket_t fd, short event, void* argc) {
    Event* evt = static_cast<Event*>(argc);
    (*(evt->callback_))(fd, event);
  }

public:
  inline event* get() { return ptr_; }

private:
  event* ptr_;

  EventCb* callback_;
}; // class Event

class EventBase : public EventHandler {
public:
  friend class EventHandler;

private:
  EventBase() : ptr_(event_base_new()) {}

public:
  virtual ~EventBase() {}

public:
  virtual void regist_event(int fd, EventCb* cb) override {
    std::unique_ptr<Event> ev(new Event(ptr_, fd, cb));
    event_add(ev->get(), nullptr);
    events_.push_back(std::move(ev));
  }

  virtual void start() override {
    event_base_loop(ptr_, 0);
  }

private:
  event_base* ptr_;

  std::vector<std::unique_ptr<Event>> events_;
};

std::shared_ptr<EventHandler> EventHandler::create() {
  return std::shared_ptr<EventHandler>(new EventBase());
}

}