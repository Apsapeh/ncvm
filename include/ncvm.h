#ifndef __NCVM
#define __NCVM

#if (defined(__GNUC__) /*&& defined(__clang__)*/)
    #define _packed __attribute__((__packed__))
    #define _export __attribute__((visibility("default")))
#elif (defined(_MSC_VER))
    #define _packed 
    #ifdef __NCVM_DYN_LIB_EXPORT
        #define _export __declspec(dllexport)
    #else
        #define _export __declspec(dllimport)
    #endif
#else
    #define _packed
    #define _export
#endif



enum _packed OPCODE {
    /*==>  Other  <==*/
    NOP,    /*_|_|_,_*/
    STOP,   /*_|_|_|_*/
    RET,    /*_|_|_|_*/


    /*==> Mem Func <==*/  
    IMOV,   /*ird|ir1|_|ir1->ird*/
    LMOV,   /*lrd|lr1|_|lr1->lrd*/
    FMOV,   /*frd|fr1|_|fr1->frd*/
    DMOV,   /*drd|dr1|_|dr1->drd*/

    IRCLR,  /*ir1|_|_|Clear ir1*/
    LRCLR,  /*lr1|_|_|Clear lr1*/
    FRCLR,  /*fr1|_|_|Clear fr1*/
    DRCLR,  /*dr1|_|_|Clear dr1*/

    ISR,    /*ird|0..7|8..15|Set first 16 bits to ird*/
    LSR,    /*lrd|0..7|8..15|Set first 16 bits to lrd*/

    /*==> Static Mem Func <==*/
    /* 32-bit int commands */    
    ISMLD,  /*ird|v|_|Load 'v' count of bytes from static memory to ird. ird = ST_MEM[ar..ar+v]*/
    ISMST,  /*ir1|v|_|Set 'v' count of bytes from ir1 to static memory. ST_MEM[ar..ar+v] = ir1[0..v]*/

    /* 64-bit int commands */        
    LSMLD,  /*lrd|v|_|Load 'v' count of bytes from static memory to lrd. lrd = ST_MEM[ar..ar+v]*/
    LSMST,  /*lr1|v|_|Set 'v' count of bytes from lr1 to static memory. ST_MEM[ar..ar+v] = lr1[0..v]*/
    
    /* 32-bit float commands */        
    FSMLD,  /*frd|_|_|Load 4 bytes from static memory to frd. frd = ST_MEM[ar..ar+4]*/
    FSMST,  /*fr1|_|_|Set 4 bytes from fr1 to static memory. ST_MEM[ar..ar+4] = fr1*/
    
    /* 64-bit float commands*/ 
    DSMLD,  /*drd|_|_|Load 8 bytes from static memory to drd. drd = ST_MEM[ar..ar+8]*/
    DSMST,  /*dr1|_|_|Set 8 bytes from dr1 to static memory. ST_MEM[ar..ar+8] = dr1*/ 


    /*==>  Stack  <==*/
    POPI,   /*0..7|8..15|16..23|Pop from stack (0..23) bytes*/
    POPA,   /*_|_|_|Pop from stack ar bytes*/
    
    IPUSH,  /*ir1|v|_|Push to stack 'v' count of bytes from ir1*/
    ISTLD,  /*ird|v|_|Load 'v' count of bytes from stack to ird. ird = STACK[ar..ar+v]*/
    ISTST,  /*ir1|v|_|Set 'v' count of bytes from ir1 to stack. STACK[ar..ar+v] = ir1[0..v]*/
    
    LPUSH,  /*lr1|v|_|Push to stack 'v' count of bytes from lr1*/
    LSTLD,  /*lrd|v|_|Load 'v' count of bytes from stack to lrd. lrd = STACK[ar..ar+v]*/
    LSTST,  /*lr1|v|_|Set 'v' count of bytes from lr1 to stack. STACK[ar..ar+v] = lr1[0..v]*/
    
