#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#ifdef LOGGING
#define LOG(logmessage, ...) ({ fprintf(stderr, "[LOG]  " logmessage "\n" __VA_OPT__(, ) __VA_ARGS__); })
#define WARN(warnmessage, ...) ({ fprintf(stderr, "[WARN] " warnmessage "\n" __VA_OPT__(, ) __VA_ARGS__); })
#else
#define LOG(logmessage, ...) ({})
#define WARN(warnmessage, ...) ({})
#endif

#endif  // LOG_H
