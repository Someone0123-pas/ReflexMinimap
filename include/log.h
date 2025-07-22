#ifndef LOG_H
#define LOG_H

#include <stdio.h>

extern bool g_logging_enabled;

#define LOG(logmessage, ...)                                                       \
    ({                                                                             \
        if (g_logging_enabled) {                                                   \
            fprintf(stderr, "[LOG]  " logmessage "\n" __VA_OPT__(, ) __VA_ARGS__); \
        }                                                                          \
    })
#define WARN(warnmessage, ...)                                                      \
    ({                                                                              \
        if (g_logging_enabled) {                                                    \
            fprintf(stderr, "[WARN] " warnmessage "\n" __VA_OPT__(, ) __VA_ARGS__); \
        }                                                                           \
    })

#endif  // LOG_H
