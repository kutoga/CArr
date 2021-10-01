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

    // bool arr_add_ptr(t_arr *arr, const T *element)
    //  Returns true if successful, otherwise false.
    const char *name = "Karl";
    c_ok = arr_add_ptr(&names,  &name);
    const int number = 71;
    d_ok = arr_add_ptr(&numbers, &number);
    assert(c_ok);
    assert(d_ok);

    // const T *arr_at_ptr(const t_arr *arr, size_t index)
    //       T *arr_at_ptr(      t_arr *arr, size_t index)
    // const T *arr_at_ptr_unsafe(const t_arr *arr, size_t index)
    //       T *arr_at_ptr_unsafe(      t_arr *arr, size_t index)
    //  "_unsafe"-function do no bounds-check
    const char **name0 = arr_ptr_at(&names, 0);
    printf("Name: %s\n", *name0);
    int *number0 = arr_ptr_at(&numbers, 0);
    printf("Number: %d\n", *number0);

    // T arr_at(const t_arr *arr, size_t index)
    // T arr_at_unsafe(const t_arr *arr, size_t index)
    //  "_unsafe"-function does no bounds-check
    printf("Name: %s\n", arr_at(&names, 1));
    printf("Number: %d\n", arr_at(&numbers, 1));

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

    // arr_foreach_ptr (t_arr *arr, iter_name)
    arr_foreach_ptr (&names, iter) {
        printf("names[%lu] = %s\n", iter.index, *iter.value);
    }

    // arr_foreach_ptr (t_arr *arr, iter_name)
    arr_foreach (&names, iter) {
        printf("names[%lu] = %s\n", iter.index, iter.value);
    }

    // void arr_destroy(t_arr *arr)
    //  Cleans up dynamically allocated data. Has no effect for arr_c(T, N)
    arr_destroy(&names);
    arr_destroy(&numbers);

    // be sure that the structures have the minimal possible size
    assert(sizeof(names)   == sizeof(size_t) + sizeof(const char *) * 3);
    //                        count            elements               capacity
    assert(sizeof(numbers) == sizeof(size_t) + sizeof(int *)        + sizeof(size_t));
    //                        count            elements               capacity
}

// the arr-types can be used in typedef, because they are normal types
typedef t_arr_d(char) my_arr_type;
typedef t_arr_c(unsigned long, 3) my_other_arr_type;
