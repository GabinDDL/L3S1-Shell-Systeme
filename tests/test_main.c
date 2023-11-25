#include <stdio.h>
#include <assert.h>
#include "parser/test_parser.h"

#include "test_string_utils.h"

int main() {
    printf("Running tests...\n");

    printf("Running test parser_utils\n");
    test_parser_utils();
    printf("Test parser_utils passed\n");
    printf("Running test string_utils\n");
    test_string_utils();
    printf("Test string_utils passed\n");

    printf("All test cases passed!\n");

    return 0;
}
