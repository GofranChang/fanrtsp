#pragma once

#include <vector>
#include <thread>
#include <memory>
#include <functional>

#include "common/thread_pool.hpp"
#include "event/event_handler.h"

namespace gortsp {

using message_pack = void*;

class TaskScheduler {
public:
  explicit TaskScheduler(size_t max_threads);

  void register_task(int fd, EventCb* f);

  void register_iotask(int fd,
                       IOEventCb* r_cb,
                       IOEventCb* w_cb,
                       EventCb* e_cb);

  // Run loop
  void start();

  void stop();

private:
  size_t max_threads_;
  ThreadPool thread_pool_;

  std::shared_ptr<EventHandler> evt_handler_;
};

} // namespace gortsp