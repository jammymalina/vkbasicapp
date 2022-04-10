#ifndef INI_PARSER_H
#define INI_PARSER_H

#include "../logger/logger.h"

#define INI_PARSER_MAX_LINE_SIZE 512
#define INI_PARSER_MAX_SECTION_SIZE 64
#define INI_PARSER_MAX_NAME_SIZE 256

#define INI_PARSER_START_COMMENT_PREFIXES ";#"
#define INI_PARSER_INLINE_COMMENT_PREFIXES ";"

#define INI_PARSER_ASSERT_INT(section, name, value, sign, result)                                                      \
    do {                                                                                                               \
        bool _is_valid = string_validate_int(value, sign);                                                             \
        if (!_is_valid) {                                                                                              \
            log_warning("Invalid numerical value, [%s] %s = %s", section, name, value);                                \
            return result;                                                                                             \
        }                                                                                                              \
    } while (0)

typedef char* (*ini_parser_reader)(char* str, int num, void* stream);

typedef int (*ini_parser_handler)(void* user, const char* section, const char* name, const char* value, int lineno);

int ini_parse(const char* filename, ini_parser_handler handler, void* user);

#endif
