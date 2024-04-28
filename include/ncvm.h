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

#define NCVM_MIN_VERSION 0
#define NCVM_VERSION     0

enum _packed OPCODE {
    /*==>  Other  <==*/
    NOP,    /*_|_|_|_|_*/
    STOP,   /*_|_|_|_|_*/
    RET,    /*_|_|_|_|_*/


    /*==> Mem Func <==*/  
    IMOV,   /*ird|ir1|_|_|ir1->ird*/
    LMOV,   /*lrd|lr1|_|_|lr1->lrd*/
    FMOV,   /*frd|fr1|_|_|fr1->frd*/
    DMOV,   /*drd|dr1|_|_|dr1->drd*/

    IRCLR,  /*ir1|_|_|_|Clear ir1*/
    LRCLR,  /*lr1|_|_|_|Clear lr1*/
    FRCLR,  /*fr1|_|_|_|Clear fr1*/
    DRCLR,  /*dr1|_|_|_|Clear dr1*/

    ISR,    /*ird|0..7|8..15|_|Set first 16 bits to ird*/
    LSR,    /*lrd|0..7|8..15|_|Set first 16 bits to lrd*/
    /*ISRF,    ird|0..7|8..15|_|Set first 16 bits to ird*/
    /*ISRS,    ird|0..7|8..15|_|Set 16..31 bits to ird*/
    /*LSRF,    lrd|0..7|8..15|_|Set first 16 bits to lrd*/
    /*LSRS,    lrd|0..7|8..15|_|Set 16..31 bits to lrd*/
    /*LSRT,    lrd|0..7|8..15|_|Set 32..47 bits to lrd*/
    /*LSRQ,    lrd|0..7|8..15|_|Set 48..63 bits to lrd*/

    IRSI,   /*ir1|v|_|_|Shift ir1 on 'v' count of bits to the right*/
    ILSI,   /*ir1|v|_|_|Shift ir1 on 'v' count of bits to the left*/
    LRSI,   /*lr1|v|_|_|Shift lr1 on 'v' count of bits to the right*/
    LLSI,   /*lr1|v|_|_|Shift lr1 on 'v' count of bits to the left*/

    IRSA,   /*ir1|_|_|a|Shift ir1 on ar bits to the right*/
    ILSA,   /*ir1|_|_|a|Shift ir1 on ar bits to the left*/
    LRSA,   /*lr1|_|_|a|Shift lr1 on ar bits to the right*/
    LLSA,   /*lr1|_|_|a|Shift lr1 on ar bits to the left*/

    /*==> Static Mem Func <==*/
    /* 32-bit int commands */    
    ISMLD,  /*ird|v|_|a|Load 'v' count of bytes from static memory to ird. ird = ST_MEM[ar..ar+v]*/
    ISMST,  /*ir1|v|_|a|Set 'v' count of bytes from ir1 to static memory. ST_MEM[ar..ar+v] = ir1[0..v]*/

    /* 64-bit int commands */        
    LSMLD,  /*lrd|v|_|a|Load 'v' count of bytes from static memory to lrd. lrd = ST_MEM[ar..ar+v]*/
    LSMST,  /*lr1|v|_|a|Set 'v' count of bytes from lr1 to static memory. ST_MEM[ar..ar+v] = lr1[0..v]*/
    
    /* 32-bit float commands */        
    FSMLD,  /*frd|_|_|a|Load 4 bytes from static memory to frd. frd = ST_MEM[ar..ar+4]*/
    FSMST,  /*fr1|_|_|a|Set 4 bytes from fr1 to static memory. ST_MEM[ar..ar+4] = fr1*/
    
    /* 64-bit float commands*/ 
    DSMLD,  /*drd|_|_|a|Load 8 bytes from static memory to drd. drd = ST_MEM[ar..ar+8]*/
    DSMST,  /*dr1|_|_|a|Set 8 bytes from dr1 to static memory. ST_MEM[ar..ar+8] = dr1*/ 


    /*==>  Stack  <==*/
    POPI,   /*0..7|8..15|16..23|Pop from stack (0..23) bytes*/
    POPA,   /*_|_|_|a|Pop from stack ar bytes*/
    
    IPUSH,  /*ir1|v|_|_|Push to stack 'v' count of bytes from ir1*/
    ISTLD,  /*ird|v|_|a|Load 'v' count of bytes from stack to ird. ird = STACK[ar..ar+v]*/
    ISTST,  /*ir1|v|_|a|Set 'v' count of bytes from ir1 to stack. STACK[ar..ar+v] = ir1[0..v]*/
    
