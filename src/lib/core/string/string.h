#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

bool string_copy(const char* src, char* dst, size_t max_dst_length);
bool string_substring_idx(const char* src, ssize_t start_index, ssize_t end_index, char* dst, size_t max_dst_length);
char* string_substring_char(const char* str, char c);

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

bool string_append(char* dst, const char* src, size_t max_dst_length);
bool string_add_number_postfix(char* dst, size_t max_dst_length, const char* str, int num, int base);

int string_to_int(const char* str);

#endif
