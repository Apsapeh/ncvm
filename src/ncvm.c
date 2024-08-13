#include <ncvm.h>
#include <stdlib.h>
#include <string.h>
#include <extc_stack.h>
#include <extc_rint.h>

#include "util.h"

stack_template_def(byte, u8)
stack_template_impl(byte, u8)

stack_template_def(usize, usize)
stack_template_impl(usize, usize)


_export ncvm ncvm_init(
    Instruction*      inst_p,  /*unsigned long inst_count,*/
    unsigned char*    static_mem_p,/*, unsigned long static_mem_size*/
    ncvm_lib_function (*get_lib_function)(const char* name, void* lib_data_p),
    void*             lib_data_p
) {
    ncvm ret;
    ThreadSettings settings;
    DefaultThreadSettingsANSI(settings)
    
    ret.inst_p = inst_p;
    /*ret.inst_count = inst_count;*/
    ret.static_mem_p = static_mem_p;
    ret.main_thread_settings = settings;
    return ret;
}


_export void ncvm_free(ncvm* vm) {
    free(vm->inst_p);
    free(vm->static_mem_p);
    /*vm = NULL;*/
}

_export u8 ncvm_execute(ncvm* vm) {
    /* Create main thread */
    int code;
    ncvm_thread thread = ncvm_create_thread(
        vm,
        vm->inst_p,
        NULL, 
        0, 
        vm->main_thread_settings, 
        &code
    );
    ncvm_execute_thread(&thread);
    return code;
}

