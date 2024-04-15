#include "util.h"

int is_little_endian()
{
  unsigned int x = 1;
  char *c = (char*) &x;
  return (int)*c;
}