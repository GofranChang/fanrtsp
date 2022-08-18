/*
 * @Descripttion: 
 * @version: 
 * @Author: zhanggaofan <zhanggaofan@baidu.com>
 * @Date: 2021-08-25 15:43:48
 * @LastEditTime: 2021-11-03 14:31:14
 */
// Interface visibility
#pragma once

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_DLL
#ifdef __GNUC__
#define FANNETWORK_PUBLIC __attribute__((dllexport))
#else // ifdef __GNUC__
#define FANNETWORK_PUBLIC __declspec(dllexport)
#endif // ifdef __GNUC__
#else // ifdef BUILDING_DLL
#ifdef __GNUC__
#define FANNETWORK_PUBLIC __attribute__((dllimport))
#else // ifdef __GNUC__
#define FANNETWORK_PUBLIC __declspec(dllimport)
#endif // ifdef __GNUC__
#endif // ifdef BUILDING_DLL
#define FANNETWORK_LOCAL
#else // if defined _WIN32 || defined __CYGWIN__
#if __GNUC__ >= 4
#define FANNETWORK_PUBLIC __attribute__((visibility("default")))
#define FANNETWORK_LOCAL __attribute__((visibility("hidden")))
#else // if __GNUC__ >= 4
#define FANNETWORK_PUBLIC
#define FANNETWORK_LOCAL
#endif // if __GNUC__ >= 4
#endif // if defined _WIN32 || defined __CYGWIN__
