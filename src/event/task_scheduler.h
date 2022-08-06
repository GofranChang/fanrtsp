#include <vector>
#include <thread>
#include <functional>

#include "common/thread_pool.hpp"
#include "event/event_wrapper.h"

namespace gortsp {

using message_pack = void*;

class TaskScheduler {
public:
  TaskScheduler(size_t max_threads) :
      max_threads_(max_threads),
      thread_pool_(max_threads) {
  }

  RtspStatus register_task(int fd, TaskFunction&& f);

  // Run loop
  void start();

  void stop();

private:
  size_t max_threads_;
  ThreadPool thread_pool_;

  EventBase ebs_;
};

} // namespace gortsp