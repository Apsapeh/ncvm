#include <iostream>
#include <ncvm.h>


int main() {
    Instruction instructions[] = {
        Instruction_Int(ISMLD, 0, 0, 1),
        Instruction_Int(ISMLD, 1, 1, 1),
        Instruction_Int(IADD,  2, 0, 1),
        Instruction_Int(ISMST, 2, 2, 1),
        Instruction_Int(STOP,  0, 0, 0),
    };

    unsigned char static_memory[] = {
        0x10, 0x15, 0x00
    };
    
    ncvm vm = ncvm_initArr(
        instructions, sizeof(instructions) / sizeof(Instruction),
        static_memory, sizeof(static_memory) / sizeof(char)
    );
    
    ncvm_execute(&vm, DefaultThreadSettings);  

    int i = 0;
    for (auto & ch : static_memory)
        std::cout << "[" << i++ << "] - " << (int)ch  << "\n";
}