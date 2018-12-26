
#include <stdio.h>
#include <stdlib.h>

void error( int i )
{
     fprintf(stderr, "error: %d\n", i);
     exit(i);
}