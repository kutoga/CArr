# CArr

I program quite a lot of C and from time to time I need some simple and stupid arrays like these:

```C
struct t_person {

    // reallocated array
    const char **hobbies;
    int hobby_count;
    int hobby_capacity;

    // fix sized array (used when an upper-bound is known)
    int favourite_numbers[32];
    int favourite_number_count;
};

void person_add_hobby(struct t_person *person, const char *hobby) {
    if (person->hobby_count == person->hobby_capacity) {
        if (person->hobby_capacity == 0) {
            person->hobby_capacity = 8;
        }
        else {
            person->hobby_capacity *= 2;
        }
        person->hobbies = realloc(person->hobbies, sizeof(const char *) * person->hobby_capacity);
    }
    person->hobbies[person->hobby_count++] = hobby;
}

bool person_add_favourite_number(struct t_person *person, int number) {
    if (person->favourite_number_count >= sizeof(person->favourite_numbers) / sizeof(int)) {
        return false;
    }
    person->favourite_numbers[person->favourite_number_count++] = number;
    return true;
}
```

Managing the bound-checks is always similar or even the same. It's annoying and repeating code.
This small header-only library adds two generic types which make life easier. The library has
a focus on efficient code and type-safety.

```C
#include "carr.h"

struct t_person {

    // reallocated array
    t_arr_d(const char *) hobbies; // arr_d => dynamic array

    // fix sized array (used when an upper-bound is known)
    t_arr_c(int, 32) favourite_numbers;
};

void person_add_hobby(struct t_person *person, const char *hobby) {
    arr_add(&person->hobbies, hobby);
}

bool person_add_favourite_number(struct t_person *person, int number) {
    return arr_add(&person->favourite_numbers, number);
}
```

One type is `t_arr_d(T)` which describes a dynamically reallocated array
of the type `T`. On the other side `t_arr_c(T, N)` describes an array
with a constant size `N` and the type `T`.

Both data types are simple structs with these fields:
- `elements`: (an array with the data)
- `count`: Used elements
- `capacity`: Capacity (only for `t_arr_d(T)`)

These fields could directlty be used, but it's recommended to use the
predefined functions / macros. They are generic and work both for `t_arr_d(T)`
and `t_arr_c(T, N)`:

```C
#include <stdio.h>
#include <assert.h>
#include "carr.h"

int main() {

    // create two empty arrays:
    // - names contains the type "const char *" and has a constant size of 3 elements
    // - numbers contains the type int and is dynamically allocated
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
```
