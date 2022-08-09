#include "task_scheduler.h"

namespace gortsp {

TaskScheduler::TaskScheduler(size_t max_threads) :
    max_threads_(max_threads),
    thread_pool_(max_threads),
    evt_handler_(EventHandler::create()) {
}

RtspStatus TaskScheduler::register_task(int fd, EventCb* f) {
  evt_handler_->regist_event(fd, f);
  return RtspStatus::SUCCESS;
}

void TaskScheduler::start() {
  evt_handler_->start();
}

}