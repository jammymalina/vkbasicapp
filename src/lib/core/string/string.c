#include "./string.h"

#include <stdlib.h>

size_t string_length(const char* str) {
    size_t i;
    for (i = 0; str[i] != '\0'; ++i);
    return i;
}

bool string_is_empty(const char* str) {
    return str[0] == '\0';
}

bool string_copy(char* dest, size_t max_dest_length, const char* src) {
    if (max_dest_length == 0) {
        return false;
    }

    size_t i;
    for (i = 0; i < max_dest_length; ++i) {
        dest[i] = src[i];
        if (src[i] == '\0') {
            break;
        }
    }
    return src[i] == '\0';
}

static inline ssize_t string_transform_idx(const char* str, ssize_t idx) {
    ssize_t result_idx = idx;
    const size_t str_len = string_length(str);
    if (idx < 0) {
        result_idx = str_len + (idx + 1);
    }
    if (result_idx < 0) {
        return -1;
    }

    return result_idx <= str_len ? result_idx : -1;
}

bool string_substring_idx(char* dest, const size_t max_dest_length,
    const char* str, const ssize_t start_index, const ssize_t end_index)
{
    string_copy(dest, max_dest_length, "");
    ssize_t start_idx = string_transform_idx(str, start_index);
    ssize_t end_idx = string_transform_idx(str, end_index);

    if (start_idx == -1 || end_idx == -1 || start_idx > end_idx) {
        return false;
    }
    size_t ext_len = end_idx - start_idx;
    if (ext_len >= max_dest_length) {
        return false;
    }

    for (size_t dest_idx = 0; dest_idx < ext_len; ++dest_idx) {
        dest[dest_idx] = str[start_idx + dest_idx];
    }
    dest[ext_len] = '\0';

    return true;
}

ssize_t string_index_of(const char* str, char c) {
    return string_index_of_nth(str, c, 1);
}

ssize_t string_index_of_nth(const char* str, char c, size_t n) {
    size_t occurence_count = 0;
    for (ssize_t i = 0; str[i] != '\0'; ++i) {
        if (str[i] == c) {
            ++occurence_count;
            if (occurence_count == n) {
                return i;
            }
        }
    }
    return -1;
}

ssize_t string_last_index_of(const char* str, char c) {
    return string_last_index_of_nth(str, c, 1);
}

ssize_t string_last_index_of_nth(const char* str, char c, size_t n) {
    size_t occurence_count = 0;
    for (ssize_t i = string_length(str) - 1; i >= 0; --i) {
        if (str[i] == c) {
            ++occurence_count;
            if (occurence_count == n) {
                return i;
            }
        }
    }
    return -1;
}

bool string_equal(const char* str1, const char* str2) {
    size_t i;
    for (i = 0; str1[i] == str2[i] && str1[i] != '\0' && str2[i] != '\0'; ++i);
    return str1[i] == str2[i];
}

bool string_starts_with(const char* str, const char* substr) {
    size_t i;
    for (i = 0; str[i] == substr[i] && str[i] != '\0' && substr[i] != '\0';
        ++i);
    return str[i] == substr[i] || substr[i] == '\0';
}

bool string_ends_with(const char* str, const char* substr) {
    if (string_is_empty(substr)) {
        return true;
    }
    if (string_is_empty(str)) {
        return false;
    }
    size_t i = string_length(str) - 1, j = string_length(substr) - 1;
    while (true) {
        if (str[i] != substr[j]) {
            return false;
        }
        if (i == 0 || j == 0) {
            break;
        }
        --i, --j;
    }
    return j == 0;
}

bool string_append(char* dest, size_t max_dest_length, const char* src) {
    if (string_is_empty(src)) {
        return true;
    }
    if (max_dest_length == 0) {
        return false;
    }

    size_t i, j;
    for (i = 0; dest[i] != '\0' && i < max_dest_length - 1; ++i);

    for (j = 0; src[j] != '\0' && i < max_dest_length - 1; ++j, ++i) {
        dest[i] = src[j];
    }
    dest[i] = '\0';

    return src[j] == '\0';
}

void string_reverse(char* str, size_t start_index, size_t end_index) {
    while (start_index < end_index) {
        char tmp = str[start_index];
        str[start_index] = str[end_index];
        str[end_index] = tmp;
        start_index++;
        end_index--;
    }
}

bool string_add_number_postfix(char* dest, size_t max_dest_length,
    const char* str, int num, int base)
{
    if (max_dest_length == 0) {
        return false;
    }

    size_t i;
    for (i = 0; str[i] != '\0' && i < max_dest_length - 1; ++i) {
        dest[i] = str[i];
    }
    if (i >= max_dest_length - 1) {
        dest[max_dest_length - 1] = '\0';
        return false;
    }

    if (num == 0) {
        dest[i] = '0';
        dest[i + 1] = '\0';
        return true;
    } else if (num < 0) {
        dest[i] = '-';
        num = -num;
        ++i;
    }

    size_t start_index = i;
    while (num != 0 && i < max_dest_length - 1) {
        div_t d = div(num, base);
        dest[i] = (d.rem > 9)? (d.rem - 10) + 'a' : d.rem + '0';
        num = d.quot;
        ++i;
    }

    dest[i] = '\0';
    string_reverse(dest, start_index, i - 1);

    return num == 0;
}
