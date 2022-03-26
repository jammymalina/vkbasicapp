#ifndef LOGGER_H
#define LOGGER_H

#include <SDL2/SDL.h>
#include <stdlib.h>

typedef enum LogSeverity {
    LOG_SEVERITY_DEBUG,
    LOG_SEVERITY_INFO,
    LOG_SEVERITY_WARNING,
    LOG_SEVERITY_ERROR,
} LogSeverity;

#define log_debug(message, ...)                                                                                        \
    do {                                                                                                               \
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, message "\n", ##__VA_ARGS__);                                       \
    } while (0)

#define log_info(message, ...)                                                                                         \
    do {                                                                                                               \
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, message "\n", ##__VA_ARGS__);                                        \
    } while (0)

#define log_error(message, ...)                                                                                        \
    do {                                                                                                               \
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, message "\n", ##__VA_ARGS__);                                       \
    } while (0)

#define log_warning(message, ...)                                                                                      \
    do {                                                                                                               \
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, message "\n", ##__VA_ARGS__);                                        \
    } while (0)

#define log(log_severity, message, ...)                                                                                \
    do {                                                                                                               \
        switch (log_severity) {                                                                                        \
            case LOG_SEVERITY_DEBUG:                                                                                   \
                log_debug(message, __VA_ARGS__);                                                                       \
                break;                                                                                                 \
            case LOG_SEVERITY_INFO:                                                                                    \
                log_info(message, __VA_ARGS__);                                                                        \
                break;                                                                                                 \
            case LOG_SEVERITY_WARNING:                                                                                 \
                log_warning(message, __VA_ARGS__);                                                                     \
                break;                                                                                                 \
            case LOG_SEVERITY_ERROR:                                                                                   \
                log_error(message, __VA_ARGS__);                                                                       \
                break;                                                                                                 \
            default:                                                                                                   \
                log_info(message, __VA_ARGS__);                                                                        \
                break;                                                                                                 \
        }                                                                                                              \
    } while (0)

#endif
