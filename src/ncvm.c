#include <ncvm.h>
#include <stdlib.h>
#include <extc_vec.h>

vector_template_def(int, int);
vector_template_impl(int, int);

_export int factorial(int a) {
    vec_int vec = vec_int_init();
    vec_int_push_back(&vec, 456);
    int num = vec.data[0];
    vec_int_free(&vec);
    return num;
}

_export int* get_size() {
    int* ret = (int*)malloc(4*3);
    *ret = sizeof(enum OPCODE);
    *(ret+1) = sizeof(enum REGISTER);
    ret[2] = sizeof(size_t);
    return ret;
}


