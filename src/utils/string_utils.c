#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_utils.h"

bool start_with(const char *str, const char *prefix) {
    if (str == NULL || prefix == NULL) {
        return false;
    }
    size_t len_str = strlen(str);
    size_t len_prefix = strlen(prefix);

    if (len_prefix > len_str) {
        return false;
    }
    for (size_t i = 0; i < len_prefix; i++) {
        if (str[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}

bool start_with_exception(const char *str, const char *prefix, char c) {
    if (str == NULL || prefix == NULL) {
        return false;
    }
    size_t len_str = strlen(str);
    size_t len_prefix = strlen(prefix);

    if (len_prefix > len_str) {
        return false;
    }
    size_t i = 0;

    while (i < len_str) {
        if (str[i] != c) {
            break;
        }
        i++;
    }

    if (len_prefix > len_str - i) {
        return false;
    }

    for (size_t j = 0; j < len_prefix; j++) {
        if (str[i] != prefix[j]) {
            return false;
        }
        i++;
    }
    return true;
}

char *concat_with_delimiter(const char *str1, const char *str2, char delimiter) {
    if (str1 == NULL || str2 == NULL) {
        return NULL;
    }
    size_t len_str1 = strlen(str1);
    size_t len_str2 = strlen(str2);

    char *result = malloc((len_str1 + len_str2 + 2) * sizeof(char));

    assert(result != NULL);

    memmove(result, str1, len_str1);
    result[len_str1] = delimiter;
    memmove(result + len_str1 + 1, str2, len_str2);
    result[len_str1 + len_str2 + 1] = '\0';

    return result;
}

bool is_integer(const char *str) {
    for (size_t i = 0; i < strlen(str); ++i) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

bool has_sequence_of(const char *str, char c) {
    size_t str_len = strlen(str);

    if (str_len == 0) {
        return false;
    }

    for (size_t i = 0; i < str_len - 1; i++) {
        if (str[i] == c && str[i + 1] == c) {
            return true;
        }
    }
    return false;
}
