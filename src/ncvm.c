#include <ncvm.h>
#include <stdlib.h>
#include <string.h>
#include <extc_stack.h>
/*#include <extc_rint.h>*/

#include "util.h"

stack_template_def(byte, uint8_t)
stack_template_impl(byte, uint8_t)

stack_template_def(usize, unsigned long)
stack_template_impl(usize, unsigned long)


_export uint8_t ncvm_init(
    ncvm*             vm,
    Instruction*      inst_p,  /*unsigned long inst_count,*/
    uint8_t*    static_mem_p,/*, unsigned long static_mem_size*/
    ncvm_lib_function (*get_lib_function)(const char* name, void* lib_data_p),
    void*             lib_data_p
) {
    ThreadSettings settings;
    DefaultThreadSettingsANSI(settings)

    if (!is_little_endian())
        return NCVM_IS_BIG_ENDIAN;
    
    vm->inst_p = inst_p;
    /*ret.inst_count = inst_count;*/
    vm->static_mem_p = static_mem_p;
    vm->main_thread_settings = settings;
    return NCVM_OK;
}


_export void ncvm_free(ncvm* vm) {
    free(vm->inst_p);
    free(vm->static_mem_p);
    free(vm->lib_functions);
    for (unsigned long i = 0; i < vm->pub_functions_count; i++) {
        free(vm->pub_functions[i].name);
    }
    free(vm->pub_functions);
    /*vm = NULL;*/
}

_export uint8_t ncvm_execute(ncvm* vm) {
    /* Create main thread */
    ncvm_thread thread;
    uint8_t code =ncvm_create_thread(
        &thread,
        vm,
        vm->inst_p,
        NULL, 
        0, 
        vm->main_thread_settings
    );
    ncvm_execute_thread(&thread);
    return code;
}

_export uint8_t ncvm_create_thread(
    ncvm_thread*       thread,
    ncvm* vm, const Instruction* si_p, uint8_t* EST, unsigned long EST_size,
    ThreadSettings settings
) {
    uint8_t st_r;

    thread->vm = (void*)vm;
    thread->current_instr_p = si_p;

    /* Check is little indian*/
    if (!is_little_endian()) {
        return NCVM_IS_BIG_ENDIAN;
    }

    /* Create stack, registers */
    thread->stack_p = malloc(sizeof(stack_byte));
    *(stack_byte*)thread->stack_p = stack_byte_init(settings.stack_size, &st_r);
    if (st_r != 0) {
        return NCVM_STACK_ALLOC_ERROR;
    }

    thread->call_stack_p = malloc(sizeof(stack_usize));
    *(stack_usize*)thread->call_stack_p = stack_usize_init(settings.call_stack_size, &st_r);
    if (st_r != 0) {
        return NCVM_STACK_ALLOC_ERROR;
    }
    
    /* Add extern stack to thread stack */
    if (EST != NULL)
        stack_byte_push_ptr((stack_byte*)thread->stack_p, EST, EST_size);

    /* Alloc registers */
    thread->u32_registers = (uint32_t*)malloc(sizeof(uint32_t) * settings.u32_reg_size);
    thread->u64_registers = (uint64_t*)malloc(sizeof(uint64_t) * (settings.u64_reg_size+1));
    thread->f32_registers = (float*)malloc(sizeof(float) * settings.f32_reg_size);
    thread->f64_registers = (double*)malloc(sizeof(double) * settings.f64_reg_size);
    
    /* Clear registers */
    memset(thread->u32_registers, 0, sizeof(uint32_t) * settings.u32_reg_size);
    memset(thread->u64_registers, 0, sizeof(uint64_t) * (settings.u64_reg_size+1));
    memset(thread->f32_registers, 0, sizeof(float) * settings.f32_reg_size);
    memset(thread->f64_registers, 0, sizeof(double) * settings.f64_reg_size);


    return NCVM_OK;  
}


_export void ncvm_thread_free(ncvm_thread* thread) {
    stack_byte_free((stack_byte*)thread->stack_p);
    stack_usize_free((stack_usize*)thread->call_stack_p);
    free(thread->u32_registers);
    free(thread->u64_registers);
    free(thread->f32_registers);
    free(thread->f64_registers);
    /*thread = NULL;*/
}

_export uint8_t ncvm_find_function_addr(ncvm* vm, const char* name, unsigned long* addr) {
    for (unsigned long i = 0; i < vm->pub_functions_count; i++) {
        if (strcmp(vm->pub_functions[i].name, name) == 0) {
            *addr = vm->pub_functions[i].addr;
            return NCVM_OK;
        }
    }
    return NCVM_FUNCTION_NOT_FOUND;
}

#include <stdio.h>

