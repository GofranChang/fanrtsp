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

using EventTaskCb = std::function<RtspStatus(int, short)>;

using EventIOTaskCb = std::function<RtspStatus()>;

class EventBase;

class Event {
public:
  friend class EventBase;

private:
  Event(event_base* base,
        int fd,
        EventTaskCb* cb) :
      ptr_(event_new(base, fd, EV_TIMEOUT | EV_READ | EV_PERSIST, Event::event_cb, cb)) {}

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
    EventTaskCb* cb = static_cast<EventTaskCb*>(argc);
    (*cb)(fd, event);
  }

public:
  inline event* get() { return ptr_; }

private:
  event* ptr_;
}; // class Event

#if 0
class BufferEvent {
public:
  friend class EventBase;

private:
  BufferEvent(event_base* base,
              int fd,
              EventTaskCb& read_cb,
              EventTaskCb& write_cb,
              EventTaskCb& error_cb,
              void* argc) :
      ptr_(bufferevent_socket_new(base, fd, EV_TIMEOUT | EV_READ | EV_PERSIST)) {
    bufferevent_setcb(ptr_,
                      nullptr,
                      nullptr,
                      nullptr,
                      argc);
  }

  BufferEvent(const Event&) = delete;

  BufferEvent operator=(const Event&) = delete;

public:
  ~BufferEvent() {
    if (ptr_ != nullptr)
      bufferevent_free(ptr_);

    ptr_ = nullptr;
  }

public:
  static void read_cb(struct bufferevent *bev, void *ctx) {
  }

  static void write_cb(struct bufferevent *bev, void *ctx) {
  }

  static void error_cb(evutil_socket_t fd, short event, void* argc) {
  }

public:
  inline bufferevent* get() { return ptr_; }

private:
  bufferevent* ptr_;
}; // class BufferEvent
#endif

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

  void register_event(int fd, EventTaskCb* cb) {
    std::unique_ptr<Event> ev(new Event(ptr_, fd, cb));
    event_add(ev->get(), nullptr);
    events_.push_back(std::move(ev));
  }

  void register_bufferevent(int fd,
                            EventTaskCb& read_cb,
                            EventTaskCb& write_cb,
                            EventTaskCb& err_cb) {}

  void start() {
    int res = event_base_loop(ptr_, 0);
    GLOGE("{}", res);
  }

private:
  event_base* ptr_;

  std::vector<std::unique_ptr<Event>> events_;
}; // class EventBase

}
