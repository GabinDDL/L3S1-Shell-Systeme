#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/utils/string_utils.h"
#include "test_string_utils.h"

void test_start_with();
void test_concat_with_delimiter();
void test_is_integer();
void test_start_with_exception();void test_has_sequence_of();

void test_string_utils() {
    printf("Test function start_with\n");
    test_start_with();
    printf("Test start_with passed\n");

    printf("Test function start_with_exception\n");
    test_start_with_exception();
    printf("Test start_with_exception passed\n");
    
    printf("Test function concat_with_delimiter\n");
    test_concat_with_delimiter();
    printf("Test concat_with_delimiter passed\n");
    
    printf("Test function is_integer\n");
    test_is_integer();
    printf("Test is_integer passed\n");

    printf("Test function has_sequence_of\n");
    test_is_integer();
    printf("Test has_sequence_of passed\n");
}

void test_start_with() {
    assert(start_with("/doc/test/doc", ""));
    assert(start_with("/doc/test/doc", "/"));
    assert(start_with("/doc/test/doc", "/doc/test/doc"));
    assert(!start_with("/doc/test/doc", "o"));
    assert(!start_with("/doc/test/doc", "/doc1"));
    assert(!start_with("/doc/test/doc", "/doc/test/doc1"));
}

void test_start_with_exception() {
    assert(start_with_exception("           /doc/test/doc", "", ' '));
    assert(start_with_exception("                /doc/test/doc", "/", ' '));
    assert(start_with_exception(" /doc/test/doc", "/doc/test/doc", ' '));
    assert(!start_with_exception("       /doc/test/doc", "o", ' '));
    assert(!start_with_exception("           /doc/test/doc", "/doc1", ' '));
    assert(!start_with_exception(" /doc/test/doc", "/doc/test/doc1", ' '));
}

void test_concat_with_delimiter() {
    char *test_str = concat_with_delimiter("", "", '/');
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

void test_is_integer() {
    char *test_str = "123abc";
    assert(!is_integer(test_str));

    test_str = "123";
    assert(is_integer(test_str));

    test_str = "0";
    assert(is_integer(test_str));

    test_str = "/documents";
    assert(!is_integer(test_str));

    test_str = "a1bc";
    assert(!is_integer(test_str));

    test_str = "+123";
    assert(!is_integer(test_str));

    test_str = "1";
    assert(is_integer(test_str));

    test_str = "300";
    assert(is_integer(test_str));
}

void test_has_sequence_of() {
    assert(!has_sequence_of("", 'c'));
    assert(!has_sequence_of("c", 'c'));
    assert(!has_sequence_of("cdcd c c cddd cddc", 'c'));
    assert(has_sequence_of("cc", 'c'));
    assert(has_sequence_of("dss dddvfs dcc", 'c'));
    assert(has_sequence_of("dss ddccfs dcdsd", 'c'));
    assert(has_sequence_of("dss ddccfs dcdsccd", 'c'));
}
