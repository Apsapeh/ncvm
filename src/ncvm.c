#include <ncvm.h>
#include <stdlib.h>
#include <string.h>
#include <extc_stack.h>
/*#include <stdio.h>*/

#define ANY_REGISTER_COUNT 8
#define U32_REGISTER_COUNT ANY_REGISTER_COUNT
#define U64_REGISTER_COUNT ANY_REGISTER_COUNT
#define F32_REGISTER_COUNT ANY_REGISTER_COUNT
#define F64_REGISTER_COUNT ANY_REGISTER_COUNT


stack_template_def(byte, u8)
stack_template_impl(byte, u8)

_export ncvm ncvm_initArr(
    Instruction* inst_p, usize inst_count,
    u8* static_mem_p, usize static_mem_size
) {
    ncvm ret;
   /*ret.inst_p = (Instruction*)malloc(sizeof(Instruction) * inst_count);
    memcpy(ret.inst_p, inst_p, sizeof(Instruction) * inst_count);
    ret.inst_count = inst_count;*/
    ret.inst_p = inst_p;
    ret.inst_count = inst_count;
    ret.static_mem_p = static_mem_p;
    ret.static_mem_size = static_mem_size;

    Instruction a = {.opcode = DDIV, .r1 = 0, .r2 = 1, .r3.valf=3};

    return ret;
}

_export ncvm ncvm_initData(const char* data_p, usize data_size) {
    ncvm ret;
    /* TODO: Add body */
    return ret;
}

_export void ncvm_free(ncvm* vm) {
    free(vm->inst_p);
}


_export u8 ncvm_execute(ncvm* vm) {
    /* Create main thread */
    return ncvm_create_thread(vm, vm->inst_p, NULL, 0);
}

_export u8 ncvm_create_thread(ncvm* vm, Instruction* si_p, u8* EST, usize EST_size) {
    /* Create stack, registers */
    u8 st_r;
    stack_byte stack = stack_byte_init(1024*1024*1, &st_r);
    if (st_r != 0)
        return 1;
        
    // Add extern stack to thread stack
    if (EST != NULL)
        stack_byte_push_ptr(&stack, EST, EST_size);

    u32 u32_registers[U32_REGISTER_COUNT] = {0};
    u64 u64_registers[U64_REGISTER_COUNT] = {0};
    f32 f32_registers[F32_REGISTER_COUNT] = {0};
    f64 f64_registers[F64_REGISTER_COUNT] = {0};

    register Instruction* IP = si_p;

    while (true) {
        switch (IP->opcode) {
            case STOP:
                /* TODO: add regisetr */
                goto while_exit;
                break;
            case RET:
                break;
                
            case IPUSH:
                stack_byte_push_ptr(&stack, (u8*)&u32_registers[0], 1);
                break;
            case IPOP:
                break;
            case ISTLD:
                break;
            case ISTST:
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
            /*case SMLD:
                u64_registers[IP->r1] = 0;
                memcpy(&u64_registers[IP->r1], &vm->static_mem_p[IP->r2], IP->r3.vali);
                break;
            case SMST:
                memcpy(&vm->static_mem_p[IP->r1], &u64_registers[IP->r2], IP->r3.vali);
                break;*/
                
            case IADD:
                u32_registers[IP->r1] = u32_registers[IP->r2] + u32_registers[IP->r3.vali];
                break;
            case ISUB:
                u32_registers[IP->r1] = u32_registers[IP->r2] - u32_registers[IP->r3.vali];
                break;
            case IMULT:
                u32_registers[IP->r1] = u32_registers[IP->r2] * u32_registers[IP->r3.vali];
                break;
            case IDIV:
                u32_registers[IP->r1] = u32_registers[IP->r2] / u32_registers[IP->r3.vali];
                break;
            case IMOD:
                u32_registers[IP->r1] = u32_registers[IP->r2] % u32_registers[IP->r3.vali];
                break;
            case IINC:
                ++u32_registers[IP->r1];
                break;
            case IDEC:
                --u32_registers[IP->r1];
                break;

            case LADD:
                u64_registers[IP->r1] = u64_registers[IP->r2] + u64_registers[IP->r3.vali];
                break;
            case LSUB:
                u64_registers[IP->r1] = u64_registers[IP->r2] - u64_registers[IP->r3.vali];
                break;
            case LMULT:
                u64_registers[IP->r1] = u64_registers[IP->r2] * u64_registers[IP->r3.vali];
                break;
            case LDIV:
                u64_registers[IP->r1] = u64_registers[IP->r2] / u64_registers[IP->r3.vali];
                break;
            case LMOD:
                u64_registers[IP->r1] = u64_registers[IP->r2] % u64_registers[IP->r3.vali];
                break;
            case LINC:
                ++u64_registers[IP->r1];
                break;
            case LDEC:
                --u64_registers[IP->r1];
                break;

            case FADD:
                f32_registers[IP->r1] = f32_registers[IP->r2] + f32_registers[IP->r3.vali];
                break;
            case FSUB:
                f32_registers[IP->r1] = f32_registers[IP->r2] - f32_registers[IP->r3.vali];
                break;
            case FMULT:
                f32_registers[IP->r1] = f32_registers[IP->r2] * f32_registers[IP->r3.vali];
                break;
            case FDIV:
                f32_registers[IP->r1] = f32_registers[IP->r2] / f32_registers[IP->r3.vali];
                break;
            case FINC:
                ++f32_registers[IP->r1];
                break;
            case FDEC:
                --f32_registers[IP->r1];
                break;

            case DADD:
                f64_registers[IP->r1] = f64_registers[IP->r2] + f64_registers[IP->r3.vali];
                break;
            case DSUB:
                f64_registers[IP->r1] = f64_registers[IP->r2] - f64_registers[IP->r3.vali];
                break;
            case DMULT:
                f64_registers[IP->r1] = f64_registers[IP->r2] * f64_registers[IP->r3.vali];
                break;
            case DDIV:
                f64_registers[IP->r1] = f64_registers[IP->r2] / f64_registers[IP->r3.vali];
                break;
            case DINC:
                ++f64_registers[IP->r1];
                break;
            case DDEC:
                --f64_registers[IP->r1];
                break;

            case FTOI:
                u32_registers[IP->r1] = (u32)f32_registers[IP->r2];
                break;
            case ITOF:
                f32_registers[IP->r1] = (f32)u32_registers[IP->r2];
                break;
            case DTOI:
                u32_registers[IP->r1] = (u32)f64_registers[IP->r2];
                break;
            case ITOD:
                f64_registers[IP->r1] = (f64)u32_registers[IP->r2];
                break;
            case FTOL:
                u64_registers[IP->r1] = (u64)f32_registers[IP->r2];
                break;
            case LTOF:
                f32_registers[IP->r1] = (f32)u64_registers[IP->r2];
                break;
            case DTOL:
                u64_registers[IP->r1] = (u64)f64_registers[IP->r2];
                break;
            case LTOD:
                f64_registers[IP->r1] = (f64)u64_registers[IP->r2];
                break;
            case FTOD:
                f64_registers[IP->r1] = (f64)f32_registers[IP->r2];
                break;
            case DTOF:
                f32_registers[IP->r1] = (f32)f64_registers[IP->r2];
                break;
            default:
                break;
        }
        ++IP;
    }
    while_exit:;
    /*printf("I: %d\n", u32_registers[0]);
    printf("L: %lld\n", u64_registers[0]);    
    printf("F: %f\n", f32_registers[0]);
    printf("D: %lf\n", f64_registers[0]);*/


    stack_byte_free(&stack);
    return 0;
}

