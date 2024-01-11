#include <ncvm.h>
#include <stdlib.h>
#include <extc_vec.h>

vector_template_def(int, int);
vector_template_impl(int, int);


_export ncvm ncvm_initArr(Instruction* inst_p, long inst_count) {
    ncvm ret;
    ret.inst_p = (Instruction*)malloc(sizeof(Instruction) * inst_count);
    memcpy(ret.inst_p, inst_p, sizeof(Instruction) * inst_count);
    ret.inst_count = inst_count;
    return ret;
}

_export ncvm ncvm_initData(const char* data_p, long data_size) {
    ncvm ret;
    /* TODO: Add body */
    return ret;
}

_export ncvm ncvm_initFile(const char* filename) {
    ncvm ret;
    /* TODO: Add body */
    return ret;
}

_export void ncvm_free(ncvm* vm) {
    free(vm->inst_p);
}




_export int ncvm_execute(ncvm* vm) {
    
    
    return 0;
}










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
    ret[2] = sizeof(u64);
    return ret;
}


