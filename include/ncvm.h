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
    NOP,
    STOP,
    RET,

    /*==>  Stack  <==*/
    IPUSH,
    IPOP,
    ISTLD,
    ISTST,
    
    LPUSH,
    LPOP,
    LSTLD,
    LSTST,
    
    FPUSH,
    FPOP,
    FSTLD,
    FSTST,
    
    DPUSH,
    DPOP,
    DSTLD,
    DSTST,
    
    /*==>  Heap  <==*/
    ALLOC,
    FREE,
    HELD,
    HEST,

    /*==> Mem Func <==*/  
    IMOV,
    LMOV,    /*dst(r), from(r)*/
    FMOV,
    DMOV,

    ISR,     /*dst(r), _, val*/
    LSR,     /*dst(r), _, val*/
    FSR,     /*dst(r), _, val*/
    DSR,     /*dst(r), _, val*/

    /*==> Static Mem Func <==*/
    /* 32-bit int commands */    
    ISMLD,   /*dst(r), from(st), val*/
    ISMST,   /*dst(st), from(r), val*/

    /* 64-bit int commands */        
    LSMLD,   /*dst(r), from(st), val*/
    LSMST,   /*dst(st), from(r), val*/
    
    /* 32-bit float commands */        
    FSMLD,   /*dst(r), from(st), val*/
    FSMST,   /*dst(st), from(r), val*/
    
    /* 64-bit float commands*/ 
    DSMLD,   /*dst(r), from(st), val*/
    DSMST,   /*dst(st), from(r), val*/   
    

    /*==>  Math  <==*/
    /* 32-bit int commands */
    IADD,    /*dst(r), val(r), val(r)*/
    ISUB,    /*dst(r), val(r), val(r)*/
    IMULT,   /*dst(r), val(r), val(r)*/
    IDIV,    /*dst(r), val(r), val(r)*/
    IMOD,    /*dst(r), val(r), val(r)*/
    IINC,    /*dst(r)*/
    IDEC,    /*dst(r)*/

    /* 64-bit int commands */    
    LADD,    /*dst(r), val(r), val(r)*/
    LSUB,    /*dst(r), val(r), val(r)*/
    LMULT,   /*dst(r), val(r), val(r)*/
    LDIV,    /*dst(r), val(r), val(r)*/
    LMOD,    /*dst(r), val(r), val(r)*/
    LINC,    /*dst(r)*/
    LDEC,    /*dst(r)*/
    
    /* 32-bit float commands */    
    FADD,
    FSUB,
    FMULT,
    FDIV,
    FINC,
    FDEC,

    /* 64-bit float commands */    
    DADD,
    DSUB,
    DMULT,
    DDIV,
    DINC,
    DDEC,

    /*==> Move between u32, u64, f32, f64 registers <==*/
    FTOI,
    ITOF,
    DTOI,
    ITOD,
    FTOL,
    LTOF,
    DTOL,
    LTOD,
    FTOD,
    DTOF,


    /*==>  Jump  <==*/
    JMP,
    /* 32-bit int commands */    
    IJEZ,
    IJNZ,
    IJEQ,
    IJNQ,
    IJML,
    IJEL,
    IJMG,
    IJEG,
    
    /* 64-bit int commands */        
    LJEZ,
    LJNZ,
    LJEQ,
    LJNQ,
    LJML,
    LJEL,
    LJMG,
    LJEG,
    
    /* 32-bit float commands */        
    FJEZ,
    FJNZ,
    FJEQ,
    FJNQ,
    FJML,
    FJEL,
    FJMG,
    FJEG,
    
    /* 64-bit float commands */        
    DJEZ,
    DJNZ,
    DJEQ,
    DJNQ,
    DJML,
    DJEL,
    DJMG,
    DJEG,

    /*==>  Logic  <==*/


    /*==>  Lib  <==*/
    L_CALL
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
    union LongOrDouble{
        unsigned long long vali;
        double valf;
    } r3;
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