    FPUSH,  /*fr1|_|_|Push to stack 4 bytes from fr1*/
    FSTLD,  /*frd|_|_|Load 4 bytes from stack to frd. frd = STACK[ar..ar+4]*/
    FSTST,  /*fr1|_|_|Set 4 bytes from fr1 to stack. STACK[ar..ar+4] = fr1[0..4]*/
    
    DPUSH,  /*dr1|_|_|Push to stack 8 bytes from dr1*/
    DSTLD,  /*drd|_|_|Load 8 bytes from stack to drd. drd = STACK[ar..ar+8]*/
    DSTST,  /*dr1|_|_|Set 8 bytes from dr1 to stack. STACK[ar..ar+8] = dr1[0..8]*/


    /*==>  Heap  <==*/
    ALLOC,  /*_|_|_|!_*/
    FREE,   /*_|_|_|!_*/
    HELD,   /*_|_|_|!_*/
    HEST,   /*_|_|_|!_*/


    /*==>  Math  <==*/
    /* 32-bit int commands */
    IADD,   /*ird|ir1|ir2|ird = ir1 + ir2*/
    ISUB,   /*ird|ir1|ir2|ird = ir1 - ir2*/
    IMULT,  /*ird|ir1|ir2|ird = ir1 * ir2*/
    IDIV,   /*ird|ir1|ir2|ird = ir1 / ir2*/
    IMOD,   /*ird|ir1|ir2|ird = ir1 % ir2*/
    IINC,   /*ird|_|_|ird += 1*/
    IDEC,   /*ird|_|_|ird += 1*/

    /* 64-bit int commands */    
    LADD,   /*lrd|lr1|lr2|lrd = lr1 + lr2*/
    LSUB,   /*lrd|lr1|lr2|lrd = lr1 - lr2*/
    LMULT,  /*lrd|lr1|lr2|lrd = lr1 * lr2*/
    LDIV,   /*lrd|lr1|lr2|lrd = lr1 / lr2*/
    LMOD,   /*lrd|lr1|lr2|lrd = lr1 % lr2*/
    LINC,   /*lrd|_|_|lrd += 1*/
    LDEC,   /*lrd|_|_|lrd += 1*/
    
    /* 32-bit float commands */    
    FADD,   /*frd|fr1|fr2|frd = fr1 + fr2*/
    FSUB,   /*frd|fr1|fr2|frd = fr1 - fr2*/
    FMULT,  /*frd|fr1|fr2|frd = fr1 * fr2*/
    FDIV,   /*frd|fr1|fr2|frd = fr1 / fr2*/
    FINC,   /*frd|_|_|frd += 1*/
    FDEC,   /*frd|_|_|frd += 1*/

    /* 64-bit float commands */    
    DADD,   /*drd|dr1|dr2|drd = dr1 + dr2*/
    DSUB,   /*drd|dr1|dr2|drd = dr1 - dr2*/
    DMULT,  /*drd|dr1|dr2|drd = dr1 * dr2*/
    DDIV,   /*drd|dr1|dr2|drd = dr1 / dr2*/
    DINC,   /*drd|_|_|drd += 1*/
    DDEC,   /*drd|_|_|drd += 1*/


    /*==> Move between u32, u64, f32, f64 registers <==*/
    LTOI,   /*ird|lr1|_|ird = lr1*/
    FTOI,   /*ird|fr1|_|ird = fr1*/
    DTOI,   /*ird|dr1|_|ird = dr1*/
    ITOL,   /*lrd|ir1|_|lrd = ir1*/
    FTOL,   /*lrd|fr1|_|lrd = fr1*/
    DTOL,   /*lrd|dr1|_|lrd = dr1*/
    ITOF,   /*frd|ir1|_|frd = ir1*/
    LTOF,   /*frd|lr1|_|frd = lr1*/
    DTOF,   /*frd|dr1|_|frd = dr1*/
    ITOD,   /*drd|ir1|_|drd = ir1*/
    LTOD,   /*drd|lr1|_|drd = lr1*/
    FTOD,   /*drd|fr1|_|drd = fr1*/


    /*==>  Logic  <==*/


