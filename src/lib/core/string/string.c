#include "./string.h"

#include <stdlib.h>

size_t string_length(const char* str) {
    size_t i;
    for (i = 0; str[i] != '\0'; ++i)
        ;
    return i;
}

bool string_is_empty(const char* str) { return str[0] == '\0'; }

bool string_copy(const char* src, char* dst, size_t max_dst_length) {
    if (max_dst_length == 0) {
        return false;
    }

    size_t i;
    for (i = 0; i < max_dst_length; ++i) {
        dst[i] = src[i];
        if (src[i] == '\0') {
            break;
        }
    }
    // ensures that the dst has always terminating character
    dst[max_dst_length - 1] = '\0';

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

bool string_substring_idx(const char* str, ssize_t start_index, ssize_t end_index, char* dst, size_t max_dst_length) {
    string_copy("", dst, max_dst_length);
    ssize_t start_idx = string_transform_idx(str, start_index);
    ssize_t end_idx = string_transform_idx(str, end_index);

    if (start_idx == -1 || end_idx == -1 || start_idx > end_idx) {
        return false;
    }
    size_t ext_len = end_idx - start_idx;
    if (ext_len >= max_dst_length) {
        return false;
    }

    for (size_t dst_idx = 0; dst_idx < ext_len; ++dst_idx) {
        dst[dst_idx] = str[start_idx + dst_idx];
    }
    dst[ext_len] = '\0';

    return true;
}

ssize_t string_index_of(const char* str, char c) { return string_index_of_nth(str, c, 1); }

ssize_t string_index_of_nth(const char* str, char c, size_t n) {
    size_t occurrence_count = 0;
    for (ssize_t i = 0; str[i] != '\0'; ++i) {
        if (str[i] == c) {
            ++occurrence_count;
            if (occurrence_count == n) {
                return i;
            }
        }
    }
    return -1;
}

ssize_t string_last_index_of(const char* str, char c) { return string_last_index_of_nth(str, c, 1); }

ssize_t string_last_index_of_nth(const char* str, char c, size_t n) {
    size_t occurrence_count = 0;
    for (ssize_t i = string_length(str) - 1; i >= 0; --i) {
        if (str[i] == c) {
            ++occurrence_count;
            if (occurrence_count == n) {
                return i;
            }
        }
    }
    return -1;
}

bool string_equal(const char* str1, const char* str2) {
    size_t i;
    for (i = 0; str1[i] == str2[i] && str1[i] != '\0' && str2[i] != '\0'; ++i)
        ;
    return str1[i] == str2[i];
}

bool string_starts_with(const char* str, const char* substr) {
    size_t i;
    for (i = 0; str[i] == substr[i] && str[i] != '\0' && substr[i] != '\0'; ++i)
        ;
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

bool string_append(char* dst, const char* src, size_t max_dst_length) {
    if (string_is_empty(src)) {
        return true;
    }
    if (max_dst_length == 0) {
        return false;
    }

    size_t i, j;
    for (i = 0; dst[i] != '\0' && i < max_dst_length - 1; ++i)
        ;

    for (j = 0; src[j] != '\0' && i < max_dst_length - 1; ++j, ++i) {
        dst[i] = src[j];
    }
    dst[i] = '\0';

    return src[j] == '\0';
}

void string_reverse(char* str, size_t start_index, size_t end_index) {
    while (start_index < end_index) {
        char tmp = str[start_index];
        str[start_index] = str[end_index];
        str[end_index] = tmp;
        ++start_index;
        --end_index;
    }
}

bool string_add_number_postfix(char* dst, size_t max_dst_length, const char* str, int num, int base) {
    if (max_dst_length == 0) {
        return false;
    }

    size_t i;
    for (i = 0; str[i] != '\0' && i < max_dst_length - 1; ++i) {
        dst[i] = str[i];
    }
    if (i >= max_dst_length - 1) {
        dst[max_dst_length - 1] = '\0';
        return false;
    }

    if (num == 0) {
        dst[i] = '0';
        dst[i + 1] = '\0';
        return true;
    } else if (num < 0) {
        dst[i] = '-';
        num = -num;
        ++i;
    }

    size_t start_index = i;
    while (num != 0 && i < max_dst_length - 1) {
        div_t d = div(num, base);
        dst[i] = (d.rem > 9) ? (d.rem - 10) + 'a' : d.rem + '0';
        num = d.quot;
        ++i;
    }

    dst[i] = '\0';
    string_reverse(dst, start_index, i - 1);

    return num == 0;
}
