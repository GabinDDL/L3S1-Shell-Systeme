#include "int_utils.h"

int get_nb_of_digits(int nb) {
    int res = 0;
    int tmp = nb;

    while (tmp != 0) {
        tmp = tmp / 10;
        ++res;
    }

    return res;
}