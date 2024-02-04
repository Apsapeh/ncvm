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
    PUSH,
    POP,
    STLD,
    STST,
    
    /*==>  Heap  <==*/
    ALLOC,
    FREE,
    HELD,
    HEST,

    /*==> Mem Func <==*/  
    IMOV,
    LMOV,    /*dst(r), in(r)*/
    FMOV,
    DMOV,
    SRI,   /*dst(r), _, val*/
    SRD,   /*dst(r), _, val*/
    SMLD,   /*dst(r), in(st), val*/
    SMST,   /*dst(st), in(r), val*/

    /*==>  Math  <==*/
    /* 32-bit int commands*/
    IADD,    /*dst(r), val(r), val(r)*/
    ISUB,    /*dst(r), val(r), val(r)*/
    IMULT,   /*dst(r), val(r), val(r)*/
    IDIV,    /*dst(r), val(r), val(r)*/
    IMOD,    /*dst(r), val(r), val(r)*/
    IINC,    /*dst(r)*/
    IDEC,    /*dst(r)*/

    /* 64-bit int commands*/    
    LADD,    /*dst(r), val(r), val(r)*/
    LSUB,    /*dst(r), val(r), val(r)*/
    LMULT,   /*dst(r), val(r), val(r)*/
    LDIV,    /*dst(r), val(r), val(r)*/
    LMOD,    /*dst(r), val(r), val(r)*/
    LINC,    /*dst(r)*/
    LDEC,    /*dst(r)*/
    
    /* 32-bit float commands*/    
    FADD,
    FSUB,
    FMULT,
    FDIV,
    FINC,
    FDEC,

    /* 64-bit float commands*/    
    DADD,
    DSUB,
    DMULT,
    DDIV,
    DINC,
    DDEC,

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
    JEZ,
    JNZ,
    JEQ,
    JNQ,
    JML,
    JEL,
    JMG,
    JEG,

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
    R7,
    D0 = 0,
    D1,
    D2
};

typedef struct {
    enum OPCODE opcode;
    unsigned char r1;
    unsigned char r2;
    union {
        unsigned long long vali;
        double valf;
    };
} Instruction;

typedef struct {
    Instruction*   inst_p;
    unsigned long  inst_count;
    unsigned char* static_mem_p;
    unsigned long  static_mem_size; /* Size in bytes */
} ncvm;



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
_export unsigned char ncvm_execute(ncvm* vm);
_export unsigned char ncvm_create_thread(
    ncvm* vm, Instruction* start_instr_p, unsigned char* ext_stack_p
);


/* For cpp support */
#ifdef __cplusplus
}
#endif

#endif