#ifndef H_LOG_UTIL_H
#define H_LOG_UTIL_H

#include "pdu_base.h"

enum LogLevel {

    Level_Debug,
    Level_Trace,
    Level_Info,
    Level_Warning,
    Level_Error,
    Level_Fatal,

};


#define LOGD(x, ...) LogImp(Level_Debug, __FILE__, __LINE__,  x , ## __VA_ARGS__)

#define LOGT(x, ...) LogImp(Level_Trace, __FILE__, __LINE__,  x , ## __VA_ARGS__)

#define LOGI(x, ...) LogImp(Level_Info, __FILE__, __LINE__,  x , ## __VA_ARGS__)

#define LOGW(x, ...) LogImp(Level_Warning, __FILE__, __LINE__,  x , ## __VA_ARGS__)

#define LOGE(x, ...) LogImp(Level_Error, __FILE__, __LINE__,  x , ## __VA_ARGS__)

#define LOGF(x, ...) LogImp(Level_Fatal, __FILE__, __LINE__,  x , ## __VA_ARGS__)


void LogImp(LogLevel l, const char *file, int line, const char *msg, ...);

void LogSetLevel(LogLevel);

void printPdu(PDUBase &_pdu);


#endif // H_LOG_UTIL_H
