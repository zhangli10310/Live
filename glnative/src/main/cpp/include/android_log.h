//
// Created by zhangli on 2021/7/12.
//

#ifndef LIVE_ANDROID_LOG_H
#define LIVE_ANDROID_LOG_H

#include "android/log.h"

#define TAG "GL_NATIVE" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)

#endif //LIVE_ANDROID_LOG_H
