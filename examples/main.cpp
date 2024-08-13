#include <iostream>
#include <ncvm.h>

#include <fstream>
//#include <unordered_map>

std::ifstream file("examples/asm/perf_test.ncvm", std::ios::binary);



const unsigned char* get_next_n_bytes(unsigned long n, void* data_p) {
    unsigned char* data = new unsigned char[n];
    file.read((char*)data, n);
    return data;
}



int main() {
    /* read byte file */
    const char* libs[]= {
        "build/linux/x86_64/release/liblib1.so",
    };

    ncvm_default_lib_loader lib_loader = ncvm_default_lib_loader_init(libs, 1);

    std::ifstream file("examples/asm/perf_test.ncvm", std::ios::binary);
    int ret_code = 0;
    ncvm vm = ncvm_loadBytecodeStream(
        get_next_n_bytes,
        nullptr,
        ncvm_default_get_lib_function,
        &lib_loader,
        &ret_code
    );
    file.close();

    ThreadSettings settings;
    DefaultThreadSettingsANSI(settings);
    ncvm_thread thread = ncvm_create_thread(&vm, vm.inst_p, NULL, 0, settings, NULL);
    

    /*std::unordered_map<int, double> times;
    std::unordered_map<unsigned char, int> counters;
    while (thread.current_instr_p->opcode != STOP) {
        auto start = std::chrono::high_resolution_clock::now();
        ncvm_execute_thread_step(&thread);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        int command_id = thread.current_instr_p - vm.inst_p-1;

        if (times.find(command_id) == times.end()) {
            times[command_id] = elapsed.count();
        } else {
            times[command_id] += elapsed.count();
        }

        if (counters.find(thread.current_instr_p->opcode) == counters.end()) {
            counters[thread.current_instr_p->opcode] = 1;
        } else {
            counters[thread.current_instr_p->opcode] += 1;
        }

        //std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    }

    for (auto& [key, value] : times) {
        std::cout << "Command " << key << " took " << value << " s\n";
    }

    long long summ = 0;
    for (auto& [key, value] : counters) {
        std::cout << "Command " << (int)key << " was executed " << value << " times\n";
        summ += value;
    }

    std::cout << "Total commands executed: " << summ << "\n";*/
    




    
    ncvm_execute_thread(&thread);
    

    ncvm_thread_free(&thread);
    ncvm_free(&vm);
    ncvm_default_lib_function_loader_free(&lib_loader);
    /*std::cout << "\n\nStatic memory:\n";
    for (int i = 0; i < vm.static_mem_size; i++)
        std::cout << "[" << i << "] - " << (int)vm.static_mem_p[i] << "\n";*/
}