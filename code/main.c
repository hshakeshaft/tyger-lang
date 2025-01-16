#include <stdio.h>
#include "foo.h"

int main(int argc, const char *argv[])
{
    for (int i = 0; i < argc; ++i)
    {
        printf("argv[%i] = %s\n", i, argv[i]);
    }

    for (int i = 0; i < 10; ++i)
    {
        printf("square(%i) = %i | ", i, squarei(i));
    }
    printf("\n");

    return 0;
}
