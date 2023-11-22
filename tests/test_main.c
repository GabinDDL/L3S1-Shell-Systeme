#include <stdio.h>
#include <assert.h>
#include "parser/test_parser.h"

int main() {
    printf("Running tests...\n");
    assert(test_parse_command() == 0);
    printf("All tests passed!\n");
    return 0;
}
