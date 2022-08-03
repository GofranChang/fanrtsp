// Baidu VIS-TVC vision SDK
// E-mail : vis-tvc@baidu.com

#pragma once

#include "common/GORTSP_API.h"
#include <cstdint>
#include <memory>
#include <string>

namespace gortsp {

class GORTSP_PUBLIC RtspServer {
public:
    virtual ~RtspServer() {}

    static std::shared_ptr<RtspServer> create(const std::string & ip, int16_t port);
};

}  // namespace gortsp
