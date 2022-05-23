#include "CPU.h"

#include <stdio.h>

int main(int argc, char ** argv)
{
    if (argc <= 1)
    {
        printf ("Few arguments\n");
        return 1;
    }
    else if (argc > 2)
    {
        printf ("Too many arguments. Possibility of error\n");
    }

    runProgram (argv[1]);

    printf ("\nProcess completed\n");
    return 0;
}