    /*==>  Jump  <==*/
    JMP,    /*_|_|_|_*/
    /* 32-bit int commands */    
    IJEZ,   /*_|_|_|_*/
    IJNZ,   /*_|_|_|_*/
    IJEQ,   /*_|_|_|_*/
    IJNQ,   /*_|_|_|_*/
    IJML,   /*_|_|_|_*/
    IJEL,   /*_|_|_|_*/
    IJMG,   /*_|_|_|_*/
    IJEG,   /*_|_|_|_*/
    
    /* 64-bit int commands */        
    LJEZ,   /*_|_|_|_*/
    LJNZ,   /*_|_|_|_*/
    LJEQ,   /*_|_|_|_*/
    LJNQ,   /*_|_|_|_*/
    LJML,   /*_|_|_|_*/
    LJEL,   /*_|_|_|_*/
    LJMG,   /*_|_|_|_*/
    LJEG,   /*_|_|_|_*/
    
    /* 32-bit float commands */        
    FJEZ,   /*_|_|_|_*/
    FJNZ,   /*_|_|_|_*/
    FJEQ,   /*_|_|_|_*/
    FJNQ,   /*_|_|_|_*/
    FJML,   /*_|_|_|_*/
    FJEL,   /*_|_|_|_*/
    FJMG,   /*_|_|_|_*/
    FJEG,   /*_|_|_|_*/
    
    /* 64-bit float commands */        
    DJEZ,   /*_|_|_|_*/
    DJNZ,   /*_|_|_|_*/
    DJEQ,   /*_|_|_|_*/
    DJNQ,   /*_|_|_|_*/
    DJML,   /*_|_|_|_*/
    DJEL,   /*_|_|_|_*/
    DJMG,   /*_|_|_|_*/
    DJEG,   /*_|_|_|_*/


    /*==>  Lib  <==*/
    L_CALL  /*_|_|_|_*/
};



enum _packed REGISTER {
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7
};

typedef struct {
    enum OPCODE opcode;
    unsigned char r1;
    unsigned char r2;
    unsigned char r3;
} Instruction;

#define Instruction_Int(OP, R1, R2, R3) {OP, R1, R2, R3} 
#define Instruction_Float(OP, R1, R2, R3) {OP, R1, R2, R3} 

typedef struct {
    Instruction*   inst_p;
    unsigned long  inst_count;
    unsigned char* static_mem_p;
    unsigned long  static_mem_size; /* Size in bytes */
} ncvm;

typedef struct {
    ncvm* vm;
    Instruction* current_instr_p;
    unsigned char* extern_stack_p;
    unsigned long extern_stack_size;
    void* stack_p;
    unsigned int* u32_registers;
    unsigned long* u64_registers;
    float* f32_registers;
    double* f64_registers;
} ncvm_thread;

typedef struct {
    unsigned long u32_reg_size;
    unsigned long u64_reg_size;
    unsigned long f32_reg_size;
    unsigned long f64_reg_size;
    unsigned long stack_size;   /* In bytes */
} ThreadSettings;

#define DefaultThreadSettings {\
    .u32_reg_size=8, .u64_reg_size=8,\
    .f32_reg_size=8, .f64_reg_size=8,\
    .stack_size=1024*1024*1\
}



/* ======================> Functions (C and CPP) <====================== */

/* For cpp support */
#ifdef __cplusplus
extern "C" {
#endif

_export ncvm ncvm_initArr(
    Instruction* inst_p, unsigned long inst_count,
    unsigned char* static_mem_p, unsigned long static_mem_size
);
_export ncvm ncvm_initData(const char* data_p, unsigned long data_size);
_export void ncvm_free(ncvm* vm);

/**
    @param vm VM
*/
_export unsigned char ncvm_execute(ncvm* vm, ThreadSettings settings);
_export unsigned char ncvm_create_thread(
    ncvm* vm, Instruction* start_instr_p, 
    unsigned char* ext_stack_p, unsigned long ext_stack_s,
    ThreadSettings settings
);


/* For cpp support */
#ifdef __cplusplus
}
#endif

#endif