#pragma once

#include <memory>
#include <map>
#include <vector>
#include <string>
#include "net_state.hpp"

class ThreadPool;

namespace fannetwork {

class EventHandler;

using EventCb = std::function<void(int, short)>;

using IOEventCb = std::function<void(int, std::string)>;

class Reactor;

class TaskScheduler {
public:
  static TaskScheduler* instance() {
    static TaskScheduler* p = new TaskScheduler();
    return p;
  }

private:
  TaskScheduler();

  TaskScheduler(TaskScheduler&) = delete;

  TaskScheduler(TaskScheduler&&) = delete;

  TaskScheduler operator()(TaskScheduler&) = delete;

  TaskScheduler operator()(TaskScheduler&&) = delete;

public:
  ~TaskScheduler() = default;

public:
  NetState init(size_t thread_nums, const std::vector<std::string>& sub_reactor_name);

  NetState register_main_event(int fd, const std::shared_ptr<EventHandler>& handler);

  NetState register_sub_event(const std::string& reactor_name,
                            int fd,
                            const std::shared_ptr<EventHandler>& handler);

  void run();

private:
  std::shared_ptr<Reactor> main_reactor_;

  std::map<std::string, std::shared_ptr<Reactor>> sub_reactors_;

  std::unique_ptr<ThreadPool> thread_pool_;
};

}
