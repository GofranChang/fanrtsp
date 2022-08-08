#include "task_scheduler.h"

namespace gortsp {

RtspStatus TaskScheduler::register_task(int fd, EventTaskCb* f) {
  ebs_.register_event(fd, f);
  return RtspStatus::SUCCESS;
}

void TaskScheduler::start() {
  ebs_.start();
}

}