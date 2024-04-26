#include <ncvm.h>
#include <stdlib.h>
#include <string.h>
#include <extc_stack.h>

/*#include <stdio.h>*/

#include "util.h"

stack_template_def(byte, u8)
stack_template_impl(byte, u8)

_export ncvm ncvm_initArr(
    Instruction* inst_p,
    u8* static_mem_p
) {
    ncvm ret;
    ret.inst_p = inst_p;
    /*ret.inst_count = inst_count;*/
    ret.static_mem_p = static_mem_p;

    return ret;
}


_export void ncvm_free(ncvm* vm) {
    free(vm->inst_p);
    free(vm->static_mem_p);
    vm = NULL;
}


_export u8 ncvm_execute(ncvm* vm, ThreadSettings settings) {
    /* Create main thread */
    int code;
    ncvm_create_thread(vm, vm->inst_p, NULL, 0, settings, &code);
    return code;
}

_export ncvm_thread ncvm_create_thread(
    ncvm* vm, const Instruction* si_p, u8* EST, usize EST_size,
    ThreadSettings settings, int* ret_code
) {
    ncvm_thread result;

    result.vm = vm;
    result.current_instr_p = si_p;

    /* Check is little indian*/
    if (!is_little_endian()) {
        if (ret_code != NULL)
            *ret_code = 1;
        return result;
    }

    /* Create stack, registers */
    u8 st_r;
    result.stack_p = malloc(sizeof(stack_byte));
    *(stack_byte*)result.stack_p = stack_byte_init(settings.stack_size, &st_r);
    if (st_r != 0) {
        if (ret_code != NULL)
            *ret_code = 2;
        return result;
    }
    
    /* Add extern stack to thread stack */
    if (EST != NULL)
        stack_byte_push_ptr((stack_byte*)result.stack_p, EST, EST_size);

    /* Alloc registers */
    result.u32_registers = (u32*)malloc(sizeof(u32) * settings.u32_reg_size);
    result.u64_registers = (u64*)malloc(sizeof(u64) * settings.u64_reg_size+1);
    result.f32_registers = (f32*)malloc(sizeof(f32) * settings.f32_reg_size);
    result.f64_registers = (f64*)malloc(sizeof(f64) * settings.f64_reg_size);
    
    /* Clear registers */
    memset(result.u32_registers, 0, sizeof(u32) * settings.u32_reg_size);
    memset(result.u64_registers, 0, sizeof(u64) * settings.u64_reg_size);
    memset(result.f32_registers, 0, sizeof(f32) * settings.f32_reg_size);
    memset(result.f64_registers, 0, sizeof(f64) * settings.f64_reg_size);

    if (ret_code != NULL)
        *ret_code = 0;
    return result;  
}


_export void ncvm_thread_free(ncvm_thread* thread) {
    stack_byte_free((stack_byte*)thread->stack_p);
    free(thread->u32_registers);
    free(thread->u64_registers);
    free(thread->f32_registers);
    free(thread->f64_registers);
    thread = NULL;
}


