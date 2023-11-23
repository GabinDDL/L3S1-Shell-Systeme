#include <stdio.h>
#include <assert.h>
#include "parser/test_parser.h"

int main() {
    printf("Running tests...\n");

    printf("Running test parser_utils\n");
    test_parser_utils();
    printf("Test parser_utils passed\n");

    printf("All test cases passed!\n");

    return 0;
}
