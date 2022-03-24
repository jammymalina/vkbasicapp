#ifndef LOGGER_H
#define LOGGER_H

#include <SDL2/SDL.h>
#include <stdlib.h>

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

#endif
