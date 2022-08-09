#include "task_scheduler.h"

namespace gortsp {

TaskScheduler::TaskScheduler(size_t max_threads) :
    max_threads_(max_threads),
    thread_pool_(max_threads),
    evt_handler_(EventHandler::create()) {
}

void TaskScheduler::register_task(int fd, EventCb* f) {
  evt_handler_->regist_event(fd, f);
}

void TaskScheduler::register_iotask(int fd,
                                    IOEventCb* r_cb,
                                    IOEventCb* w_cb,
                                    EventCb* e_cb) {
  evt_handler_->register_ioevent(fd, r_cb, w_cb, e_cb);
}

void TaskScheduler::start() {
  evt_handler_->start();
}

}