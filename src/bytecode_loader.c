#include <stdint.h>
#include <ncvm.h>
#include "util.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    const uint8_t* data_p;
    unsigned long data_size;

} dataStream; 

const uint8_t* get_next_n_bytes(
    unsigned long n, void* data_p
) {
    dataStream* stream = (dataStream*)data_p;
    if (stream->data_size < n) {
        return (void*)0;
    }

    /*const uint8_t* data = stream->data_p;*/
    stream->data_p += n;
    stream->data_size -= n;
    return stream->data_p;
}

_export uint8_t ncvm_loadBytecodeData(
    ncvm*                vm,
    const uint8_t* data_p,
    const unsigned long  data_size,
    ncvm_lib_function    (*get_lib_function)(const char* name, void* lib_data_p),
    void*                lib_data_p
) {
    dataStream stream;
    uint8_t ret_code;
    stream.data_p = data_p;
    stream.data_size = data_size;
    
    ret_code =  ncvm_loadBytecodeStream(
        vm,
        get_next_n_bytes,
        &stream,
        get_lib_function,
        lib_data_p
    );
    return ret_code;
}



#define load_field(name, type, size) \
    tmp = get_next_n_bytes(size, data_p); \
    if (tmp == (void*)0) { \
        return NCVM_BYTECODE_READ_ERROR; \
    } \
    type name = *(type*)tmp;

#ifdef __NCVM_DEBUG
#include <stdio.h>
#endif

/* Load  */
_export uint8_t ncvm_loadBytecodeStream(
    ncvm*             vm,
    const uint8_t* (*get_next_n_bytes)(const unsigned long n, void* const data_p),
    void*             data_p,
    ncvm_lib_function (*get_lib_function)(const char* name, void* lib_data_p),
    void*             lib_data_p
) {
    const uint8_t* tmp;
    
    /* Init result */
    vm->inst_p = (Instruction*)0;
    /*result.inst_count = 0;
    result.static_mem_p = (void*)0;
    result.static_mem_size = 0;
    result.main_thread_settings.u32_reg_size = 0;
    result.main_thread_settings.u64_reg_size = 0;
    result.main_thread_settings.f32_reg_size = 0;
    result.main_thread_settings.f64_reg_size = 0;*/

    if (!is_little_endian()) {
        return NCVM_IS_BIG_ENDIAN;
    }
    
    load_field(version, uint32_t, 4);
    if (version < NCVM_MIN_VERSION || version > NCVM_VERSION) {
        return NCVM_INCOMPATIBLE_VERSION;
    }

    load_field(u32_count, uint8_t, 1)
    load_field(u64_count, uint8_t, 1)
    load_field(f32_count, uint8_t, 1)
    load_field(f64_count, uint8_t, 1)
    load_field(stack_size,          unsigned long, 8)
    load_field(call_stack_size,     unsigned long, 8)
    load_field(pub_functions_count, unsigned long, 8)
    load_field(lib_functions_count, unsigned long, 8)
    load_field(lib_functions_size,  unsigned long, 8)
    load_field(static_mem_size,     unsigned long, 8)
    load_field(block_size,          unsigned long, 8)

    ncvm_pub_function* pub_functions = malloc(pub_functions_count * sizeof(ncvm_pub_function));
    for (unsigned long i = 0; i < pub_functions_count; i++) {
        load_field(size, uint8_t, 1);
        const char* name_ptr = (const char*)get_next_n_bytes(size, data_p);
        if (name_ptr == (const char*)0) {
            return NCVM_BYTECODE_READ_ERROR;
        }
        char* name = (char*)malloc(size);
        if (name == (const char*)0) {
            return NCVM_STACK_ALLOC_ERROR;
        }
        memcpy(name, name_ptr, size);
        load_field(addr, unsigned long, 8);
        pub_functions[i].name = name;
        pub_functions[i].addr = addr;
    }

    const char* fn_names = (const char*)get_next_n_bytes(lib_functions_size, data_p); \
    if (tmp == (void*)0) { \
        return NCVM_BYTECODE_READ_ERROR; \
    } \

    const uint8_t* static_memory = get_next_n_bytes(static_mem_size, data_p);
    if (static_memory == (void*)0) {
        return NCVM_BYTECODE_READ_ERROR;
    }

    Instruction* instructions = (Instruction*)get_next_n_bytes(block_size * 4, data_p);
    if (instructions == (void*)0) {
        return NCVM_BYTECODE_READ_ERROR;
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

    vm->main_thread_settings.u32_reg_size = u32_count;
    vm->main_thread_settings.u64_reg_size = u64_count;
    vm->main_thread_settings.f32_reg_size = f32_count;
    vm->main_thread_settings.f64_reg_size = f64_count;
    vm->main_thread_settings.stack_size = stack_size;
    vm->main_thread_settings.call_stack_size = call_stack_size;

    vm->pub_functions = pub_functions;
    vm->pub_functions_count = pub_functions_count;

    vm->inst_count = block_size;
    vm->inst_p = malloc(block_size * 4);
    memcpy(vm->inst_p, instructions, (unsigned long)block_size * 4);

    vm->static_mem_size = static_mem_size;
    vm->static_mem_p = malloc(static_mem_size);
    memcpy(vm->static_mem_p, (void*)static_memory, (unsigned long)static_mem_size);


    vm->lib_functions = malloc(lib_functions_count * sizeof(void*));
    {
        unsigned long i_func = 0;
        for (i_func = 0; i_func < lib_functions_count; ++i_func) {
            ncvm_lib_function f = get_lib_function(fn_names, lib_data_p);
            if (f == (ncvm_lib_function)0) {
                return NCVM_LIB_FUNCTION_NOT_FOUND;
            }
    
            vm->lib_functions[i_func] = f;
            fn_names += strlen(fn_names) + 1;
        }
    }
    return NCVM_OK;
}
