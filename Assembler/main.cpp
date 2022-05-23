#include "Assembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int assembSortCmp (const void* arg1, const void* arg2);

int main(int argc, char ** argv)
{
    if (argc <= 1)
    {
        printf ("Few arguments\n");
        return 1;
    }
    else if (argc > 3)
    {
        printf ("Too many arguments. Possibility of error\n");
    }

    if (strcmp (argv[1], "compile") == 0)
    {
        Compile (argv[2]);
    }
    else if (strcmp (argv[1], "decompile") == 0)
    {
        Decompile (argv[2]);
    }
    else
    {
        printf ("Wrong command");
    }

    printf ("\nProcess completed\n");
    return 0;
}
