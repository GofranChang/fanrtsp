#include "task_scheduler.hpp"

#include "common/logger.h"
#include "common/thread_pool.hpp"
#include "reactor.h"
#include "event_handler.hpp"

namespace fannetwork {

TaskScheduler::TaskScheduler() : main_reactor_(nullptr),
                                 sub_reactors_({}),
                                 thread_pool_(nullptr) {
}

NetState TaskScheduler::init(size_t thread_nums, const std::vector<std::string>& sub_reactor_name) {
  if (main_reactor_ || !sub_reactors_.empty() || thread_pool_) {
    GLOGE("Init task scheduler failed, already initialized");
    return NetState::MULTI_OPERATOR;
  }

  GLOGD("Init thread size {}", thread_nums + 2);
  thread_pool_.reset(new ThreadPool(thread_nums + 2));

  main_reactor_ = Reactor::create();

  auto start_reactor = [this](const std::string& reactor_name) {
    if ("" == reactor_name) {
      GLOGE("Create reactor failed, already created");
      return;
    }
    auto reactor = Reactor::create();
    // TODO: Add mutex
    this->sub_reactors_.insert(std::make_pair(reactor_name, reactor));
    reactor->start();
  };

  for (auto name : sub_reactor_name) {
    // auto cur_reactor = Reactor::create();
    // sub_reactors_.insert(std::make_pair(reactor, cur_reactor));
    thread_pool_->enqueue(start_reactor, name);
  }

  // thread_pool_->enqueue(start_reactor, connection_reactor_);
  // thread_pool_->enqueue(start_reactor, task_reactor_);

  return NetState::SUCCESS;
}

NetState TaskScheduler::register_main_event(int fd, const std::shared_ptr<EventHandler>& handler) {
  if (!main_reactor_ ) {
    GLOGE("Run task scheduler loop failed, main reactor is null");
    return NetState::UNINITIALIZED;
  }

  GLOGT("Regist main reactor, fd {}", fd);
  main_reactor_->register_event(fd, handler);
  return NetState::SUCCESS;
}

NetState TaskScheduler::register_sub_event(const std::string& reactor_name,
                                         int fd,
                                         const std::shared_ptr<EventHandler>& handler) {
  auto it = sub_reactors_.find(reactor_name);

  if (it == sub_reactors_.end()) {
    GLOGE("Couldn't found reactor {}", reactor_name);
    return NetState::UNINITIALIZED;
  }

  it->second->register_ioevent(fd, handler);
  return NetState::SUCCESS;
}

void TaskScheduler::run() {
  if (!main_reactor_ ) {
    GLOGE("Run task scheduler loop failed, main reactor is null");
    return;
  }

  main_reactor_->start();
}

}