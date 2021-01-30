//
// Created by chenqiongyao on 2021/1/25.
//

#include "log_util.h"
#include <stdio.h>
#include <stdarg.h>

#define BUF_MAX 1024

int VSLogStub(LogLevel l, const char *file, int line, const char *msg, va_list ap) {
    int bytes = 0;
    char data[BUF_MAX] = "";

    bytes += snprintf(data + bytes, BUF_MAX - 1 - bytes, "%02d; %04d; %s  \n", l, line, file);

    if (bytes < BUF_MAX - 1) {
        bytes += vsnprintf(data + bytes, BUF_MAX - 1 - bytes, msg, ap);
    }

    printf("%s\n", data);

    return 1;
}

void LogImp(LogLevel l, const char *file, int line, const char *msg, ...) {
    va_list ap;
    int ret_val = 0;

    va_start(ap, msg);
    ret_val = VSLogStub(l, file, line, msg, ap);
    va_end(ap);
}




