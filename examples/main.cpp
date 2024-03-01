#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ncvm.h>


int main() {
    Instruction instructions[] = {
        {.opcode=DDIV, .r1=0, .r2=0, .r3.valf=0},
        /*{SMLD, 1, 0, 2},
        {SMLD, 2, 8, 1},
        {LADD, 1, 1, 2},
        {SMST, 6, 1, 2},
        {SMLD, 2, 2, 4},
        {SMLD, 3, 6, 2},
        {LADD, 0, 2, 3},
        {LDEC, 0, 0, 0},
        {LINC, 0, 0, 0},
        {SMLD, 1, 9, 1},
        {LMOD, 0, 0, 1},
        {LTOD, 0, 1},
        {SMLD, 2, 10, 1},
        {LTOD, 1, 2},*/
        {DDIV, 0, 0, 0.0},
        {DMULT, 0, 0, 0.0},
        {STOP, 0, 0, 0}
    };

    unsigned char static_memory[] = {
        0xE8, 0x03, 0x57, 0x12, 0x9E, 0x00, 0x00, 0x00,
        1, 2, 3
    };
    
    ncvm vm = ncvm_initArr(
        instructions, sizeof(instructions) / sizeof(Instruction),
        static_memory, sizeof(static_memory) / sizeof(char)
    );
    ncvm_execute(&vm);  

    return 0;
}