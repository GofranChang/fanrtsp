// Baidu VIS-TVC vision SDK
// E-mail : vis-tvc@baidu.com

#pragma once

#include <functional>

namespace gortsp {

enum class RtspStatus : int {
    SUCCESS,
    UNINITIALIZED,
    MULTI_OPERATOR,
    IO_OPERATOR_ERR,
    ILLEGAL_PARAMS,
    INTERNAL_ERR,
    LICENSE_ERR,
    MULTI_INIT,
};

}  // namespace gortsp

namespace std {

template <>
struct hash<::gortsp::RtspStatus> {
    size_t operator()(::gortsp::RtspStatus type) const {
        return std::hash<int>()(static_cast<int>(type));
    }
};

}  // namespace std
