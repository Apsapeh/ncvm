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
    //read byte file
    const char* libs[]= {
        "build/macosx/arm64/release/liblib1.dylib",
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