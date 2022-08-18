#pragma once

#include <functional>

namespace fannetwork {

enum class NetState : int {
  SUCCESS,
  UNINITIALIZED,
  MULTI_OPERATOR,
  IO_OPERATOR_ERR,
  ILLEGAL_PARAMS,
  INTERNAL_ERR,
  LICENSE_ERR,
  MULTI_INIT,
};

}  // namespace fannetwork

namespace std {

template <>
struct hash<::fannetwork::NetState> {
  size_t operator()(::fannetwork::NetState type) const {
    return std::hash<int>()(static_cast<int>(type));
  }
};

}  // namespace std
