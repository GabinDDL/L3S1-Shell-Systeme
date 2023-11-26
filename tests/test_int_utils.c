#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../src/utils/int_utils.h"
#include "test_int_utils.h"

void test_nb_of_digits();

void test_int_utils() {
    printf("Running test get_nb_of_digits\n");
    test_nb_of_digits();
    printf("Test get_nb_of_digits passed\n");
}

void test_nb_of_digits() {
    int val = 1;
    assert(get_nb_of_digits(val) == 1);
    for (size_t i = 1; i < 10; ++i) {
        int val_times_power_of_ten = val*pow(10, i);
        assert(get_nb_of_digits(val_times_power_of_ten) == i+1);
    }
}