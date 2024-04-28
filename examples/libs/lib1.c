#include <ncvm.h>

#include <stdio.h>

_export NCVM_LIB_FUNCTION(println) {
    printf("%s\n", thread->vm->static_mem_p + thread->u64_registers[1]);
}