#include <ncvm.h>

#include <stdio.h>

_export NCVM_LIB_FUNCTION(println) {
    printf("%s\n", thread->vm->static_mem_p + thread->u64_registers[1]);
}

_export NCVM_LIB_FUNCTION(print_long) {
    printf("%lld\n", thread->u64_registers[2]);
}

_export NCVM_LIB_FUNCTION(print_pi) {
    printf("%.16f\n", thread->f64_registers[1]);
}

_export NCVM_LIB_FUNCTION(print_r3) {
    printf("%d\n", thread->u32_registers[3]);
}