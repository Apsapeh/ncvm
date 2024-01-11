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

    /*==>  Heap  <==*/
    ALLOC,
    FREE,
    LOAD,
    SET,

    /*==>  Math  <==*/
    ADD,
    SUB,
    MULT,
    DIV,
    MOD,
    INC,
    DEC,

    /*==>  Logic  <==*/


    /*==>  Jump  <==*/
    JMP,

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
} Instruction;

typedef struct {
    Instruction* inst_p;
    long         inst_count;
} ncvm;



/* ======================> Functions (C and CPP) <====================== */

/* For cpp support */
#ifdef __cplusplus
extern "C" {
#endif

_export ncvm ncvm_initArr(Instruction* inst_p, long inst_count);
_export ncvm ncvm_initData(const char* data_p, long data_size);
_export ncvm ncvm_initFile(const char* filename);
_export void ncvm_free(ncvm* vm);

_export int ncvm_execute(ncvm* vm);


_export int factorial(int n);
_export int* get_size();


/* For cpp support */
#ifdef __cplusplus
}
#endif

#endif