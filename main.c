#include <stdio.h>
#include "carr.h"

int main()
{
    t_arr_d(int) d_arr = {0};
    t_arr_c(int, 12) c_arr = {0};

    for (int i = 0; i < 200; ++i) {
        printf("%d)\n", i);

        const bool ok_a = arr_add(&d_arr, i);
        const bool ok_b = arr_add(&c_arr, i);

        printf("d) count=%lu capacity=%lu %d\n", arr_count(&d_arr), arr_capacity(&d_arr), (int)ok_a);
        printf("c) count=%lu capacity=%lu %d\n", arr_count(&c_arr), arr_capacity(&c_arr), (int)ok_b);
    }

    arr_foreach (&d_arr, iter) {
        printf("%lu => %d\n", iter.index, iter.value);
    }

    arr_foreach (&c_arr, iter) {
        printf("%lu => %d\n", iter.index, iter.value);
    }

    printf("end!\n");
}
