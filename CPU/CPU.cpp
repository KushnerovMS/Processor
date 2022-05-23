#include "CPU.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

#include "Stack/Stack.h"

static int CodeCmp (const void* arg1, const void* arg2);
static const Command** CommandsSortByCode ();

void dataPrint (FILE * file, const void * item)
{
    fprintf (file, "%"arg_format, *(arg_type*) item);
}


void runProgram (char* InFileName)
{
    assert (InFileName);

    struct CPU* CPU = (struct CPU*) calloc (1, sizeof (struct CPU));

    for (int i = 0; i < sizeof (CPU -> reg) / sizeof (CPU -> reg[0]); i ++)
        CPU -> reg[i] = 0;

    //CPU init
    CPU -> stack = (Stack*) calloc (1, sizeof (Stack));
    StackCtor (CPU -> stack, INITIAL_STACK_CAPACITY, arg_type, dataPrint)

    //opening file
    struct stat stInBuff = {};
    FILE * InFile = fopen (InFileName, "rb");
    if (stat (InFileName, &stInBuff) != 0 || InFile == nullptr)
        return;

    //reading file in CPU
    CPU -> code = (unsigned char*) calloc (stInBuff.st_size, sizeof (char));
    fread (CPU -> code, stInBuff.st_size, sizeof (char), InFile);
    fclose (InFile);

    //getting number of commands
    size_t cmdNumb = * (size_t*) CPU -> code;
    CPU -> code += sizeof (size_t);
    CPU -> rip = 0;

    //sorting commands by code
    const Command** CommandsSort = CommandsSortByCode ();

    //counting commands with argument
    size_t argCmdCount = 0;
    while (CommandsSort[argCmdCount] -> harg)
        argCmdCount ++;


    //running commands
    unsigned char cmdNum = 0;

    do
    {
        //get command
        if (CPU -> code[CPU -> rip] < (1 << 7)) //command without argument
            cmdNum = CPU -> code[CPU -> rip] + argCmdCount;
        else //command with argument
            cmdNum = CPU -> code[CPU -> rip] & 15;
    }
    while (CommandsSort[cmdNum] -> cmd (CPU) == 0 && CPU -> rip < stInBuff.st_size);
    
    StackDtor (CPU -> stack);
    free (CPU -> stack);
    free (CPU -> code - sizeof (size_t));
    free (CPU);

    free (CommandsSort);
}

static int CodeCmp (const void* arg1, const void* arg2)
{
    return ((*(Command**)arg1) -> harg != (*(Command**)arg2) -> harg)?
            ((*(Command**)arg2) -> harg -  (*(Command**)arg1) -> harg) * 1000 :
            (*(Command**)arg1) -> code -  (*(Command**)arg2) -> code;
}

static const Command** CommandsSortByCode ()
{
    const Command** CommandsSort = (const Command**)
        calloc (CommandsCount, sizeof (Command*));

    for (int i = 0; i < CommandsCount; i ++)
        CommandsSort[i] = &Commands[i];

    qsort (CommandsSort, CommandsCount, sizeof (Command*), CodeCmp);

    return CommandsSort;
}

arg_type getArg (CPU* CPU)
{
    unsigned char code = CPU -> code[CPU -> rip ++];

    arg_type arg = 0;

    if (code & (1 << 5))
    {
        arg += CPU -> reg[CPU -> code[CPU -> rip ++]];
    }

    if (code & (1 << 4))
    {
        arg += * (arg_type*) (CPU -> code + CPU -> rip);
        CPU -> rip += sizeof (arg_type);
    }

    if (code & (1 << 6))
    {
        arg = CPU -> cash [(unsigned)arg];
    }

    return arg;
}

arg_type* getArgPtr (CPU* CPU)
{
    unsigned char code = CPU -> code[CPU -> rip ++];

    arg_type* argptr = nullptr;

    long long int shift = 0;

    if (code & (1 << 5))
    {
        argptr = CPU -> reg + CPU -> code[CPU -> rip ++];
    }

    if (code & (1 << 4))
    {
        shift = (long long int) * (arg_type*) (CPU -> code + CPU -> rip);

        CPU -> rip += sizeof (arg_type);
    }

    if (code & (1 << 6))
    {
        argptr = CPU -> cash + (long long int) * (arg_type*) argptr + shift;
    }

    if (code & (1 << 6) == 0 && code & (1 << 5) == 0)
        return nullptr;

    return argptr;
}
