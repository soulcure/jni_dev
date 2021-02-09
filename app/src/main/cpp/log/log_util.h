#ifndef LOG_UTIL_H
#define LOG_UTIL_H

#include  <android/log.h>

// log标签
#define  TAG    "JNI"
#define  DEBUG   "android"

enum LogLevel {
    Level_Debug,
    Level_Trace,
    Level_Info,
    Level_Warning,
    Level_Error,
    Level_Fatal,
};

#ifdef DEBUG
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#else
#define LOGD(x, ...) LogImp(Level_Debug, __FILE__, __LINE__,  x , ## __VA_ARGS__)

#define LOGT(x, ...) LogImp(Level_Trace, __FILE__, __LINE__,  x , ## __VA_ARGS__)

#define LOGI(x, ...) LogImp(Level_Info, __FILE__, __LINE__,  x , ## __VA_ARGS__)

#define LOGW(x, ...) LogImp(Level_Warning, __FILE__, __LINE__,  x , ## __VA_ARGS__)

#define LOGE(x, ...) LogImp(Level_Error, __FILE__, __LINE__,  x , ## __VA_ARGS__)

#define LOGF(x, ...) LogImp(Level_Fatal, __FILE__, __LINE__,  x , ## __VA_ARGS__)

void LogImp(LogLevel l, const char *file, int line, const char *msg, ...);

#endif




#endif // LOG_UTIL_H