    LPUSH,  /*lr1|v|_|_|Push to stack 'v' count of bytes from lr1*/
    LSTLD,  /*lrd|v|_|a|Load 'v' count of bytes from stack to lrd. lrd = STACK[ar..ar+v]*/
    LSTST,  /*lr1|v|_|a|Set 'v' count of bytes from lr1 to stack. STACK[ar..ar+v] = lr1[0..v]*/
    
    FPUSH,  /*fr1|_|_|_|Push to stack 4 bytes from fr1*/
    FSTLD,  /*frd|_|_|a|Load 4 bytes from stack to frd. frd = STACK[ar..ar+4]*/
    FSTST,  /*fr1|_|_|a|Set 4 bytes from fr1 to stack. STACK[ar..ar+4] = fr1[0..4]*/
    
    DPUSH,  /*dr1|_|_|_|Push to stack 8 bytes from dr1*/
    DSTLD,  /*drd|_|_|a|Load 8 bytes from stack to drd. drd = STACK[ar..ar+8]*/
    DSTST,  /*dr1|_|_|a|Set 8 bytes from dr1 to stack. STACK[ar..ar+8] = dr1[0..8]*/


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
    JMP,    /*_|_|_|a|jmp to ar*/
    /* 32-bit int commands */    
    IJEZ,   /*ir1|_|_|a|jmp to ar if ir1 == 0*/
    IJNZ,   /*ir1|_|_|a|jmp to ar if ir1 != 0*/
    IJEQ,   /*ir1|ir2|_|a|jmp to ar if ir1 == ir2*/
    IJNQ,   /*ir1|ir2|_|a|jmp to ar if ir1 != ir2*/
    IJML,   /*ir1|ir2|_|a|jmp to ar if ir1 < ir2*/
    IJEL,   /*ir1|ir2|_|a|jmp to ar if ir1 <= ir2*/
    IJMG,   /*ir1|ir2|_|a|jmp to ar if ir1 > ir2*/
    IJEG,   /*ir1|ir2|_|a|jmp to ar if ir1 >= ir2*/
    
    /* 64-bit int commands */        
    LJEZ,   /*lr1|_|_|a|jmp to ar if lr1 == 0*/
    LJNZ,   /*lr1|_|_|a|jmp to ar if lr1 != 0*/
    LJEQ,   /*lr1|lr2|_|a|jmp to ar if lr1 == lr2*/
    LJNQ,   /*lr1|lr2|_|a|jmp to ar if lr1 != lr2*/
    LJML,   /*lr1|lr2|_|a|jmp to ar if lr1 < lr2*/
    LJEL,   /*lr1|lr2|_|a|jmp to ar if lr1 <= lr2*/
    LJMG,   /*lr1|lr2|_|a|jmp to ar if lr1 > lr2*/
    LJEG,   /*lr1|lr2|_|a|jmp to ar if lr1 >= lr2*/
    
    /* 32-bit float commands */        
    FJEZ,   /*fr1|_|_|a|jmp to ar if fr1 == 0*/
    FJNZ,   /*fr1|_|_|a|jmp to ar if fr1 != 0*/
    FJEQ,   /*fr1|fr2|_|a|jmp to ar if fr1 == fr2*/
    FJNQ,   /*fr1|fr2|_|a|jmp to ar if fr1 != fr2*/
    FJML,   /*fr1|fr2|_|a|jmp to ar if fr1 < fr2*/
    FJEL,   /*fr1|fr2|_|a|jmp to ar if fr1 <= fr2*/
    FJMG,   /*fr1|fr2|_|a|jmp to ar if fr1 > fr2*/
    FJEG,   /*fr1|fr2|_|a|jmp to ar if fr1 >= fr2*/
    
    /* 64-bit float commands */        
    DJEZ,   /*dr1|_|_|a|jmp to ar if dr1 == 0*/
    DJNZ,   /*dr1|_|_|a|jmp to ar if dr1 != 0*/
    DJEQ,   /*dr1|dr2|_|a|jmp to ar if dr1 == dr2*/
    DJNQ,   /*dr1|dr2|_|a|jmp to ar if dr1 != dr2*/
    DJML,   /*dr1|dr2|_|a|jmp to ar if dr1 < dr2*/
    DJEL,   /*dr1|dr2|_|a|jmp to ar if dr1 <= dr2*/
    DJMG,   /*dr1|dr2|_|a|jmp to ar if dr1 > dr2*/
    DJEG,   /*dr1|dr2|_|a|jmp to ar if dr1 >= dr2*/