#define JUMP_TO_ADDR IP = &vm->inst_p[*addr_register] - 1;
#define EXECUTE_COMMAND\
    switch (IP->opcode) {\
        case STOP:\
            /* TODO: add regisetr */\
            goto while_exit;\
            break;\
        case RET:\
            if (stack_usize_pop((stack_usize*)call_stack, (unsigned long*)addr_register) == 0)\
                goto while_exit;\
            JUMP_TO_ADDR\
            break;\
\
        case IMOV:\
            u32_registers[IP->r1] = u32_registers[IP->r2];\
            break;\
        case LMOV:\
            u64_registers[IP->r1] = u64_registers[IP->r2];\
            break;\
        case FMOV:\
            f32_registers[IP->r1] = f32_registers[IP->r2];\
            break;\
        case DMOV:\
            f64_registers[IP->r1] = f64_registers[IP->r2];\
            break;\
\
        case IRCLR:\
            u32_registers[IP->r1] = 0;\
            break;\
        case LRCLR:\
            u64_registers[IP->r1] = 0;\
            break;\
        case FRCLR:\
            f32_registers[IP->r1] = 0;\
            break;\
        case DRCLR:\
            f64_registers[IP->r1] = 0;\
            break;\
\
        case ISR:\
            *((uint8_t*)(u32_registers+IP->r1)) = IP->r2;\
            *((uint8_t*)(u32_registers+IP->r1)+1) = IP->r3;\
            break;\
        case LSR:\
            *((uint8_t*)(u64_registers+IP->r1)) = IP->r2;\
            *((uint8_t*)(u64_registers+IP->r1)+1) = IP->r3;\
            break;\
\
        /*case ISRF:\
            *((uint8_t*)(u32_registers+IP->r1)) = IP->r2;\
            *((uint8_t*)(u32_registers+IP->r1)+1) = IP->r3;\
            break;\
        case ISRS:\
            *((uint8_t*)(u32_registers+IP->r1)+2) = IP->r2;\
            *((uint8_t*)(u32_registers+IP->r1)+3) = IP->r3;\
            break;\
        case LSRF:\
            *((uint8_t*)(u64_registers+IP->r1)) = IP->r2;\
            *((uint8_t*)(u64_registers+IP->r1)+1) = IP->r3;\
            break;\
        case LSRS:\
            *((uint8_t*)(u64_registers+IP->r1)+2) = IP->r2;\
            *((uint8_t*)(u64_registers+IP->r1)+3) = IP->r3;\
            break;\
        case LSRT:\
            *((uint8_t*)(u64_registers+IP->r1)+4) = IP->r2;\
            *((uint8_t*)(u64_registers+IP->r1)+5) = IP->r3;\
            break;\
        case LSRQ:\
            *((uint8_t*)(u64_registers+IP->r1)+6) = IP->r2;\
            *((uint8_t*)(u64_registers+IP->r1)+7) = IP->r3;\
            break;*/\
\
        case IRSI:\
            u32_registers[IP->r1] >>= IP->r2;\
            break;\
        case ILSI:\
            u32_registers[IP->r1] <<= IP->r2;\
            break;\
        case LRSI:\
            u64_registers[IP->r1] >>= IP->r2;\
            break;\
        case LLSI:\
            u64_registers[IP->r1] <<= IP->r2;\
            break;\
\
        case IRSA:\
            u32_registers[IP->r1] >>= *addr_register;\
            break;\
        case ILSA:\
            u32_registers[IP->r1] <<= *addr_register;\
            break;\
        case LRSA:\
            u64_registers[IP->r1] >>= *addr_register;\
            break;\
        case LLSA:\
            u64_registers[IP->r1] <<= *addr_register;\
            break;\
\
        case ISMLD:\
            u32_registers[IP->r1] = 0;\
            memcpy(&u32_registers[IP->r1], &vm->static_mem_p[*addr_register], IP->r2);\
            break;\
        case ISMST:\
            memcpy(&vm->static_mem_p[*addr_register], &u32_registers[IP->r1], IP->r2);\
            break;\
\
        case LSMLD:\
            u64_registers[IP->r1] = 0;\
            memcpy(&u64_registers[IP->r1], &vm->static_mem_p[*addr_register], IP->r2);\
            break;\
        case LSMST:\
            memcpy(&vm->static_mem_p[*addr_register], &u64_registers[IP->r1], IP->r2);\
            break;\
\
        case FSMLD:\
            memcpy(&f32_registers[IP->r1], &vm->static_mem_p[*addr_register], 4);\
            break;\
        case FSMST:\
            memcpy(&vm->static_mem_p[*addr_register], &f32_registers[IP->r1], 4);\
            break;\
\
        case DSMLD:\
            memcpy(&f64_registers[IP->r1], &vm->static_mem_p[*addr_register], 8);\
            break;\
        case DSMST:\
            memcpy(&vm->static_mem_p[*addr_register], &f64_registers[IP->r1], 8);\
            break;\
\
\
        case POPI:\
            stack_byte_pop_ptr(stack, IP->r1 + (IP->r2*256) + (IP->r3*65536), NULL);\
            break;\
        case POPA:\
            stack_byte_pop_ptr(stack, *addr_register, NULL);\
            break;\
        \
        case IPUSH:\
            stack_byte_push_ptr(stack, (uint8_t*)&u32_registers[IP->r1], IP->r2);\
            break;\
        case ISTLD:\
            u32_registers[IP->r1] = 0;\
            memcpy(&u32_registers[IP->r1], &stack->data[*addr_register], IP->r2);\
            break;\
        case ISTST:\
            memcpy(&stack->data[*addr_register], &u32_registers[IP->r1], IP->r2);\
            break;\
\
        case LPUSH:\
            stack_byte_push_ptr(stack, (uint8_t*)&u64_registers[IP->r1], IP->r2);\
            break;\
        case LSTLD:\
            u64_registers[IP->r1] = 0;\
            memcpy(&u64_registers[IP->r1], &stack->data[*addr_register], IP->r2);\
            break;\
        case LSTST:\
            memcpy(&stack->data[*addr_register], &u64_registers[IP->r1], IP->r2);\
            break;\
\
        case FPUSH:\
            stack_byte_push_ptr(stack, (uint8_t*)&f32_registers[IP->r1], 4);\
            break;\
        case FSTLD:\
            memcpy(&f32_registers[IP->r1], &stack->data[*addr_register], 4);\
            break;\
        case FSTST:\
            memcpy(&stack->data[*addr_register], &f32_registers[IP->r1], 4);\
            break;\
\
        case DPUSH:\
            stack_byte_push_ptr(stack, (uint8_t*)&f64_registers[IP->r1], 8);\
            break;\
        case DSTLD:\
            memcpy(&f64_registers[IP->r1], &stack->data[*addr_register], 8);\
            break;\
        case DSTST:\
            memcpy(&stack->data[*addr_register], &f64_registers[IP->r1], 8);\
            break;\
\
\
        /*\
            HEAP\
        */\
\
\
        case IADD:\
            u32_registers[IP->r1] = u32_registers[IP->r2] + u32_registers[IP->r3];\
            break;\
        case ISUB:\
            u32_registers[IP->r1] = u32_registers[IP->r2] - u32_registers[IP->r3];\
            break;\
        case IMULT:\
            u32_registers[IP->r1] = u32_registers[IP->r2] * u32_registers[IP->r3];\
            break;\
        case IDIV:\
            u32_registers[IP->r1] = u32_registers[IP->r2] / u32_registers[IP->r3];\
            break;\
        case IMOD:\
            u32_registers[IP->r1] = u32_registers[IP->r2] % u32_registers[IP->r3];\
            break;\
        case IINC:\
            ++u32_registers[IP->r1];\
            break;\
        case IDEC:\
            --u32_registers[IP->r1];\
            break;\
\
        case LADD:\
            u64_registers[IP->r1] = u64_registers[IP->r2] + u64_registers[IP->r3];\
            break;\
        case LSUB:\
            u64_registers[IP->r1] = u64_registers[IP->r2] - u64_registers[IP->r3];\
            break;\
        case LMULT:\
            u64_registers[IP->r1] = u64_registers[IP->r2] * u64_registers[IP->r3];\
            break;\
        case LDIV:\
            u64_registers[IP->r1] = u64_registers[IP->r2] / u64_registers[IP->r3];\
            break;\
        case LMOD:\
            u64_registers[IP->r1] = u64_registers[IP->r2] % u64_registers[IP->r3];\
            break;\
        case LINC:\
            ++u64_registers[IP->r1];\
            break;\
        case LDEC:\
            --u64_registers[IP->r1];\
            break;\
\
        case FADD:\
            f32_registers[IP->r1] = f32_registers[IP->r2] + f32_registers[IP->r3];\
            break;\
        case FSUB:\
            f32_registers[IP->r1] = f32_registers[IP->r2] - f32_registers[IP->r3];\
            break;\
        case FMULT:\
            f32_registers[IP->r1] = f32_registers[IP->r2] * f32_registers[IP->r3];\
            break;\
        case FDIV:\
            f32_registers[IP->r1] = f32_registers[IP->r2] / f32_registers[IP->r3];\
            break;\
        case FINC:\
            ++f32_registers[IP->r1];\
            break;\
        case FDEC:\
            --f32_registers[IP->r1];\
            break;\
\
        case DADD:\
            f64_registers[IP->r1] = f64_registers[IP->r2] + f64_registers[IP->r3];\
            break;\
        case DSUB:\
            f64_registers[IP->r1] = f64_registers[IP->r2] - f64_registers[IP->r3];\
            break;\
        case DMULT:\
            f64_registers[IP->r1] = f64_registers[IP->r2] * f64_registers[IP->r3];\
            break;\
        case DDIV:\
            f64_registers[IP->r1] = f64_registers[IP->r2] / f64_registers[IP->r3];\
            break;\
        case DINC:\
            ++f64_registers[IP->r1];\
            break;\
        case DDEC:\
            --f64_registers[IP->r1];\
            break;\
\
        case LTOI:\
            u32_registers[IP->r1] = (uint32_t)u64_registers[IP->r2];\
            break;\
        case FTOI:\
            u32_registers[IP->r1] = (uint32_t)f32_registers[IP->r2];\
            break;\
        case DTOI:\
            u32_registers[IP->r1] = (uint32_t)f64_registers[IP->r2];\
            break;\
        case ITOL:\
            u64_registers[IP->r1] = (uint64_t)u32_registers[IP->r2];\
            break;\
        case FTOL:\
            u64_registers[IP->r1] = (uint64_t)f32_registers[IP->r2];\
            break;\
        case DTOL:\
            u64_registers[IP->r1] = (uint64_t)f64_registers[IP->r2];\
            break;\
        case ITOF:\
            f32_registers[IP->r1] = (float)u32_registers[IP->r2];\
            break;\
        case LTOF:\
            f32_registers[IP->r1] = (float)u64_registers[IP->r2];\
            break;\
        case DTOF:\
            f32_registers[IP->r1] = (float)f64_registers[IP->r2];\
            break;\
        case ITOD:\
            f64_registers[IP->r1] = (double)u32_registers[IP->r2];\
            break;\
        case LTOD:\
            f64_registers[IP->r1] = (double)u64_registers[IP->r2];\
            break;\
        case FTOD:\
            f64_registers[IP->r1] = (double)f32_registers[IP->r2];\
            break;\
\
\
        case JMP:\
            JUMP_TO_ADDR\
            break;\
\
        case IJEZ:\
            if (u32_registers[IP->r1] == 0) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case IJNZ:\
            if (u32_registers[IP->r1] != 0) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case IJEQ:\
            if (u32_registers[IP->r1] == u32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case IJNQ:\
            if (u32_registers[IP->r1] != u32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case IJML:\
            if (u32_registers[IP->r1] < u32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case IJEL:\
            if (u32_registers[IP->r1] <= u32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case IJMG:\
            if (u32_registers[IP->r1] > u32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case IJEG:\
            if (u32_registers[IP->r1] >= u32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
\
        case LJEZ:\
            if (u64_registers[IP->r1] == 0) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case LJNZ:\
            if (u64_registers[IP->r1] != 0) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case LJEQ:\
            if (u64_registers[IP->r1] == u64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case LJNQ:\
            if (u64_registers[IP->r1] != u64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case LJML:\
            if (u64_registers[IP->r1] < u64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case LJEL:\
            if (u64_registers[IP->r1] <= u64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case LJMG:\
            if (u64_registers[IP->r1] > u64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case LJEG:\
            if (u64_registers[IP->r1] >= u64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case FJEZ:\
            if (f32_registers[IP->r1] == 0) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case FJNZ:\
            if (f32_registers[IP->r1] != 0) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case FJEQ:\
            if (f32_registers[IP->r1] == f32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case FJNQ:\
            if (f32_registers[IP->r1] != f32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case FJML:\
            if (f32_registers[IP->r1] < f32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case FJEL:\
            if (f32_registers[IP->r1] <= f32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case FJMG:\
            if (f32_registers[IP->r1] > f32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case FJEG:\
            if (f32_registers[IP->r1] >= f32_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
\
        case DJEZ:\
            if (f64_registers[IP->r1] == 0) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case DJNZ:\
            if (f64_registers[IP->r1] != 0) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case DJEQ:\
            if (f64_registers[IP->r1] == f64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case DJNQ:\
            if (f64_registers[IP->r1] != f64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case DJML:\
            if (f64_registers[IP->r1] < f64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case DJEL:\
            if (f64_registers[IP->r1] <= f64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case DJMG:\
            if (f64_registers[IP->r1] > f64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
        case DJEG:\
            if (f64_registers[IP->r1] >= f64_registers[IP->r2]) {\
                JUMP_TO_ADDR\
            }\
            break;\
\
        case CALL:\
            if (stack_usize_push((stack_usize*)call_stack, IP - vm->inst_p + 1) == 0)\
                return 1;\
            JUMP_TO_ADDR\
            break;\
\
        case LIBCALL:\
            ((ncvm_lib_function)vm->lib_functions[*addr_register])(thread);\
            break;\
    }\
    ++IP;


_export  uint8_t ncvm_execute_thread_step(ncvm_thread* thread) {
    /*uint32_t* u32_registers = thread->u32_registers;
    u64* u64_registers = thread->u64_registers;
    float* f32_registers = thread->f32_registers;
    double* f64_registers = thread->f64_registers;
    stack_byte* stack  = (stack_byte*)thread->stack_p;
    stack_usize* call_stack = (stack_usize*)thread->call_stack_p;
    ncvm *vm = thread->vm;

    u64* const addr_register = &thread->u64_registers[0];
    const Instruction* IP = thread->current_instr_p;*/

    /*EXECUTE_COMMAND*/    

    /*while_exit:;
    thread->current_instr_p = IP;*/
    return 0;
}

/*#if defined (__GNUC__) || defined (__clang__) || defined (NCVM_USE_JUMPTABLE)*/
/*#define __NCVM_USE_JUMPTABLE*/
#ifdef NCVM_USE_JUMPTABLE
#define INSTRUCTION_END continue;
#define INSTRUCTION_CASE 
#define CYCLE_END break;
#else
#define INSTRUCTION_END break;
#define INSTRUCTION_CASE case
#define CYCLE_END goto while_exit;
#endif

#define DEREF_ADDR unsigned_long_long_to_usize((*addr_register))

_export uint8_t ncvm_execute_thread(ncvm_thread* thread) {
    uint32_t* u32_registers = thread->u32_registers;
    uint64_t* u64_registers = thread->u64_registers;
    float* f32_registers = thread->f32_registers;
    double* f64_registers = thread->f64_registers;
    stack_byte* stack = (stack_byte*)thread->stack_p;
    stack_usize* call_stack = (stack_usize*)thread->call_stack_p;
    ncvm *vm = (ncvm*)thread->vm;

    #ifdef NCVM_USE_JUMPTABLE
    static const void *const labels[124] = {
        &&NOP,    &&STOP,  &&RET,   &&IMOV,  &&LMOV,  &&FMOV,  &&DMOV,
        &&IRCLR,  &&LRCLR, &&FRCLR, &&DRCLR, &&ISR,   &&LSR,   &&IRSI,
        &&ILSI,   &&LRSI,  &&LLSI,  &&IRSA,  &&ILSA,  &&LRSA,  &&LLSA,
        &&ISMLD,  &&ISMST, &&LSMLD, &&LSMST, &&FSMLD, &&FSMST, &&DSMLD,
        &&DSMST,  &&POPI,  &&POPA,  &&IPUSH, &&ISTLD, &&ISTST, &&LPUSH,
        &&LSTLD,  &&LSTST, &&FPUSH, &&FSTLD, &&FSTST, &&DPUSH, &&DSTLD,
        &&DSTST,  &&ALLOC, &&FREE,  &&HELD,  &&HEST,  &&IADD,  &&ISUB,
        &&IMULT,  &&IDIV,  &&IMOD,  &&IINC,  &&IDEC, &&INEG,  &&LADD,  &&LSUB,
        &&LMULT,  &&LDIV,  &&LMOD,  &&LINC,  &&LDEC, &&LNEG,  &&FADD,  &&FSUB,
        &&FMULT,  &&FDIV,  &&FINC,  &&FDEC, &&FNEG,  &&DADD,  &&DSUB,  &&DMULT,
        &&DDIV,   &&DINC,  &&DDEC, &&DNEG,  &&LTOI,  &&FTOI,  &&DTOI,  &&ITOL,
        &&FTOL,   &&DTOL,  &&ITOF,  &&LTOF,  &&DTOF,  &&ITOD,  &&LTOD,
        &&FTOD,   &&JMP,   &&IJEZ,  &&IJNZ,  &&IJEQ,  &&IJNQ,  &&IJML,
        &&IJEL,   &&IJMG,  &&IJEG,  &&LJEZ,  &&LJNZ,  &&LJEQ,  &&LJNQ,
        &&LJML,   &&LJEL,  &&LJMG,  &&LJEG,  &&FJEZ,  &&FJNZ,  &&FJEQ,
        &&FJNQ,   &&FJML,  &&FJEL,  &&FJMG,  &&FJEG,  &&DJEZ,  &&DJNZ,
        &&DJEQ,   &&DJNQ,  &&DJML,  &&DJEL,  &&DJMG,  &&DJEG,  &&CALL,
        &&LIBCALL
    };
    #endif

    uint64_t* const addr_register = &thread->u64_registers[0];
    register const Instruction* IP = thread->current_instr_p-1;

    while (1) {
        ++IP;
        /* printf("%d\n", IP->opcode); */
        #ifdef NCVM_USE_JUMPTABLE
            goto *labels[IP->opcode];
        #else
            switch (IP->opcode) {
        #endif


        /* EXECUTE_COMMAND */
    INSTRUCTION_CASE NOP:
        INSTRUCTION_END;
    INSTRUCTION_CASE STOP:
        /* TODO: add regisetr */
        CYCLE_END;
    INSTRUCTION_CASE RET:
        if (stack_usize_pop((stack_usize *)call_stack,
                            (unsigned long *)addr_register) == 0)
            /* goto while_exit; */
            /*break;*/
            CYCLE_END
        JUMP_TO_ADDR
        INSTRUCTION_END;

    INSTRUCTION_CASE IMOV:
        u32_registers[IP->r1] = u32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE LMOV:
        u64_registers[IP->r1] = u64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE FMOV:
        f32_registers[IP->r1] = f32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE DMOV:
        f64_registers[IP->r1] = f64_registers[IP->r2];
        INSTRUCTION_END;

    INSTRUCTION_CASE IRCLR:
        u32_registers[IP->r1] = 0;
        INSTRUCTION_END;
    INSTRUCTION_CASE LRCLR:
        u64_registers[IP->r1] = 0;
        INSTRUCTION_END;
    INSTRUCTION_CASE FRCLR:
        f32_registers[IP->r1] = 0;
        INSTRUCTION_END;
    INSTRUCTION_CASE DRCLR:
        f64_registers[IP->r1] = 0;
        INSTRUCTION_END;

    INSTRUCTION_CASE ISR:
        *((uint8_t *)(u32_registers + IP->r1)) = IP->r2;
        *((uint8_t *)(u32_registers + IP->r1) + 1) = IP->r3;
        INSTRUCTION_END;
    INSTRUCTION_CASE LSR:
        *((uint8_t *)(u64_registers + IP->r1)) = IP->r2;
        *((uint8_t *)(u64_registers + IP->r1) + 1) = IP->r3;
        INSTRUCTION_END;

        /*ISRF:
            *((uint8_t*)(u32_registers+IP->r1)) = IP->r2;
            *((uint8_t*)(u32_registers+IP->r1)+1) = IP->r3;
            INSTRUCTION_END;
        ISRS:
            *((uint8_t*)(u32_registers+IP->r1)+2) = IP->r2;
            *((uint8_t*)(u32_registers+IP->r1)+3) = IP->r3;
            INSTRUCTION_END;
        LSRF:
            *((uint8_t*)(u64_registers+IP->r1)) = IP->r2;
            *((uint8_t*)(u64_registers+IP->r1)+1) = IP->r3;
            INSTRUCTION_END;
        LSRS:
            *((uint8_t*)(u64_registers+IP->r1)+2) = IP->r2;
            *((uint8_t*)(u64_registers+IP->r1)+3) = IP->r3;
            INSTRUCTION_END;
        LSRT:
            *((uint8_t*)(u64_registers+IP->r1)+4) = IP->r2;
            *((uint8_t*)(u64_registers+IP->r1)+5) = IP->r3;
            INSTRUCTION_END;
        LSRQ:
            *((uint8_t*)(u64_registers+IP->r1)+6) = IP->r2;
            *((uint8_t*)(u64_registers+IP->r1)+7) = IP->r3;
            INSTRUCTION_END;*/

    INSTRUCTION_CASE IRSI:
        u32_registers[IP->r1] = u32_registers[IP->r2] >> IP->r3;
        INSTRUCTION_END;
    INSTRUCTION_CASE ILSI:
        u32_registers[IP->r1] = u32_registers[IP->r2] << IP->r3;
        INSTRUCTION_END;
    INSTRUCTION_CASE LRSI:
        u64_registers[IP->r1] = u64_registers[IP->r2] >> IP->r3;
        INSTRUCTION_END;
    INSTRUCTION_CASE LLSI:
        u64_registers[IP->r1] = u64_registers[IP->r2] << IP->r3;
        INSTRUCTION_END;

    INSTRUCTION_CASE IRSA:
        u32_registers[IP->r1] = u32_registers[IP->r2] >> *addr_register;
        INSTRUCTION_END;
    INSTRUCTION_CASE ILSA:
        u32_registers[IP->r1] = u32_registers[IP->r2] << *addr_register;
        INSTRUCTION_END;
    INSTRUCTION_CASE LRSA:
        u64_registers[IP->r1] = u64_registers[IP->r2] >> *addr_register;
        INSTRUCTION_END;
    INSTRUCTION_CASE LLSA:
        u64_registers[IP->r1] = u64_registers[IP->r2] << *addr_register;
        INSTRUCTION_END;

    INSTRUCTION_CASE ISMLD:
        u32_registers[IP->r1] = 0;
        memcpy(&u32_registers[IP->r1], &vm->static_mem_p[*addr_register],
               IP->r2);
        INSTRUCTION_END;
    INSTRUCTION_CASE ISMST:
        memcpy(&vm->static_mem_p[*addr_register], &u32_registers[IP->r1],
               IP->r2);
        INSTRUCTION_END;

    INSTRUCTION_CASE LSMLD:
        u64_registers[IP->r1] = 0;
        memcpy(&u64_registers[IP->r1], &vm->static_mem_p[*addr_register],
               IP->r2);
        INSTRUCTION_END;
    INSTRUCTION_CASE LSMST:
        memcpy(&vm->static_mem_p[*addr_register], &u64_registers[IP->r1],
               IP->r2);
        INSTRUCTION_END;

    INSTRUCTION_CASE FSMLD:
        memcpy(&f32_registers[IP->r1], &vm->static_mem_p[*addr_register], 4);
        INSTRUCTION_END;
    INSTRUCTION_CASE FSMST:
        memcpy(&vm->static_mem_p[*addr_register], &f32_registers[IP->r1], 4);
        INSTRUCTION_END;

    INSTRUCTION_CASE DSMLD:
        memcpy(&f64_registers[IP->r1], &vm->static_mem_p[*addr_register], 8);
        INSTRUCTION_END;
    INSTRUCTION_CASE DSMST:
        memcpy(&vm->static_mem_p[*addr_register], &f64_registers[IP->r1], 8);
        INSTRUCTION_END;

    INSTRUCTION_CASE POPI:
        if (stack_byte_pop_ptr(stack, IP->r1 + (IP->r2 * 256) + (IP->r3 * 65536),
                           NULL) == 0)
                           return NCVM_STACK_UNDERFLOW;
        INSTRUCTION_END;
    INSTRUCTION_CASE POPA:
        if (stack_byte_pop_ptr(stack, *addr_register, NULL) == 0)
            return NCVM_STACK_UNDERFLOW;
        INSTRUCTION_END;

    INSTRUCTION_CASE IPUSH:
        if (stack_byte_push_ptr(stack, (uint8_t *)&u32_registers[IP->r1], IP->r2) == 0)
            return NCVM_STACK_OVERFLOW;
        INSTRUCTION_END;
    INSTRUCTION_CASE ISTLD:
        u32_registers[IP->r1] = 0;
        memcpy(&u32_registers[IP->r1], &stack->data[*addr_register], IP->r2);
        INSTRUCTION_END;
    INSTRUCTION_CASE ISTST:
        memcpy(&stack->data[*addr_register], &u32_registers[IP->r1], IP->r2);
        INSTRUCTION_END;

    INSTRUCTION_CASE LPUSH:
        if (stack_byte_push_ptr(stack, (uint8_t *)&u64_registers[IP->r1], IP->r2) == 0)
            return NCVM_STACK_OVERFLOW;
        INSTRUCTION_END;
    INSTRUCTION_CASE LSTLD:
        u64_registers[IP->r1] = 0;
        memcpy(&u64_registers[IP->r1], &stack->data[*addr_register], IP->r2);
        INSTRUCTION_END;
    INSTRUCTION_CASE LSTST:
        memcpy(&stack->data[*addr_register], &u64_registers[IP->r1], IP->r2);
        INSTRUCTION_END;

    INSTRUCTION_CASE FPUSH:
        if (stack_byte_push_ptr(stack, (uint8_t *)&f32_registers[IP->r1], 4) == 0) 
            return NCVM_STACK_OVERFLOW;
        INSTRUCTION_END;
    INSTRUCTION_CASE FSTLD:
        memcpy(&f32_registers[IP->r1], &stack->data[*addr_register], 4);
        INSTRUCTION_END;
    INSTRUCTION_CASE FSTST:
        memcpy(&stack->data[*addr_register], &f32_registers[IP->r1], 4);
        INSTRUCTION_END;

    INSTRUCTION_CASE DPUSH:
        if (stack_byte_push_ptr(stack, (uint8_t *)&f64_registers[IP->r1], 8) == 0)
            return NCVM_STACK_OVERFLOW;
        INSTRUCTION_END;
    INSTRUCTION_CASE DSTLD:
        memcpy(&f64_registers[IP->r1], &stack->data[*addr_register], 8);
        INSTRUCTION_END;
    INSTRUCTION_CASE DSTST:
        memcpy(&stack->data[*addr_register], &f64_registers[IP->r1], 8);
        INSTRUCTION_END;

        /*
            HEAP
        */

    INSTRUCTION_CASE ALLOC:
        INSTRUCTION_END;
    INSTRUCTION_CASE FREE:
        INSTRUCTION_END;
    INSTRUCTION_CASE HEST:
        INSTRUCTION_END;
    INSTRUCTION_CASE HELD:
        INSTRUCTION_END;

    INSTRUCTION_CASE IADD:
        u32_registers[IP->r1] = u32_registers[IP->r2] + u32_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE ISUB:
        u32_registers[IP->r1] = u32_registers[IP->r2] - u32_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE IMULT:
        u32_registers[IP->r1] = u32_registers[IP->r2] * u32_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE IDIV:
        u32_registers[IP->r1] = u32_registers[IP->r2] / u32_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE IMOD:
        u32_registers[IP->r1] = u32_registers[IP->r2] % u32_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE IINC:
        ++u32_registers[IP->r1];
        INSTRUCTION_END;
    INSTRUCTION_CASE IDEC:
        --u32_registers[IP->r1];
        INSTRUCTION_END;
    INSTRUCTION_CASE INEG:
        u32_registers[IP->r1] = -u32_registers[IP->r2];
        INSTRUCTION_END;

    INSTRUCTION_CASE LADD:
        u64_registers[IP->r1] = u64_registers[IP->r2] + u64_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE LSUB:
        u64_registers[IP->r1] = u64_registers[IP->r2] - u64_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE LMULT:
        u64_registers[IP->r1] = u64_registers[IP->r2] * u64_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE LDIV:
        u64_registers[IP->r1] = u64_registers[IP->r2] / u64_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE LMOD:
        u64_registers[IP->r1] = u64_registers[IP->r2] % u64_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE LINC:
        ++u64_registers[IP->r1];
        INSTRUCTION_END;
    INSTRUCTION_CASE LDEC:
        --u64_registers[IP->r1];
        INSTRUCTION_END;
    INSTRUCTION_CASE LNEG:
        u64_registers[IP->r1] = -u64_registers[IP->r2];
        INSTRUCTION_END;

    INSTRUCTION_CASE FADD:
        f32_registers[IP->r1] = f32_registers[IP->r2] + f32_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE FSUB:
        f32_registers[IP->r1] = f32_registers[IP->r2] - f32_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE FMULT:
        f32_registers[IP->r1] = f32_registers[IP->r2] * f32_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE FDIV:
        f32_registers[IP->r1] = f32_registers[IP->r2] / f32_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE FINC:
        ++f32_registers[IP->r1];
        INSTRUCTION_END;
    INSTRUCTION_CASE FDEC:
        --f32_registers[IP->r1];
        INSTRUCTION_END;
    INSTRUCTION_CASE FNEG:
        f32_registers[IP->r1] = -f32_registers[IP->r2];
        INSTRUCTION_END;

    INSTRUCTION_CASE DADD:
        f64_registers[IP->r1] = f64_registers[IP->r2] + f64_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE DSUB:
        f64_registers[IP->r1] = f64_registers[IP->r2] - f64_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE DMULT:
        f64_registers[IP->r1] = f64_registers[IP->r2] * f64_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE DDIV:
        f64_registers[IP->r1] = f64_registers[IP->r2] / f64_registers[IP->r3];
        INSTRUCTION_END;
    INSTRUCTION_CASE DINC:
        ++f64_registers[IP->r1];
        INSTRUCTION_END;
    INSTRUCTION_CASE DDEC:
        --f64_registers[IP->r1];
        INSTRUCTION_END;
    INSTRUCTION_CASE DNEG:
        f64_registers[IP->r1] = -f64_registers[IP->r2];
        INSTRUCTION_END;

    INSTRUCTION_CASE LTOI:
        u32_registers[IP->r1] = (uint32_t)u64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE FTOI:
        u32_registers[IP->r1] = (uint32_t)f32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE DTOI:
        u32_registers[IP->r1] = (uint32_t)f64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE ITOL:
        u64_registers[IP->r1] = (uint64_t)u32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE FTOL:
        u64_registers[IP->r1] = (uint64_t)f32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE DTOL:
        u64_registers[IP->r1] = (uint64_t)f64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE ITOF:
        f32_registers[IP->r1] = (float)u32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE LTOF:
        f32_registers[IP->r1] = (float)u64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE DTOF:
        f32_registers[IP->r1] = (float)f64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE ITOD:
        f64_registers[IP->r1] = (double)u32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE LTOD:
        f64_registers[IP->r1] = (double)u64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE FTOD:
        f64_registers[IP->r1] = (double)f32_registers[IP->r2];
        INSTRUCTION_END;

    INSTRUCTION_CASE JMP:
        JUMP_TO_ADDR
        INSTRUCTION_END;

    INSTRUCTION_CASE IJEZ:
        if (u32_registers[IP->r1] == 0) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE IJNZ:
        if (u32_registers[IP->r1] != 0) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE IJEQ:
        if (u32_registers[IP->r1] == u32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE IJNQ:
        if (u32_registers[IP->r1] != u32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE IJML:
        if (u32_registers[IP->r1] < u32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE IJEL:
        if (u32_registers[IP->r1] <= u32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE IJMG:
        if (u32_registers[IP->r1] > u32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE IJEG:
        if (u32_registers[IP->r1] >= u32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;

    INSTRUCTION_CASE LJEZ:
        if (u64_registers[IP->r1] == 0) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJNZ:
        if (u64_registers[IP->r1] != 0) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJEQ:
        if (u64_registers[IP->r1] == u64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJNQ:
        if (u64_registers[IP->r1] != u64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJML:
        if (u64_registers[IP->r1] < u64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJEL:
        if (u64_registers[IP->r1] <= u64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJMG:
        if (u64_registers[IP->r1] > u64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJEG:
        if (u64_registers[IP->r1] >= u64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE FJEZ:
        if (f32_registers[IP->r1] == 0) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE FJNZ:
        if (f32_registers[IP->r1] != 0) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE FJEQ:
        if (f32_registers[IP->r1] == f32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE FJNQ:
        if (f32_registers[IP->r1] != f32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE FJML:
        if (f32_registers[IP->r1] < f32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE FJEL:
        if (f32_registers[IP->r1] <= f32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE FJMG:
        if (f32_registers[IP->r1] > f32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE FJEG:
        if (f32_registers[IP->r1] >= f32_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;

    INSTRUCTION_CASE DJEZ:
        if (f64_registers[IP->r1] == 0) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE DJNZ:
        if (f64_registers[IP->r1] != 0) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE DJEQ:
        if (f64_registers[IP->r1] == f64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE DJNQ:
        if (f64_registers[IP->r1] != f64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE DJML:
        if (f64_registers[IP->r1] < f64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE DJEL:
        if (f64_registers[IP->r1] <= f64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE DJMG:
        if (f64_registers[IP->r1] > f64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE DJEG:
        if (f64_registers[IP->r1] >= f64_registers[IP->r2]) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;

    INSTRUCTION_CASE CALL:
        if (stack_usize_push((stack_usize *)call_stack, IP - vm->inst_p + 1) ==
            0)
            return NCVM_CALL_STACK_OVERFLOW;
        JUMP_TO_ADDR
        INSTRUCTION_END;

    INSTRUCTION_CASE LIBCALL:
        ((ncvm_lib_function)vm->lib_functions[*addr_register])(thread);
        INSTRUCTION_END;
        /* ++IP; */
        #ifndef NCVM_USE_JUMPTABLE
        }
        #endif
    }
        
    #ifndef NCVM_USE_JUMPTABLE
    while_exit:;
    #endif
    thread->current_instr_p = IP;
    return 0;
}

