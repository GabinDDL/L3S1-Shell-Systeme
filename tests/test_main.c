#include "parser/test_parser.h"
#include <assert.h>
#include <stdio.h>

#include "utils/test_core.h"
#include "utils/test_int_utils.h"
#include "utils/test_string_utils.h"
int main() {
    printf("Running tests...\n");

    printf("Running test parser_utils\n");
    test_parser_utils();
    printf("Test parser_utils passed\n");

    printf("Running test string_utils\n");
    test_string_utils();
    printf("Test string_utils passed\n");

    printf("Running test int_utils\n");
    test_int_utils();
    printf("Test int_utils passed\n");

    printf("Running test core\n");
    test_core();
    printf("Test core passed\n");

    printf("All test cases passed!\n");

    return 0;
}
