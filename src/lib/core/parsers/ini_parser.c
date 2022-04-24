#include "./ini_parser.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>

#include "../string/string.h"

typedef struct IniParserStringContext {
    const char* ptr;
    size_t num_left;
} IniParserStringContext;

/* Strip whitespace chars off end of given string, in place. Return s. */
static char* rstrip(char* s) {
    char* p = s + string_length(s);
    while (p > s && isspace((unsigned char)(*--p))) *p = '\0';
    return s;
}

/* Strip quote chars off start and end of given string, in place. Return s. */
static char* qstrip(char* s) {
    size_t slen = string_length(s);
    if (slen < 2) {
        return s;
    }
    char* p = s + slen;
    if (*s == '"' && *(p - 1) == '"') {
        s++;
        *(p - 1) = '\0';
    }
    return s;
}

/* Return pointer to first non-whitespace char in given string. */
static char* lskip(const char* s) {
    while (*s && isspace((unsigned char)(*s))) s++;
    return (char*)s;
}

/* Return pointer to first char (of chars) or inline comment in given string,
   or pointer to NULL at end of string if neither found. Inline comment must
   be prefixed by a whitespace character to register as a comment. */
static char* find_chars_or_comment(const char* s, const char* chars) {
    int was_space = 0;
    while (*s && (!chars || !string_substring_char(chars, *s)) &&
           !(was_space && string_substring_char(INI_PARSER_INLINE_COMMENT_PREFIXES, *s))) {
        was_space = isspace((unsigned char)(*s));
        s++;
    }

    return (char*)s;
}

static int ini_parse_stream(ini_parser_reader reader, void* stream, ini_parser_handler handler, void* user) {
    char line[INI_PARSER_MAX_LINE_SIZE];
    int max_line = INI_PARSER_MAX_LINE_SIZE;

    char section[INI_PARSER_MAX_SECTION_SIZE] = "";
    char prev_name[INI_PARSER_MAX_NAME_SIZE] = "";

    char* start;
    char* end;
    char* name;
    char* value;
    int lineno = 0;
    int error = 0;

    while (reader(line, max_line, stream) != NULL) {
        lineno++;

        start = line;

        // Skip BOM
        if (lineno == 1 && (unsigned char)start[0] == 0xEF && (unsigned char)start[1] == 0xBB &&
            (unsigned char)start[2] == 0xBF) {
            start += 3;
        }

        start = lskip(rstrip(start));

        if (string_substring_char(INI_PARSER_START_COMMENT_PREFIXES, *start)) {
            /* Start-of-line comment */
        } else if (*start == '[') {
            /* A "[section]" line */
            end = find_chars_or_comment(start + 1, "]");
            if (*end == ']') {
                *end = '\0';
                string_copy(start + 1, section, sizeof(section));
                *prev_name = '\0';
                if (!handler(user, section, NULL, NULL, lineno) && !error) {
                    error = lineno;
                }
            } else if (!error) {
                /* No ']' found on section line */
                error = lineno;
            }
        } else if (*start) {
            /* Not a comment, must be a name[=:]value pair */
            end = find_chars_or_comment(start, "=:");
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = rstrip(start);
                value = end + 1;
                end = find_chars_or_comment(value, NULL);
                if (*end) {
                    *end = '\0';
                }
                value = lskip(value);
                rstrip(value);
                value = qstrip(value);

                /* Valid name[=:]value pair found, call handler */
                string_copy(name, prev_name, sizeof(prev_name));
                if (!handler(user, section, name, value, lineno) && !error) {
                    error = lineno;
                }
            } else if (!error) {
                /* No '=' or ':' found on name[=:]value line */
                error = lineno;
            }
        }

        if (error) {
            log_warning("Ini parser error on line: %d", lineno);
        }
    }
    return error;
}

int ini_parse_file(FILE* file, ini_parser_handler handler, void* user) {
    return ini_parse_stream((ini_parser_reader)fgets, file, handler, user);
}

int ini_parse(const char* filename, ini_parser_handler handler, void* user) {
    FILE* file;
    int error;

    file = fopen(filename, "r");
    if (!file) return -1;
    error = ini_parse_file(file, handler, user);
    fclose(file);
    return error;
}
