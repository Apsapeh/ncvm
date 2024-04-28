#include <iostream>
#include <ncvm.h>

#include <fstream>

std::ifstream file("/Users/ghost/Desktop/Rust Projects/Projects/ncvm_asm/foo.bin", std::ios::binary);



const unsigned char* get_next_n_bytes(unsigned long long n, void* data_p) {
    unsigned char* data = new unsigned char[n];
    file.read((char*)data, n);
    return data;
}



int main() {
    /*Instruction instructions[] = {
        Instruction_Int(LSR,   0, 0, 0),
        Instruction_Int(ISMLD, 0, 1, 0),

        Instruction_Int(LSR,   0, 1, 0),
        Instruction_Int(ISMLD, 1, 1, 0),

        Instruction_Int(IADD,  2, 0, 1),
        
        Instruction_Int(LSR, 0, 10, 0),
        Instruction_Int(JMP, 0, 0, 0),
        Instruction_Int(IADD,  2, 2, 1),
        Instruction_Int(IADD,  2, 2, 1),
        Instruction_Int(IADD,  2, 2, 1),

        Instruction_Int(LSR,   0, 2, 0),
        Instruction_Int(ISMST, 2, 1, 0),

        Instruction_Int(STOP,  0, 0, 0),

    };*/

    /*
        a = 3;
        b = 4;
        if a < b:
            c = 1;
        else:
            c = 2;
    */
    /*unsigned char static_memory[] = {
        0x00
    };
    Instruction instructions[] = {
        // a
        Instruction_Int(ISR, 0, 3, 0),

        // b
        Instruction_Int(ISR, 1, 4, 0),

        // if a < b
        Instruction_Int(LSR,  0, 7, 0),
        Instruction_Int(IJML, 0, 1, 0),
        //false:
        Instruction_Int(LSR,  0, 8, 0),
        Instruction_Int(ISR, 2, 2, 0),
        Instruction_Int(JMP, 0, 0, 0),

        //true:
        Instruction_Int(ISR, 2, 1, 0),

        //end: c
        Instruction_Int(LSR,   0, 0, 0),
        Instruction_Int(ISMST, 2, 1, 0),

        Instruction_Int(STOP,  0, 0, 0),
    };*/

    /*Instruction instructions[] = {
        //Instruction_Int(ISR, 0, 0, 1), //Set 256 to IR0
        //Instruction_Int(LSR, 1, 0, 1), //Set 256 to LR1
        Instruction_Int(ISMLD, 0, 1, 0)
    };*/

    /*unsigned char static_memory[] = {
        0x10, 0x15, 0x00
    };*/
    
    /*ncvm vm = ncvm_initArr(
        instructions,// sizeof(instructions) / sizeof(Instruction),
        static_memory//, sizeof(static_memory) / sizeof(char)
    );
    
    ncvm_execute(&vm, DefaultThreadSettings);  

    int i = 0;
    for (auto & ch : static_memory)
        std::cout << "[" << i++ << "] - " << (int)ch  << "\n";*/

    //read byte file
    const char* libs[]= {
        "build/macosx/arm64/release/liblib1.dylib",
        //"math"
    };

    ncvm_default_lib_loader lib_loader = ncvm_default_lib_loader_init(libs, 1);

    std::ifstream file("/Users/ghost/Desktop/Rust Projects/Projects/ncvm_asm/foo.bin", std::ios::binary);
    int ret_code = 0;
    ncvm vm = ncvm_loadBytecodeStream(
        get_next_n_bytes,
        nullptr,
        ncvm_default_get_lib_function,
        &lib_loader,
        &ret_code
    );
    file.close();

    ncvm_thread thread = ncvm_create_thread(&vm, vm.inst_p, NULL, 0, DefaultThreadSettings, NULL);
    ncvm_execute_thread(&thread);

    ncvm_thread_free(&thread);
    ncvm_free(&vm);
    ncvm_default_lib_function_loader_free(&lib_loader);
    //std::cout << "\n\nStatic memory:\n";
    //for (int i = 0; i < vm.static_mem_size; i++)
    //    std::cout << "[" << i << "] - " << (int)vm.static_mem_p[i] << "\n";
}