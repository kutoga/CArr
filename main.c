#include <stdio.h>
#include "carr.h"

int main()
{
    t_arr_d(int) d_arr = {0};
    t_arr_c(int, 12) c_arr = {0};

    for (int i = 0; i < 20; ++i) {
        arr_add(&d_arr, 1337);
        arr_add(&c_arr, 1337);

        printf("d) count=%lu capacity=%lu\n", arr_count(&d_arr), arr_capacity(&d_arr));
        printf("c) count=%lu capacity=%lu\n", arr_count(&c_arr), arr_capacity(&c_arr));
    }
}
