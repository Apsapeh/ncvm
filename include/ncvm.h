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
    //==>  Stack  <==//
    PUSH,
    POP,

    //==>  Heap  <==//
    ALLOC,
    FREE,
    LOAD,
    SET,

    //==>  Math  <==//
    ADD,
    SUB,
    MULT,
    DIV,
    MOD,
    INC,
    DEC,

    //==>  Logic  <==//


    //==>  Jump  <==//
    JMP,

    //==>  Lib  <==//
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

typedef struct Instruction{
    enum OPCODE opcode;
}  Instruction;



// ======================> Functions (C and CPP) <======================

// For cpp support
#ifdef __cplusplus
extern "C" {
#endif


_export int factorial(int n);
_export int* get_size();


// For cpp support
#ifdef __cplusplus
}
#endif

#endif