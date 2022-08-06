#include "task_scheduler.h"

namespace gortsp {

RtspStatus TaskScheduler::register_task(int fd, TaskFunction&& f) {
  ebs_.register_event(fd, f, nullptr);
}

void TaskScheduler::start() {
  ebs_.start();
}

}