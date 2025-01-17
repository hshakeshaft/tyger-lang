#include <stdio.h>

int main(int argc, const char *argv[])
{
    for (int i = 0; i < argc; ++i)
    {
        printf("argv[%i] = %s\n", i, argv[i]);
    }

    return 0;
}