#define JUMP_TO_ADDR IP = &vm->inst_p[*addr_register] - 1;
#define EXECUTE_COMMAND\
    switch (IP->opcode) {\
        case STOP:\
            /* TODO: add regisetr */\
            goto while_exit;\
            break;\
        case RET:\
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
            *((u8*)(u32_registers+IP->r1)) = IP->r2;\
            *((u8*)(u32_registers+IP->r1)+1) = IP->r3;\
            break;\
        case LSR:\
            *((u8*)(u64_registers+IP->r1)) = IP->r2;\
            *((u8*)(u64_registers+IP->r1)+1) = IP->r3;\
            break;\
\
        /*case ISRF:\
            *((u8*)(u32_registers+IP->r1)) = IP->r2;\
            *((u8*)(u32_registers+IP->r1)+1) = IP->r3;\
            break;\
        case ISRS:\
            *((u8*)(u32_registers+IP->r1)+2) = IP->r2;\
            *((u8*)(u32_registers+IP->r1)+3) = IP->r3;\
            break;\
        case LSRF:\
            *((u8*)(u64_registers+IP->r1)) = IP->r2;\
            *((u8*)(u64_registers+IP->r1)+1) = IP->r3;\
            break;\
        case LSRS:\
            *((u8*)(u64_registers+IP->r1)+2) = IP->r2;\
            *((u8*)(u64_registers+IP->r1)+3) = IP->r3;\
            break;\
        case LSRT:\
            *((u8*)(u64_registers+IP->r1)+4) = IP->r2;\
            *((u8*)(u64_registers+IP->r1)+5) = IP->r3;\
            break;\
        case LSRQ:\
            *((u8*)(u64_registers+IP->r1)+6) = IP->r2;\
            *((u8*)(u64_registers+IP->r1)+7) = IP->r3;\
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
            stack_byte_pop_ptr(stack, IP->r1 + (IP->r2*256) + (IP->r3*65536));\
            break;\
        case POPA:\
            stack_byte_pop_ptr(stack, *addr_register);\
            break;\
        \
        case IPUSH:\
            stack_byte_push_ptr(stack, (u8*)&u32_registers[IP->r1], IP->r2);\
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
            stack_byte_push_ptr(stack, (u8*)&u64_registers[IP->r1], IP->r2);\
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
            stack_byte_push_ptr(stack, (u8*)&f32_registers[IP->r1], 4);\
            break;\
        case FSTLD:\
            memcpy(&f32_registers[IP->r1], &stack->data[*addr_register], 4);\
            break;\
        case FSTST:\
            memcpy(&stack->data[*addr_register], &f32_registers[IP->r1], 4);\
            break;\
\
        case DPUSH:\
            stack_byte_push_ptr(stack, (u8*)&f64_registers[IP->r1], 8);\
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
            u32_registers[IP->r1] = (u32)u64_registers[IP->r2];\
            break;\
        case FTOI:\
            u32_registers[IP->r1] = (u32)f32_registers[IP->r2];\
            break;\
        case DTOI:\
            u32_registers[IP->r1] = (u32)f64_registers[IP->r2];\
            break;\
        case ITOL:\
            u64_registers[IP->r1] = (u64)u32_registers[IP->r2];\
            break;\
        case FTOL:\
            u64_registers[IP->r1] = (u64)f32_registers[IP->r2];\
            break;\
        case DTOL:\
            u64_registers[IP->r1] = (u64)f64_registers[IP->r2];\
            break;\
        case ITOF:\
            f32_registers[IP->r1] = (f32)u32_registers[IP->r2];\
            break;\
        case LTOF:\
            f32_registers[IP->r1] = (f32)u64_registers[IP->r2];\
            break;\
        case DTOF:\
            f32_registers[IP->r1] = (f32)f64_registers[IP->r2];\
            break;\
        case ITOD:\
            f64_registers[IP->r1] = (f64)u32_registers[IP->r2];\
            break;\
        case LTOD:\
            f64_registers[IP->r1] = (f64)u64_registers[IP->r2];\
            break;\
        case FTOD:\
            f64_registers[IP->r1] = (f64)f32_registers[IP->r2];\
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
        default:\
            break;\
    }\
    ++IP;


_export  u8 ncvm_execute_thread_step(ncvm_thread* thread) {
    u32* u32_registers = thread->u32_registers;
    u64* u64_registers = thread->u64_registers;
    f32* f32_registers = thread->f32_registers;
    f64* f64_registers = thread->f64_registers;
    stack_byte* stack = (stack_byte*)thread->stack_p;
    ncvm *vm = thread->vm;

    u64* const addr_register = &thread->u64_registers[0];
    register const Instruction* IP = thread->current_instr_p;

    EXECUTE_COMMAND        

    while_exit:;
    return 0;
}


_export u8 ncvm_execute_thread(ncvm_thread* thread) {
    /*u64[0] - addr register*/
    /*u64* const addr_register = &result.u64_registers[0];*/
    u32* u32_registers = thread->u32_registers;
    u64* u64_registers = thread->u64_registers;
    f32* f32_registers = thread->f32_registers;
    f64* f64_registers = thread->f64_registers;
    stack_byte* stack = (stack_byte*)thread->stack_p;
    ncvm *vm = thread->vm;

    u64* const addr_register = &thread->u64_registers[0];
    register const Instruction* IP = thread->current_instr_p;
    while (true) {
        EXECUTE_COMMAND        
    }
    while_exit:;
    return 0;
}

