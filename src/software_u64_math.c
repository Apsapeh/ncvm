#include <ncvm.h>

#ifdef __NCVM_SOFTWARE_U64_MATH

unsigned_long_long __software_u64_math_mul(unsigned_long_long a, unsigned_long_long b) {
    unsigned_long_long result;
    unsigned long a_low = a._[0] & 0xFFFF;
    unsigned long a_high = a._[0] >> 16;
    unsigned long b_low = b._[0] & 0xFFFF;
    unsigned long b_high = b._[0] >> 16;

    unsigned long low_low = a_low * b_low;
    unsigned long low_high = a_low * b_high;
    unsigned long high_low = a_high * b_low;
    unsigned long high_high = a_high * b_high;

    result._[0] = low_low + (low_high << 16) + (high_low << 16);
    result._[1] = a._[1] * b._[0] + a._[0] * b._[1] + (high_high << 16) + (low_high >> 16) + (high_low >> 16);

    return result;
}

#endif