    CALL,   /*_|_|_|a|Call to ar*/
    /*==>  Lib  <==*/
    LIBCALL  /*_|_|_|_*/
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
    unsigned char u32_reg_size;
    unsigned char u64_reg_size;
    unsigned char f32_reg_size;
    unsigned char f64_reg_size;
    unsigned long stack_size;   /* In bytes */
    unsigned long call_stack_size; /* In long */
} ThreadSettings;

#define DefaultThreadSettings {\
    .u32_reg_size=8, .u64_reg_size=8,\
    .f32_reg_size=8, .f64_reg_size=8,\
    .stack_size=1024*1024*1,\
    .call_stack_size=1024*128\
}

typedef struct {
    Instruction*   inst_p;
    unsigned long  inst_count;
    unsigned char* static_mem_p;
    unsigned long  static_mem_size; /* Size in bytes */
    ThreadSettings main_thread_settings;
    void**         lib_functions;
} ncvm;

typedef struct {
    ncvm* vm;
    const Instruction* current_instr_p;
    void* stack_p;
    void* call_stack_p;
    unsigned int*       u32_registers;
    unsigned long long* u64_registers;
    float*              f32_registers;
    double*             f64_registers;
} ncvm_thread;


typedef struct {
    unsigned long static_mem_idx;
    unsigned long inst_idx;
} ByteCodeBlocksInfo;


typedef void (*ncvm_lib_function)(ncvm_thread* thread);

#define NCVM_LIB_FUNCTION(name) void name(ncvm_thread* thread)

/* ======================> Functions (C and CPP) <====================== */

/* For cpp support */
#ifdef __cplusplus
extern "C" {
#endif

/**
    @brief Initialize VM    
    @param inst_p Instructions array
    @param static_mem_p Static memory
    @return VM
 */
_export ncvm ncvm_init(
    Instruction*      inst_p,  /*unsigned long inst_count,*/
    unsigned char*    static_mem_p,/*, unsigned long static_mem_size*/
    ncvm_lib_function (*get_lib_function)(const char* name, void* lib_data_p),
    void*             lib_data_p
);

/** 
    @brief Load VM from bytecode data
    @param data_p Bytecode data
    @param data_size Bytecode data size
    @return VM
*/
_export ncvm ncvm_loadBytecodeData(
    const unsigned char* data_p,
    const unsigned long  data_size,
    ncvm_lib_function    (*get_lib_function)(const char* name, void* lib_data_p),
    void*                lib_data_p
);

/** 
    @brief Load VM from bytecode stream
    @param get_next_n_bytes Function to get next n bytes
    @param data_p Data pointer
    @param ret_code Return code
    @return VM
*/
_export ncvm ncvm_loadBytecodeStream(
    const unsigned char* (*get_next_n_bytes)(const unsigned long long n, void* const data_p),
    void*             data_p,
    ncvm_lib_function (*get_lib_function)(const char* name, void* lib_data_p),
    void*             lib_data_p,
    int*              ret_code
);

/**
    @brief Free VM
    @param vm VM
*/
_export void ncvm_free(ncvm* vm);

/**
    @param vm VM
*/
_export unsigned char ncvm_execute(ncvm* vm);

_export ncvm_thread ncvm_create_thread(
    ncvm*              vm,
    const Instruction* start_instr_p, 
    unsigned char*     ext_stack_p,
    unsigned long      ext_stack_s,
    ThreadSettings     settings,
    int*               ret_code
);

_export void ncvm_thread_free(ncvm_thread* thread);
_export unsigned char ncvm_execute_thread_step(ncvm_thread* thread);
_export unsigned char ncvm_execute_thread(ncvm_thread* thread);
/*_export unsigned char ncvm_thread_free();*/


/* Default loaders */
/*#ifdef __NCVM_DEFAULT_LOADERS*/
typedef struct  {
    void** lib_handlers;
    unsigned long lib_handlers_count;
} ncvm_default_lib_loader;

_export ncvm_default_lib_loader ncvm_default_lib_loader_init(
    const char** lib_names,
    unsigned long lib_names_count
);
_export ncvm_lib_function ncvm_default_get_lib_function(
    const char* name,
    void* loader /* (ncvm_default_lib_loader*) */
);
_export void ncvm_default_lib_function_loader_free(
    ncvm_default_lib_loader* loader
);
/*#endif*/

/* For cpp support */
#ifdef __cplusplus
}
#endif

#endif