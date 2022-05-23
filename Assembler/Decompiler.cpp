#include "Assembler.h"
#include "Onegin/Onegin.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>

static int decompileSortCmp (const void* arg1, const void* arg2);

static const Command** decompileCommandsSort ();

void Decompile (char * InFileName)
{
    assert (InFileName);

    //opening file
    struct stat stInBuff = {};
    FILE * InFile = fopen (InFileName, "rb");
    if (stat (InFileName, &stInBuff) != 0 ||
        InFile == nullptr)
        return;

    //reading file in buffer
    unsigned char* buffer = (unsigned char*) calloc (stInBuff.st_size, sizeof (char));
    fread (buffer, stInBuff.st_size, sizeof (char), InFile);
    fclose (InFile);

    //getting count of commands in file
    size_t cmdCount = *((size_t*) buffer);
    buffer += sizeof (size_t);

    printf ("%lu\n", cmdCount);

    size_t rip = 0;

    //sorting commands by code
    const Command** CommandsSort = decompileCommandsSort ();

    //counting commands with argument
    size_t argCmdCount = 0;
    while (CommandsSort[argCmdCount] -> harg)
        argCmdCount ++;

    //get memory for string list
    StringList* strList = (StringList*) calloc (1, sizeof (StringList));
    strList -> items = (String**) calloc (cmdCount, sizeof (String*));
    strList -> length = 0;

    //decoding buffer
    while (rip < stInBuff.st_size - sizeof (size_t))
    {
        //get memory for string
        strList -> items[strList -> length] = (String*) calloc (1, sizeof (String));
        char* str = strList -> items[strList -> length] -> string = (char*) calloc (MAX_ARGUMENT_SIZE, sizeof (char));

        size_t comLen = 0;
        //decode command
        if (buffer[rip] < (1 << 7)) //command without argument
        {
            if (IS_JUMP_CODE (buffer[rip]))
            {
                comLen = sprintf (str, "%s %lu", Commands[buffer[rip ++] + argCmdCount].name, *(size_t*) (buffer + rip + 1));
                rip += sizeof (size_t);
            }
            else
                comLen = sprintf (str, "%s", Commands[buffer[rip ++] + argCmdCount].name);
        }
        else //command with argument
        {
            unsigned char code = buffer[rip ++];

            char regArg[] = "rax";
            if (code & (1 << 5))
                regArg[1] = buffer[rip ++] + 'a';

            char constArg[10] = "";
            if (code & (1 << 4))
            {
                arg_type arg = 0;
                sprintf (constArg, "%"arg_format, *((arg_type*)(buffer + rip)));
                rip += sizeof (arg_type);
            }

            //           name [ r + 1 ]
            comLen = sprintf (str, "%s %s%s%s%s%s",
                Commands[code & 15].name,
                (code & (1 << 6))? "[" : "",
                (code & (1 << 5))? regArg : "",
                (code & (1 << 5) && code & (1 << 4) && constArg[0] != '-')? "+" : "",
                constArg,
                (code & (1 << 6))? "]" : "");
        }

        //work with string
        strList -> items[strList -> length] -> length = comLen;
        strList -> items[strList -> length] -> string = (char*) realloc (str, (comLen + 1) * sizeof (char));
        strList -> length ++;
    }

    //printing decode code
    fPrintStringList (strList, stdout);

    StringListDtor (strList);

    free (buffer - sizeof (size_t));
    free (CommandsSort);
}

static int decompileSortCmp (const void* arg1, const void* arg2)
{
    return ((*(Command**)arg1) -> harg != (*(Command**)arg2) -> harg)?
            ((*(Command**)arg2) -> harg -  (*(Command**)arg1) -> harg) * 1000 :
            (*(Command**)arg1) -> code -  (*(Command**)arg2) -> code;
}

static const Command** decompileCommandsSort ()
{
    const Command** CommandsSort = (const Command**)
        calloc (CommandsCount, sizeof (Command*));

    for (int i = 0; i < CommandsCount; i ++)
        CommandsSort[i] = &Commands[i];

    qsort (CommandsSort, CommandsCount, sizeof (Command*), decompileSortCmp);

    return CommandsSort;
}
