#include "event_handler.h"

#include "network/socket.h"
#include "common/logger.h"

#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"

namespace gortsp {

#define MAX_LINE 1024

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

class BufferEvent {
public:
  friend class EventBase;

private:
  BufferEvent(event_base* base,
              int fd,
              IOEventCb* r_cb,
              IOEventCb* w_cb,
              EventCb* e_cb) :
      ptr_(bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE)),
      ev_buf_(evbuffer_new()),
      read_callback_(r_cb),
      write_callback_(w_cb),
      error_callback_(e_cb) {
    bufferevent_setcb(ptr_, BufferEvent::read_cb,
                      nullptr, BufferEvent::error_cb, nullptr);
    bufferevent_enable(ptr_, EV_READ | EV_WRITE | EV_PERSIST);
    bufferevent_setwatermark(ptr_, EV_READ, 0, 0);
  }

  BufferEvent(const Event&) = delete;

  BufferEvent operator=(const Event&) = delete;

public:
  ~BufferEvent() {
    if (ptr_ != nullptr)
      bufferevent_free(ptr_);

    if (ev_buf_ != nullptr)
      evbuffer_free(ev_buf_);

    ptr_ = nullptr;
    ev_buf_ = nullptr;
  }

public:
  static void read_cb(struct bufferevent *bev, void *ctx) {
    GLOGE("")
    BufferEvent* evt = static_cast<BufferEvent*>(ctx);

    char line[MAX_LINE + 1] = { 0 };
    int n = 0;

    GLOGE("?????")
    while (n = bufferevent_read(bev, line, MAX_LINE), n > 0) {
      line[n] = '\0';
      GLOGE("")
      evbuffer_add(evt->ev_buf_, line, n);

      // const char *x = "==";
      // struct evbuffer_ptr ptr = evbuffer_search(buf, x, strlen(x), 0);
      // if (ptr.pos != -1) {
      //   bufferevent_write_buffer(bev, buf); //使用buffer的方式输出结果
      // }
    }

    GLOGE("????? {}", line)

    if (evt->read_callback_)
      (*(evt->read_callback_))(-1, 0);
  }

  static void write_cb(struct bufferevent *bev, void *ctx) {
    GLOGE("")
    BufferEvent* evt = static_cast<BufferEvent*>(ctx);
    if (evt->write_callback_)
      (*(evt->write_callback_))(-1, 0);
  }

  static void error_cb(struct bufferevent *bev, short what, void *ctx) {
    GLOGE("")
    BufferEvent* evt = static_cast<BufferEvent*>(ctx);
    if (evt->error_callback_)
      (*(evt->error_callback_))(-1, 0);
  }

public:
  inline bufferevent* get() { return ptr_; }

private:
  bufferevent* ptr_;
  evbuffer* ev_buf_;

  IOEventCb* read_callback_;
  IOEventCb* write_callback_;
  EventCb* error_callback_;
}; // class BufferEvent

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

  virtual void register_ioevent(int fd,
                                IOEventCb* read_cb,
                                IOEventCb* write_cb,
                                EventCb* error_cb) override {
    GLOGD("regist io event, fd {}", fd);
    std::unique_ptr<BufferEvent> bev(new BufferEvent(ptr_, fd, read_cb, write_cb, error_cb));
    buffer_events_.push_back(std::move(bev));
  }

  virtual void start() override {
    event_base_loop(ptr_, 0);
  }

private:
  event_base* ptr_;

  std::vector<std::unique_ptr<Event>> events_;

  std::vector<std::unique_ptr<BufferEvent>> buffer_events_;
};

std::shared_ptr<EventHandler> EventHandler::create() {
  return std::shared_ptr<EventHandler>(new EventBase());
}

}