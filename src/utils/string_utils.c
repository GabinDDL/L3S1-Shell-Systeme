#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
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

char *concat_with_delimiter(const char *str1, const char *str2, char delimiter) {
    if (str1 == NULL || str2 == NULL) {
        return NULL;
    }
    size_t len_str1 = strlen(str1);
    size_t len_str2 = strlen(str2);

    char *result = malloc((len_str1 + len_str2 + 2) * sizeof(char));

    memmove(result, str1, len_str1);
    result[len_str1] = delimiter;
    memmove(result + len_str1 + 1, str2, len_str2);
    result[len_str1 + len_str2 + 1] = '\0';

    return result;
}
