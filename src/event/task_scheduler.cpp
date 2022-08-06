#include "task_scheduler.h"

namespace gortsp {

RtspStatus TaskScheduler::register_task(int fd, TaskFunction&& f, void* args) {
  ebs_.register_event(fd, f, args);
}

void TaskScheduler::start() {
  ebs_.start();
}

void TaskScheduler::start() {
  ebs_.start();
}

}