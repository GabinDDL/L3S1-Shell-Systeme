#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/utils/string_utils.h"
#include "test_string_utils.h"

void test_start_with();
void test_concat_with_delimiter();

void test_string_utils() {
    printf("Test function start_with\n");
    test_start_with();
    printf("Test start_with passed\n");
    printf("Test function concat_with_delimiter\n");
    test_concat_with_delimiter();
    printf("Test concat_with_delimiter passed\n");
}

void test_start_with() {
    assert(start_with("/doc/test/doc", ""));
    assert(start_with("/doc/test/doc", "/"));
    assert(start_with("/doc/test/doc", "/doc/test/doc"));
    assert(!start_with("/doc/test/doc", "o"));
    assert(!start_with("/doc/test/doc", "/doc1"));
    assert(!start_with("/doc/test/doc", "/doc/test/doc1"));
}

void test_concat_with_delimiter() {
    char* test_str = concat_with_delimiter("", "", '/');
    assert(strcmp(test_str, "/") == 0);
    free(test_str);

    test_str = concat_with_delimiter("test1", "test2", '/');
    assert(strcmp(test_str, "test1/test2") == 0);
    free(test_str);

    test_str = concat_with_delimiter("", "test1", '/');
    assert(strcmp(test_str, "/test1") == 0);
    free(test_str);

    test_str = concat_with_delimiter("test/plus/gros", "ajoute/a", '/');
    assert(strcmp(test_str, "test/plus/gros/ajoute/a") == 0);
    free(test_str);

    test_str = concat_with_delimiter("test1", "test2", '~');
    assert(strcmp(test_str, "test1~test2") == 0);
    free(test_str);
}
