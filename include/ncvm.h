#ifndef __NCVM
#define __NCVM

enum OPCODE {
    FIRST
};

struct Instruction
{
    enum OPCODE opcode;
    /* data */
};



// ======================> Functions (C and CPP) <======================

// For cpp support
#ifdef __cplusplus
extern "C" {
#endif


int factorial(int n);


// For cpp support
#ifdef __cplusplus
}
#endif

#endif