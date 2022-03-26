#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <sys/types.h>
#include <stdbool.h>
#include <stddef.h>

bool string_copy(char* dest, size_t max_dest_length, const char* src);
bool string_substring_idx(char* dest, const size_t max_dest_length,
    const char* str, const ssize_t start_index, const ssize_t end_index);

size_t string_length(const char* str);
bool string_is_empty(const char* str);

ssize_t string_index_of(const char* str, char c);
ssize_t string_index_of_nth(const char* str, char c, size_t n);

ssize_t string_last_index_of(const char* str, char c);
ssize_t string_last_index_of_nth(const char* str, char c, size_t n);

bool string_starts_with(const char* str, const char* substr);
bool string_ends_with(const char* str, const char* substr);

bool string_equal(const char* str1, const char* str2);

void string_reverse(char* str, size_t start_index, size_t end_index);

bool string_append(char* dest, size_t max_dest_length, const char* src);
bool string_add_number_postfix(char* dest, size_t max_dest_length,
    const char* str, int num, int base);

#endif
