#include "util.h"

int is_little_endian(void)
{
    unsigned int x = 1;
    char *c = (char*) &x;
    return (int)*c;
}
