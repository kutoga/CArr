#include <stdio.h>
#include <assert.h>
#include "carr.h"

int main() {

    // create two empty arrays
    t_arr_c(const char *, 3) names   = {0};
    t_arr_d(int)             numbers = {0};

    // bool arr_add(t_arr *arr, T element)
    //  Returns true if successful, otherwise false.
    bool c_ok = arr_add(&names,  "Kevin");
    bool d_ok = arr_add(&numbers, 13);
    assert(c_ok);
    assert(d_ok);

    // bool arr_add(t_arr *arr, T element)
    //  Returns true if successful, otherwise false.
    const char *name = "Karl";
    c_ok = arr_add_ptr(&names,  &name);
    const int number = 71;
    d_ok = arr_add_ptr(&numbers, &number);
    assert(c_ok);
    assert(d_ok);

    // const T *arr_at(const t_arr *arr, size_t index)
    //       T *arr_at(      t_arr *arr, size_t index)
    // char **name0 = arr_at(&names, 0);
    // printf("Name: %s\n", *name0);

    // TODO: Document

    // size_t arr_count(const t_arr *arr)
    //  Returns the used number of elements in the array.
    printf("Count: %lu\n", arr_count(&names));
    printf("Count: %lu\n", arr_count(&numbers));

    // size_t arr_capacity(const t_arr *arr)
    //  Returns the capacity of the array.
    printf("Capacity: %lu\n", arr_capacity(&names));
    printf("Capacity: %lu\n", arr_capacity(&numbers));
    
    // size_t arr_c_capacity(const t_arr_c(T, N) *arr)
    //  Like arr_capacity, but a compile-time constant which returns N
    printf("Capacity: %lu\n", arr_c_capacity(&names));

    // void arr_destroy(t_arr *arr)
    //  Cleans up dynamically allocated data. Has no effect for arr_c(T, N)
    arr_destroy(&names);
    arr_destroy(&numbers);
}