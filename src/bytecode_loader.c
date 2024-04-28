#include <ncvm.h>
#include "util.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    const unsigned char* data_p;
    unsigned long data_size;

} dataStream; 

const unsigned char* get_next_n_bytes(
    unsigned long long n, void* data_p
) {
    dataStream* stream = (dataStream*)data_p;
    if (stream->data_size < n) {
        return (void*)0;
    }

    const unsigned char* data = stream->data_p;
    stream->data_p += n;
    stream->data_size -= n;
    return data;
}

_export ncvm ncvm_loadBytecodeData(
    const unsigned char* data_p,
    const unsigned long  data_size,
    ncvm_lib_function    (*get_lib_function)(const char* name, void* lib_data_p),
    void*                lib_data_p
) {
    dataStream stream = {
        .data_p = data_p,
        .data_size = data_size
    };
    ncvm result =  ncvm_loadBytecodeStream(
        get_next_n_bytes,
        &stream,
        get_lib_function,
        lib_data_p,
        0
    );
    return result;
}



#define load_field(name, type, size) \
    tmp = get_next_n_bytes(size, data_p); \
    if (tmp == (void*)0) { \
        *ret_code = 1; \
        return result; \
    } \
    type name = *(type*)tmp;

#ifdef __NCVM_DEBUG
#include <stdio.h>
#endif
#include <dlfcn.h>

/* Load  */
_export ncvm ncvm_loadBytecodeStream(
    const unsigned char* (*get_next_n_bytes)(const unsigned long long n, void* const data_p),
    void*             data_p,
    ncvm_lib_function (*get_lib_function)(const char* name, void* lib_data_p),
    void*             lib_data_p,
    int*              ret_code
) {
    ncvm result;

    if (!is_little_endian()) {
        *ret_code = 1;
        return result;
    }

    const unsigned char* tmp;
    
    load_field(version, unsigned int, 4);
    if (version < NCVM_MIN_VERSION || version > NCVM_VERSION) {
        *ret_code = 1;
        return result;
    }

    load_field(u32_count, unsigned char, 1)
    load_field(u64_count, unsigned char, 1)
    load_field(f32_count, unsigned char, 1)
    load_field(f64_count, unsigned char, 1)
    load_field(stack_size,          unsigned long long, 8)
    load_field(call_stack_size,     unsigned long long, 8)
    load_field(lib_functions_count, unsigned long long, 8)
    load_field(lib_functions_size,  unsigned long long, 8)
    load_field(static_mem_size,     unsigned long long, 8)
    load_field(block_size,          unsigned long long, 8)

    const char* fn_names = (const char*)get_next_n_bytes(lib_functions_size, data_p); \
    if (tmp == (void*)0) { \
        *ret_code = 1; \
        return result; \
    } \

    const unsigned char * static_memory = get_next_n_bytes(static_mem_size, data_p);
    if (static_memory == (void*)0) {
        *ret_code = 1;
        return result;
    }

    Instruction * instructions = (Instruction*)get_next_n_bytes(block_size * 4, data_p);
    if (instructions == (void*)0) {
        *ret_code = 1;
        return result;
    }

    if (
           (unsigned long)stack_size          != stack_size
        || (unsigned long)call_stack_size     != call_stack_size
        || (unsigned long)lib_functions_count != lib_functions_count
        || (unsigned long)lib_functions_size  != lib_functions_size
        || (unsigned long)static_mem_size     != static_mem_size
        || (unsigned long)block_size          != block_size
    ) {
        *ret_code = 1;
        return result;
    }

#ifdef __NCVM_DEBUG
    printf("Version: %u\n", version);
    printf("u32_count: %u\n", u32_count);
    printf("u64_count: %u\n", u64_count);
    printf("f32_count: %u\n", f32_count);
    printf("f64_count: %u\n", f64_count);
    printf("stack_size: %llu\n", stack_size);
    printf("call_stack_size: %llu\n", call_stack_size);
    printf("lib_functions_count: %llu\n", lib_functions_count);
    printf("lib_functions_size: %llu\n", lib_functions_size);
    printf("static_mem_size: %llu\n", static_mem_size);
    printf("block_size: %llu\n", block_size);
#endif

    result.main_thread_settings = (ThreadSettings) {
        u32_count,
        u64_count,
        f32_count,
        f64_count,
        stack_size,
        call_stack_size
    };


    result.inst_count = block_size;
    result.inst_p = malloc(block_size * 4);
    memcpy(result.inst_p, instructions, (unsigned long)block_size * 4);

    result.static_mem_size = static_mem_size;
    result.static_mem_p = malloc(static_mem_size);
    memcpy(result.static_mem_p, (void*)static_memory, (unsigned long)static_mem_size);

    
    /*// void* handle = dlopen("build/macosx/arm64/release/liblib1.dylib", RTLD_LAZY);
    // if (!handle) {
    //     //fprintf(stderr, "dlopen failed: %s\n", dlerror());
    //     exit(25);
    // }*/



    result.lib_functions = malloc(lib_functions_count * sizeof(void*));
    unsigned long i_func = 0;
    unsigned long i_lib = 0;
    for (i_func = 0; i_func < lib_functions_count; ++i_func) {
        ncvm_lib_function f = get_lib_function(fn_names, lib_data_p);
        if (f == NULL) {
            *ret_code = 1;
            return result;
        }

        result.lib_functions[i_func] = f;
    }

    *ret_code = 0;
    return result;
}