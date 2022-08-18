#include "reactor.h"

#include <map>
#include <queue>
#include <thread>
#include <mutex>

#include "common/logger.h"
#include "event_handler.hpp"

#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"

namespace fannetwork {

struct EventMsg {
  int fd_;
  std::shared_ptr<EventHandler> handler_;
};

class ReactorImpl : public Reactor {
public:
  friend class Reactor;

private:
  ReactorImpl() : run_tid_(std::this_thread::get_id()),
                  evt_base_(event_base_new()),
                  wakeup_event_(nullptr) {
    if (pipe(wakeup_pipe_) != 0) {
      GLOGE("Create reactor wake up pipe failed");
    } else {
      wakeup_event_ = event_new(evt_base_,
                                wakeup_pipe_[0],
                                EV_READ | EV_PERSIST,
                                wakeup_cb,
                                this);

      event_add(wakeup_event_, nullptr);
    }
  }

public:
  virtual ~ReactorImpl() {
    while (!evts_.empty()) {
      auto evt = evts_.front();
      event_free(evt);
      evts_.pop();
    }

    if (nullptr != evt_base_)
      event_base_free(evt_base_);

    evt_base_ = nullptr;
  }

public:
  virtual void register_event(int fd, const std::shared_ptr<EventHandler>& handler) override {
    if (std::this_thread::get_id() == run_tid_) {
      register_event({fd, handler});
    } else {
      std::lock_guard<std::mutex> lck(eventmsg_mutex_);
      event_msgs_.push({fd, handler});
      this->wake_up();
    }
  }

  virtual void register_ioevent(int fd, const std::shared_ptr<EventHandler>& handler) override {
    if (std::this_thread::get_id() == run_tid_) {
      register_ioevent({fd, handler});
    } else {
      std::lock_guard<std::mutex> lck(eventmsg_mutex_);
      ioevent_msgs_.push({fd, handler});
      this->wake_up();
    }
  }

  virtual void register_timer_event() override {
  }

  virtual void start() override {
    if (!evt_base_) {
      GLOGE("Run reactor failed, event base is null");
      return;
    }

    event_base_dispatch(evt_base_);
  }

  virtual void wake_up() override {
    ::write(wakeup_pipe_[1], "w", 1);
  }

  virtual bool is_in_loop() const override { return run_tid_ == std::this_thread::get_id(); }

  static void event_cb(evutil_socket_t fd, short event, void* argc) {
    EventHandler* handler = static_cast<EventHandler*>(argc);
    if (handler) {
      handler->on_event(fd, event);
    }
  }

  static void read_cb(struct bufferevent *bev, void *ctx) {
    size_t read_len = evbuffer_get_length(bufferevent_get_input(bev));

#if 1
    std::string read_buffer(read_len, 0);
    bufferevent_read(bev, const_cast<char*>(read_buffer.c_str()), read_len);

    // GLOGT("Read buffer:\n{}", read_buffer);

    EventHandler* handler = static_cast<EventHandler*>(ctx);
    handler->on_read(bufferevent_getfd(bev), read_buffer);
#endif

#if 0
    auto ebf = bufferevent_get_input(bev);
    auto buffer = (char *)evbuffer_pullup(ebf, -1);
#endif
  }

  static void error_cb(struct bufferevent *bev, short what, void *ctx) {
  }

  static void wakeup_cb(evutil_socket_t fd, short event, void* argc) {
    ReactorImpl* reactor = static_cast<ReactorImpl*>(argc);
    char c[1];
    ::read(reactor->wakeup_pipe_[0], c, 1);
    GLOGT("wakeup_cb flag {}", c);

    {
      std::lock_guard<std::mutex> lck(reactor->eventmsg_mutex_);
      while (!reactor->ioevent_msgs_.empty()) {
        auto msg = reactor->ioevent_msgs_.front();
        reactor->register_ioevent(msg);
        reactor->ioevent_msgs_.pop();
      }
      // event_msgs_.push({fd, handler});
      // this->wake_up();
    }
  }

private:
  void register_event(const EventMsg& msg) {
    if (nullptr == evt_base_) {
      GLOGE("Register event error, event base is null, maybe internal error");
      return;
    }

    event* evt = event_new(evt_base_, msg.fd_, EV_TIMEOUT | EV_READ | EV_PERSIST, event_cb, msg.handler_.get());
    event_add(evt, nullptr);
    evts_.push(evt);
  }

  void register_ioevent(const EventMsg& msg) {
    if (nullptr == evt_base_) {
      GLOGE("Register event error, event base is null, maybe internal error");
      return;
    }

    GLOGE("On ioevent");
    bufferevent* bev = bufferevent_socket_new(evt_base_, msg.fd_, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, read_cb, nullptr, error_cb, msg.handler_.get());
    bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
    bufferevent_setwatermark(bev, EV_READ, 0, 0);
  }

private:
  std::thread::id run_tid_;

  event_base* evt_base_;

  std::queue<event*> evts_;

  std::map<int, EventCb*> evt_cbs_;

  int wakeup_pipe_[2];

  event* wakeup_event_;

  std::queue<EventMsg> event_msgs_;

  std::queue<EventMsg> ioevent_msgs_;

  std::mutex eventmsg_mutex_;
};

std::shared_ptr<Reactor> Reactor::create() {
  Reactor* p = new ReactorImpl;
  return std::shared_ptr<Reactor>(p);
}

Reactor::Reactor() {}

Reactor::~Reactor() {}

}