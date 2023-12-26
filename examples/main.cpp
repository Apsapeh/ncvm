#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ncvm.h>
//#include <dlfcn.h>
#include <cstdint>

/**
 * @brief Calculates the factorial of a given number.
 * 
 * @param n The number for which factorial needs to be calculated.
 * @return The factorial of the given number.
 */
int main() {
    std::cout << factorial(125) << "\n";
    int* s = get_size();
    std::cout << s[0] << "  |  " << s[1] << "  |  " << s[2] << "\n";
    free(s);
}