_export ncvm_thread ncvm_create_thread(
    ncvm* vm, const Instruction* si_p, u8* EST, usize EST_size,
    ThreadSettings settings, int* ret_code
) {
    ncvm_thread result;
    u8 st_r;

    result.vm = vm;
    result.current_instr_p = si_p;

    /* Check is little indian*/
    if (!is_little_endian()) {
        if (ret_code != NULL)
            *ret_code = 1;
        return result;
    }

    /* Create stack, registers */
    result.stack_p = malloc(sizeof(stack_byte));
    *(stack_byte*)result.stack_p = stack_byte_init(settings.stack_size, &st_r);
    if (st_r != 0) {
        if (ret_code != NULL)
            *ret_code = 2;
        return result;
    }

    result.call_stack_p = malloc(sizeof(stack_usize));
    *(stack_usize*)result.call_stack_p = stack_usize_init(settings.call_stack_size, &st_r);
    if (st_r != 0) {
        if (ret_code != NULL)
            *ret_code = 3;
        return result;
    }
    
    /* Add extern stack to thread stack */
    if (EST != NULL)
        stack_byte_push_ptr((stack_byte*)result.stack_p, EST, EST_size);

    /* Alloc registers */
    result.u32_registers = (u32*)malloc(sizeof(u32) * settings.u32_reg_size);
    result.u64_registers = (unsigned_long_long*)malloc(sizeof(unsigned_long_long) * (settings.u64_reg_size+1));
    result.f32_registers = (f32*)malloc(sizeof(f32) * settings.f32_reg_size);
    result.f64_registers = (f64*)malloc(sizeof(f64) * settings.f64_reg_size);
    
    /* Clear registers */
    memset(result.u32_registers, 0, sizeof(u32) * settings.u32_reg_size);
    memset(result.u64_registers, 0, sizeof(unsigned_long_long) * (settings.u64_reg_size+1));
    memset(result.f32_registers, 0, sizeof(f32) * settings.f32_reg_size);
    memset(result.f64_registers, 0, sizeof(f64) * settings.f64_reg_size);

    if (ret_code != NULL)
        *ret_code = 0;
    return result;  
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

#include <stdio.h>

#define JUMP_TO_ADDR IP = &vm->inst_p[unsigned_long_long_to_usize((*addr_register))] - 1;
#define EXECUTE_COMMAND\
    switch (IP->opcode) {\
        case STOP:\
            /* TODO: add regisetr */\
            goto while_exit;\
            break;\
        case RET:\
            if (stack_usize_pop((stack_usize*)call_stack, (usize*)addr_register) == false)\
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
            stack_byte_pop_ptr(stack, IP->r1 + (IP->r2*256) + (IP->r3*65536), NULL);\
            break;\
        case POPA:\
            stack_byte_pop_ptr(stack, *addr_register, NULL);\
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
        case CALL:\
            if (stack_usize_push((stack_usize*)call_stack, IP - vm->inst_p + 1) == false)\
                return 1;\
            JUMP_TO_ADDR\
            break;\
\
        case LIBCALL:\
            ((ncvm_lib_function)vm->lib_functions[*addr_register])(thread);\
            break;\
    }\
    ++IP;


_export  u8 ncvm_execute_thread_step(ncvm_thread* thread) {
    /*u32* u32_registers = thread->u32_registers;
    u64* u64_registers = thread->u64_registers;
    f32* f32_registers = thread->f32_registers;
    f64* f64_registers = thread->f64_registers;
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
#ifdef __NCVM_USE_JUMPTABLE
#define INSTRUCTION_END continue;
#define INSTRUCTION_CASE 
#define CYCLE_END break;
#else
#define INSTRUCTION_END break;
#define INSTRUCTION_CASE case
#define CYCLE_END goto while_exit;
#endif

#define DEREF_ADDR unsigned_long_long_to_usize((*addr_register))

_export u8 ncvm_execute_thread(ncvm_thread* thread) {
    u32* u32_registers = thread->u32_registers;
    unsigned_long_long* u64_registers = thread->u64_registers;
    f32* f32_registers = thread->f32_registers;
    f64* f64_registers = thread->f64_registers;
    stack_byte* stack = (stack_byte*)thread->stack_p;
    stack_usize* call_stack = (stack_usize*)thread->call_stack_p;
    ncvm *vm = thread->vm;

    #ifdef __NCVM_USE_JUMPTABLE
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

    unsigned_long_long* const addr_register = &thread->u64_registers[0];
    register const Instruction* IP = thread->current_instr_p-1;

    while (true) {
        ++IP;
        /* printf("%d\n", IP->opcode); */
        #ifdef __NCVM_USE_JUMPTABLE
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
                            (usize *)addr_register) == false)
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
        /*u64_registers[IP->r1] = 0;*/
        unsigned_long_long_clear(u64_registers[IP->r1]);
        INSTRUCTION_END;
    INSTRUCTION_CASE FRCLR:
        f32_registers[IP->r1] = 0;
        INSTRUCTION_END;
    INSTRUCTION_CASE DRCLR:
        f64_registers[IP->r1] = 0;
        INSTRUCTION_END;

    INSTRUCTION_CASE ISR:
        *((u8 *)(u32_registers + IP->r1)) = IP->r2;
        *((u8 *)(u32_registers + IP->r1) + 1) = IP->r3;
        INSTRUCTION_END;
    INSTRUCTION_CASE LSR:
        *((u8 *)(u64_registers + IP->r1)) = IP->r2;
        *((u8 *)(u64_registers + IP->r1) + 1) = IP->r3;
        INSTRUCTION_END;

        /*ISRF:
            *((u8*)(u32_registers+IP->r1)) = IP->r2;
            *((u8*)(u32_registers+IP->r1)+1) = IP->r3;
            INSTRUCTION_END;
        ISRS:
            *((u8*)(u32_registers+IP->r1)+2) = IP->r2;
            *((u8*)(u32_registers+IP->r1)+3) = IP->r3;
            INSTRUCTION_END;
        LSRF:
            *((u8*)(u64_registers+IP->r1)) = IP->r2;
            *((u8*)(u64_registers+IP->r1)+1) = IP->r3;
            INSTRUCTION_END;
        LSRS:
            *((u8*)(u64_registers+IP->r1)+2) = IP->r2;
            *((u8*)(u64_registers+IP->r1)+3) = IP->r3;
            INSTRUCTION_END;
        LSRT:
            *((u8*)(u64_registers+IP->r1)+4) = IP->r2;
            *((u8*)(u64_registers+IP->r1)+5) = IP->r3;
            INSTRUCTION_END;
        LSRQ:
            *((u8*)(u64_registers+IP->r1)+6) = IP->r2;
            *((u8*)(u64_registers+IP->r1)+7) = IP->r3;
            INSTRUCTION_END;*/

    INSTRUCTION_CASE IRSI:
        u32_registers[IP->r1] = u32_registers[IP->r2] >> IP->r3;
        INSTRUCTION_END;
    INSTRUCTION_CASE ILSI:
        u32_registers[IP->r1] = u32_registers[IP->r2] << IP->r3;
        INSTRUCTION_END;
    INSTRUCTION_CASE LRSI:
        unsigned_long_long_rshift_to(u64_registers[IP->r1], u64_registers[IP->r1], IP->r3);
        INSTRUCTION_END;
    INSTRUCTION_CASE LLSI:
        unsigned_long_long_lshift_to(u64_registers[IP->r1], u64_registers[IP->r1], IP->r3);
        INSTRUCTION_END;

    INSTRUCTION_CASE IRSA:
        u32_registers[IP->r1] = u32_registers[IP->r2] >> DEREF_ADDR;
        INSTRUCTION_END;
    INSTRUCTION_CASE ILSA:
        u32_registers[IP->r1] = u32_registers[IP->r2] << DEREF_ADDR;
        INSTRUCTION_END;
    INSTRUCTION_CASE LRSA:
        unsigned_long_long_rshift_to(u64_registers[IP->r1], u64_registers[IP->r1], DEREF_ADDR);
        INSTRUCTION_END;
    INSTRUCTION_CASE LLSA:
        unsigned_long_long_lshift_to(u64_registers[IP->r1], u64_registers[IP->r1], DEREF_ADDR);
        INSTRUCTION_END;

    INSTRUCTION_CASE ISMLD:
        u32_registers[IP->r1] = 0;
        memcpy(&u32_registers[IP->r1], &vm->static_mem_p[DEREF_ADDR],
               IP->r2);
        INSTRUCTION_END;
    INSTRUCTION_CASE ISMST:
        memcpy(&vm->static_mem_p[DEREF_ADDR], &u32_registers[IP->r1],
               IP->r2);
        INSTRUCTION_END;

    INSTRUCTION_CASE LSMLD:
        /*u64_registers[IP->r1] = 0;*/
        unsigned_long_long_clear(u64_registers[IP->r1]);
        memcpy(&u64_registers[IP->r1], &vm->static_mem_p[DEREF_ADDR],
               IP->r2);
        INSTRUCTION_END;
    INSTRUCTION_CASE LSMST:
        memcpy(&vm->static_mem_p[DEREF_ADDR], &u64_registers[IP->r1],
               IP->r2);
        INSTRUCTION_END;

    INSTRUCTION_CASE FSMLD:
        memcpy(&f32_registers[IP->r1], &vm->static_mem_p[DEREF_ADDR], 4);
        INSTRUCTION_END;
    INSTRUCTION_CASE FSMST:
        memcpy(&vm->static_mem_p[DEREF_ADDR], &f32_registers[IP->r1], 4);
        INSTRUCTION_END;

    INSTRUCTION_CASE DSMLD:
        memcpy(&f64_registers[IP->r1], &vm->static_mem_p[DEREF_ADDR], 8);
        INSTRUCTION_END;
    INSTRUCTION_CASE DSMST:
        memcpy(&vm->static_mem_p[DEREF_ADDR], &f64_registers[IP->r1], 8);
        INSTRUCTION_END;

    INSTRUCTION_CASE POPI:
        stack_byte_pop_ptr(stack, IP->r1 + (IP->r2 * 256) + (IP->r3 * 65536),
                           NULL);
        INSTRUCTION_END;
    INSTRUCTION_CASE POPA:
        stack_byte_pop_ptr(stack, DEREF_ADDR, NULL);
        INSTRUCTION_END;

    INSTRUCTION_CASE IPUSH:
        stack_byte_push_ptr(stack, (u8 *)&u32_registers[IP->r1], IP->r2);
        INSTRUCTION_END;
    INSTRUCTION_CASE ISTLD:
        u32_registers[IP->r1] = 0;
        memcpy(&u32_registers[IP->r1], &stack->data[DEREF_ADDR], IP->r2);
        INSTRUCTION_END;
    INSTRUCTION_CASE ISTST:
        memcpy(&stack->data[DEREF_ADDR], &u32_registers[IP->r1], IP->r2);
        INSTRUCTION_END;

    INSTRUCTION_CASE LPUSH:
        stack_byte_push_ptr(stack, (u8 *)&u64_registers[IP->r1], IP->r2);
        INSTRUCTION_END;
    INSTRUCTION_CASE LSTLD:
        /*u64_registers[IP->r1] = 0;*/
        unsigned_long_long_clear(u64_registers[IP->r1]);
        memcpy(&u64_registers[IP->r1], &stack->data[DEREF_ADDR], IP->r2);
        INSTRUCTION_END;
    INSTRUCTION_CASE LSTST:
        memcpy(&stack->data[DEREF_ADDR], &u64_registers[IP->r1], IP->r2);
        INSTRUCTION_END;

    INSTRUCTION_CASE FPUSH:
        stack_byte_push_ptr(stack, (u8 *)&f32_registers[IP->r1], 4);
        INSTRUCTION_END;
    INSTRUCTION_CASE FSTLD:
        memcpy(&f32_registers[IP->r1], &stack->data[DEREF_ADDR], 4);
        INSTRUCTION_END;
    INSTRUCTION_CASE FSTST:
        memcpy(&stack->data[DEREF_ADDR], &f32_registers[IP->r1], 4);
        INSTRUCTION_END;

    INSTRUCTION_CASE DPUSH:
        stack_byte_push_ptr(stack, (u8 *)&f64_registers[IP->r1], 8);
        INSTRUCTION_END;
    INSTRUCTION_CASE DSTLD:
        memcpy(&f64_registers[IP->r1], &stack->data[DEREF_ADDR], 8);
        INSTRUCTION_END;
    INSTRUCTION_CASE DSTST:
        memcpy(&stack->data[DEREF_ADDR], &f64_registers[IP->r1], 8);
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
        unsigned_long_long_add_to(u64_registers[IP->r1], u64_registers[IP->r2], u64_registers[IP->r3]);
        INSTRUCTION_END;
    INSTRUCTION_CASE LSUB:
        unsigned_long_long_sub_to(u64_registers[IP->r1], u64_registers[IP->r2], u64_registers[IP->r3]);
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
        /*++u64_registers[IP->r1];*/
        unsigned_long_long_inc(u64_registers[IP->r1]);
        INSTRUCTION_END;
    INSTRUCTION_CASE LDEC:
        /*--u64_registers[IP->r1];*/
        unsigned_long_long_dec(u64_registers[IP->r1]);
        INSTRUCTION_END;
    INSTRUCTION_CASE LNEG:
        unsigned_long_long_neg_to(u64_registers[IP->r1], u64_registers[IP->r2]);
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
        u32_registers[IP->r1] = (u32)unsigned_long_long_to_usize(u64_registers[IP->r2]);
        INSTRUCTION_END;
    INSTRUCTION_CASE FTOI:
        u32_registers[IP->r1] = (u32)f32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE DTOI:
        u32_registers[IP->r1] = (u32)f64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE ITOL:
        u64_registers[IP->r1] = (u64)u32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE FTOL:
        u64_registers[IP->r1] = (u64)f32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE DTOL:
        u64_registers[IP->r1] = (u64)f64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE ITOF:
        f32_registers[IP->r1] = (f32)u32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE LTOF:
        f32_registers[IP->r1] = (f32)u64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE DTOF:
        f32_registers[IP->r1] = (f32)f64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE ITOD:
        f64_registers[IP->r1] = (f64)u32_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE LTOD:
        f64_registers[IP->r1] = (f64)u64_registers[IP->r2];
        INSTRUCTION_END;
    INSTRUCTION_CASE FTOD:
        f64_registers[IP->r1] = (f64)f32_registers[IP->r2];
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
        if (unsigned_long_long_is_zero(u64_registers[IP->r1])) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJNZ:
        if (unsigned_long_long_is_not_zero(u64_registers[IP->r1])) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJEQ:
        if (unsigned_long_long_is_equal(u64_registers[IP->r1], u64_registers[IP->r2])) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJNQ:
        if (unsigned_long_long_is_not_equal(u64_registers[IP->r1], u64_registers[IP->r2])) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJML:
        if (unsigned_long_long_is_less(u64_registers[IP->r1], u64_registers[IP->r2])) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJEL:
        if (unsigned_long_long_is_less_or_equal(u64_registers[IP->r1], u64_registers[IP->r2])) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJMG:
        if (unsigned_long_long_is_greater(u64_registers[IP->r1], u64_registers[IP->r2])) {
            JUMP_TO_ADDR
        }
        INSTRUCTION_END;
    INSTRUCTION_CASE LJEG:
        if (unsigned_long_long_is_greater_or_equal(u64_registers[IP->r1], u64_registers[IP->r2])) {
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
            false)
            return 1;
        JUMP_TO_ADDR
        INSTRUCTION_END;

    INSTRUCTION_CASE LIBCALL:
        ((ncvm_lib_function)vm->lib_functions[DEREF_ADDR])(thread);
        INSTRUCTION_END;
        /* ++IP; */
        #ifndef __NCVM_USE_JUMPTABLE
        }
        #endif
    }
        
    #ifndef __NCVM_USE_JUMPTABLE
    while_exit:;
    #endif
    thread->current_instr_p = IP;
    return 0;
}

