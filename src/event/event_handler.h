#pragma once

#include <string>
#include <functional>
#include <memory>

#include "network/rtsp_status.hpp"

namespace gortsp {

using EventCb = std::function<RtspStatus(int, short)>;

using IOEventCb = std::function<RtspStatus(int, std::string)>;

class EventHandler {
public:
  static std::shared_ptr<EventHandler> create();

public:
  EventHandler() = default;

  virtual ~EventHandler() = default;

public:
  virtual void regist_event(int fd, EventCb* cb) = 0;

  virtual void register_ioevent(int fd,
                                IOEventCb* read_cb,
                                IOEventCb* write_cb,
                                EventCb* error_cb) = 0;

  virtual void start() = 0;

  // virtual void regist_ioevent() = 0;
};

}
