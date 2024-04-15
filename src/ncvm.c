#include <ncvm.h>
#include <stdlib.h>
#include <string.h>
#include <extc_stack.h>

#include <stdio.h>

#include "util.h"

stack_template_def(byte, u8)
stack_template_impl(byte, u8)

_export ncvm ncvm_initArr(
    Instruction* inst_p, usize inst_count,
    u8* static_mem_p, usize static_mem_size
) {
    ncvm ret;
    ret.inst_p = inst_p;
    ret.inst_count = inst_count;
    ret.static_mem_p = static_mem_p;
    ret.static_mem_size = static_mem_size;

    return ret;
}

_export ncvm ncvm_initData(const char* data_p, usize data_size) {
    ncvm ret;
    /* TODO: Add body */
    return ret;
}

_export void ncvm_free(ncvm* vm) {
    /*free(vm->inst_p);*/
}


_export u8 ncvm_execute(ncvm* vm, ThreadSettings settings) {
    /* Create main thread */
    return ncvm_create_thread(vm, vm->inst_p, NULL, 0, settings);
}

_export u8 ncvm_create_thread(
    ncvm* vm, Instruction* si_p, u8* EST, usize EST_size,
    ThreadSettings settings
) {
    /* Check is little indian*/
    if (!is_little_endian())
        return 2;

    /* Create stack, registers */
    u8 st_r;
    stack_byte stack = stack_byte_init(settings.stack_size, &st_r);
    if (st_r != 0)
        return 1;
        
    /* Add extern stack to thread stack */
    if (EST != NULL)
        stack_byte_push_ptr(&stack, EST, EST_size);


    /* Alloc registers */
    u32* u32_registers = (u32*)malloc(sizeof(u32) * settings.u32_reg_size);
    u64* u64_registers = (u64*)malloc(sizeof(u64) * settings.u64_reg_size+1);
    f32* f32_registers = (f32*)malloc(sizeof(f32) * settings.f32_reg_size);
    f64* f64_registers = (f64*)malloc(sizeof(f64) * settings.f64_reg_size);
    
    /*u64[0] - addr register*/
    u64* const addr_register = &u64_registers[0];

    /* Clear registers */
    memset(u32_registers, 0, sizeof(u32) * settings.u32_reg_size);
    memset(u64_registers, 0, sizeof(u64) * settings.u64_reg_size);
    memset(f32_registers, 0, sizeof(f32) * settings.f32_reg_size);
    memset(f64_registers, 0, sizeof(f64) * settings.f64_reg_size);

    register Instruction* IP = si_p;

    while (true) {
        switch (IP->opcode) {
            case STOP:
                /* TODO: add regisetr */
                goto while_exit;
                break;
            case RET:
                break;

            case IMOV:
                u32_registers[IP->r1] = u32_registers[IP->r2];
                break;
            case LMOV:
                u64_registers[IP->r1] = u64_registers[IP->r2];
                break;
            case FMOV:
                f32_registers[IP->r1] = f32_registers[IP->r2];
                break;
            case DMOV:
                f64_registers[IP->r1] = f64_registers[IP->r2];
                break;

            case IRCLR:
                u32_registers[IP->r1] = 0;
                break;
            case LRCLR:
                u64_registers[IP->r1] = 0;
                break;
            case FRCLR:
                f32_registers[IP->r1] = 0;
                break;
            case DRCLR:
                f64_registers[IP->r1] = 0;
                break;            

            case ISR:
                *((u8*)(u32_registers+IP->r1)) = IP->r2;
                *((u8*)(u32_registers+IP->r1)+1) = IP->r3;
                break;
            case LSR:
                *((u8*)(u64_registers+IP->r1)) = IP->r2;
                *((u8*)(u64_registers+IP->r1)+1) = IP->r3;
                break;


            case ISMLD:
                u32_registers[IP->r1] = 0;
                memcpy(&u32_registers[IP->r1], &vm->static_mem_p[*addr_register], IP->r2);
                break;
            case ISMST:
                memcpy(&vm->static_mem_p[*addr_register], &u32_registers[IP->r1], IP->r2);
                break;

            case LSMLD:
                u64_registers[IP->r1] = 0;
                memcpy(&u64_registers[IP->r1], &vm->static_mem_p[*addr_register], IP->r2);
                break;
            case LSMST:
                memcpy(&vm->static_mem_p[*addr_register], &u64_registers[IP->r1], IP->r2);
                break;

            case FSMLD:
                memcpy(&f32_registers[IP->r1], &vm->static_mem_p[*addr_register], 4);
                break;
            case FSMST:
                memcpy(&vm->static_mem_p[*addr_register], &f32_registers[IP->r1], 4);
                break;

            case DSMLD:
                memcpy(&f64_registers[IP->r1], &vm->static_mem_p[*addr_register], 8);
                break;
            case DSMST:
                memcpy(&vm->static_mem_p[*addr_register], &f64_registers[IP->r1], 8);
                break;


            case POPI:
                stack_byte_pop_ptr(&stack, IP->r1 + (IP->r2*256) + (IP->r3*65536));
                break;
            case POPA:
                stack_byte_pop_ptr(&stack, *addr_register);
                break;
            
            case IPUSH:
                stack_byte_push_ptr(&stack, (u8*)&u32_registers[IP->r1], IP->r2);
                break;
            case ISTLD:
                u32_registers[IP->r1] = 0;
                memcpy(&u32_registers[IP->r1], &stack.data[*addr_register], IP->r2);
                break;
            case ISTST:
                memcpy(&stack.data[*addr_register], &u32_registers[IP->r1], IP->r2);
                break;

            case LPUSH:
                stack_byte_push_ptr(&stack, (u8*)&u64_registers[IP->r1], IP->r2);
                break;
            case LSTLD:
                u64_registers[IP->r1] = 0;
                memcpy(&u64_registers[IP->r1], &stack.data[*addr_register], IP->r2);
                break;
            case LSTST:
                memcpy(&stack.data[*addr_register], &u64_registers[IP->r1], IP->r2);
                break;

            case FPUSH:
                stack_byte_push_ptr(&stack, (u8*)&f32_registers[IP->r1], 4);
                break;
            case FSTLD:
                memcpy(&f32_registers[IP->r1], &stack.data[*addr_register], 4);
                break;
            case FSTST:
                memcpy(&stack.data[*addr_register], &f32_registers[IP->r1], 4);
                break;

            case DPUSH:
                stack_byte_push_ptr(&stack, (u8*)&f64_registers[IP->r1], 8);
                break;
            case DSTLD:
                memcpy(&f64_registers[IP->r1], &stack.data[*addr_register], 8);
                break;
            case DSTST:
                memcpy(&stack.data[*addr_register], &f64_registers[IP->r1], 8);
                break;


            /*
                HEAP
            */


            case IADD:
                u32_registers[IP->r1] = u32_registers[IP->r2] + u32_registers[IP->r3];
                break;
            case ISUB:
                u32_registers[IP->r1] = u32_registers[IP->r2] - u32_registers[IP->r3];
                break;
            case IMULT:
                u32_registers[IP->r1] = u32_registers[IP->r2] * u32_registers[IP->r3];
                break;
            case IDIV:
                u32_registers[IP->r1] = u32_registers[IP->r2] / u32_registers[IP->r3];
                break;
            case IMOD:
                u32_registers[IP->r1] = u32_registers[IP->r2] % u32_registers[IP->r3];
                break;
            case IINC:
                ++u32_registers[IP->r1];
                break;
            case IDEC:
                --u32_registers[IP->r1];
                break;

            case LADD:
                u64_registers[IP->r1] = u64_registers[IP->r2] + u64_registers[IP->r3];
                break;
            case LSUB:
                u64_registers[IP->r1] = u64_registers[IP->r2] - u64_registers[IP->r3];
                break;
            case LMULT:
                u64_registers[IP->r1] = u64_registers[IP->r2] * u64_registers[IP->r3];
                break;
            case LDIV:
                u64_registers[IP->r1] = u64_registers[IP->r2] / u64_registers[IP->r3];
                break;
            case LMOD:
                u64_registers[IP->r1] = u64_registers[IP->r2] % u64_registers[IP->r3];
                break;
            case LINC:
                ++u64_registers[IP->r1];
                break;
            case LDEC:
                --u64_registers[IP->r1];
                break;

            case FADD:
                f32_registers[IP->r1] = f32_registers[IP->r2] + f32_registers[IP->r3];
                break;
            case FSUB:
                f32_registers[IP->r1] = f32_registers[IP->r2] - f32_registers[IP->r3];
                break;
            case FMULT:
                f32_registers[IP->r1] = f32_registers[IP->r2] * f32_registers[IP->r3];
                break;
            case FDIV:
                f32_registers[IP->r1] = f32_registers[IP->r2] / f32_registers[IP->r3];
                break;
            case FINC:
                ++f32_registers[IP->r1];
                break;
            case FDEC:
                --f32_registers[IP->r1];
                break;

            case DADD:
                f64_registers[IP->r1] = f64_registers[IP->r2] + f64_registers[IP->r3];
                break;
            case DSUB:
                f64_registers[IP->r1] = f64_registers[IP->r2] - f64_registers[IP->r3];
                break;
            case DMULT:
                f64_registers[IP->r1] = f64_registers[IP->r2] * f64_registers[IP->r3];
                break;
            case DDIV:
                f64_registers[IP->r1] = f64_registers[IP->r2] / f64_registers[IP->r3];
                break;
            case DINC:
                ++f64_registers[IP->r1];
                break;
            case DDEC:
                --f64_registers[IP->r1];
                break;

            case LTOI:
                u32_registers[IP->r1] = (u32)u64_registers[IP->r2];
                break;
            case FTOI:
                u32_registers[IP->r1] = (u32)f32_registers[IP->r2];
                break;
            case DTOI:
                u32_registers[IP->r1] = (u32)f64_registers[IP->r2];
                break;
            case ITOL:
                u64_registers[IP->r1] = (u64)u32_registers[IP->r2];
                break;
            case FTOL:
                u64_registers[IP->r1] = (u64)f32_registers[IP->r2];
                break;
            case DTOL:
                u64_registers[IP->r1] = (u64)f64_registers[IP->r2];
                break;
            case ITOF:
                f32_registers[IP->r1] = (f32)u32_registers[IP->r2];
                break;
            case LTOF:
                f32_registers[IP->r1] = (f32)u64_registers[IP->r2];
                break;
            case DTOF:
                f32_registers[IP->r1] = (f32)f64_registers[IP->r2];
                break;
            case ITOD:
                f64_registers[IP->r1] = (f64)u32_registers[IP->r2];
                break;
            case LTOD:
                f64_registers[IP->r1] = (f64)u64_registers[IP->r2];
                break;
            case FTOD:
                f64_registers[IP->r1] = (f64)f32_registers[IP->r2];
                break;
            default:
                break;
        }
        ++IP;
    }
    while_exit:;
    printf("I: %u\n", u32_registers[0]);
    printf("L: %lld\n", u64_registers[1]);    
    printf("F: %f\n", f32_registers[0]);
    printf("D: %lf\n", f64_registers[0]);

    free(u32_registers);
    free(u64_registers);
    free(f32_registers);
    free(f64_registers);


    stack_byte_free(&stack);
    return 0;
}

