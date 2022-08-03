#! /usr/bin/env bash
###
 # @Descripttion: 
 # @version: 
 # @Author: zhanggaofan <zhanggaofan@baidu.com>
 # @Date: 2021-12-29 12:22:58
 # @LastEditTime: 2022-04-01 18:21:38
### 

### Configuration

# 当前目录
current_dir=$(cd $(dirname $0); pwd)
root_dir=${current_dir}/../

# Djinni IDL file location
djinni_file=$root_dir/generator/rtsp.djinni

# C++ namespace for generated src
namespace="gortsp"

# output directories for generated src
cpp_out=$root_dir/src/interface/generated

# Determine os version and choose correspondent .sh or .bat file accordingly.
os_version="`uname -a`"

rm -rf $cpp_out/*.hpp

# execute the djinni command
$current_dir/deps/djinni/bin/djinni \
    --cpp-out $cpp_out \
    --cpp-namespace $namespace \
    --idl $djinni_file \
    --cpp-export-define GORTSP_PUBLIC \
    --cpp-export-define-header \"common/GORTSP_API.h\"
