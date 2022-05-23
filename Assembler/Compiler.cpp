#include "Assembler.h"

#include "Onegin/Onegin.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>

#include "HashTable/HashTable.h"

static const Command** compileCommandsSort ();
static int compileSortCmp (const void* arg1, const void* arg2);

typedef struct
{
    const char* name;
    size_t place;
} JumpMark;

static int cmp (const void *key, const void *datum);
static int isequal (const void* a, const void* b)
{
    return !strcmp (((const JumpMark*) a) -> name, (const char*) b);
}
static unsigned long long int hash (const void* val);

static size_t encodeArgCmd (unsigned char *buffer, const char *str);
static size_t encodeJumpMarck (const char *str, HashTable* JumpMarkTable, size_t rip = (size_t) -1);


void Compile (char * InFileName)
{
    assert (InFileName);

    //getting code to compile
    StringList* strList = fGetStringList (InFileName);

    //sorting commands by name
    const Command** CommandsSort = compileCommandsSort ();

    //getting memory for buffer
    unsigned char* buffer = (unsigned char*) calloc (strList -> length * MAX_COMMAND_SIZE, sizeof (char));

    //declaribg table of jump markers
    HashTable* JumpMarkTable =  HashTableCtr (sizeof (JumpMark), hash, isequal);

    size_t rip = 0;
    size_t strNum = 0;
    size_t emptyStrNum = 0;
    //compiling code
    for (int counter = 0; counter < 2; counter ++)
    {
        rip = 0;
        strNum = 0;
        emptyStrNum = 0;
        for (; strNum < strList -> length; strNum ++)
        {
            String* curstr = strList -> items[strNum];
            
            //skip if empty string
            if (curstr -> length == 0)
            {
                emptyStrNum ++;
                continue;
            }

            //check if it is jump marck
            if (encodeJumpMarck (curstr -> string, JumpMarkTable, rip) != (size_t) -1)
                continue;

            //getting name of command
            int nameLen = 0;
            while (curstr -> string[nameLen] == ' ' &&
                   curstr -> string[nameLen] != '\0')
                nameLen ++;


            int nameBeg = nameLen;

            while (curstr -> string[nameLen] != ' ' &&
                   curstr -> string[nameLen] != '\0')
            {
                curstr -> string[nameLen] = tolower (curstr -> string[nameLen]);
                nameLen ++;
            }

            printf ("%s\n", curstr -> string + nameLen);


            curstr -> string[nameLen] = '\0';

            //searching for name of command
            Command** cmdres = (Command**) bsearch (curstr -> string + nameBeg, CommandsSort, CommandsCount, sizeof (Command*), cmp);
            if (cmdres == nullptr)
            {
                printf ("ERROR: unknown command \"%s\"\n", curstr -> string);
                break;
            }
            Command* cmd = *cmdres;

            buffer[rip] = cmd -> code;

            //codding command
            if (cmd -> harg) //command with argument
            {
                if (curstr -> length > nameLen)
                    curstr -> string[nameLen] = ' ';

                int plusRip= encodeArgCmd (buffer + rip, curstr -> string + nameLen);
                if (plusRip < 0)
                    return;

                rip += plusRip;
            }
            else if (IS_JUMP_CODE (cmd -> code)) //jump command
            {
                if (curstr -> length > nameLen)
                    curstr -> string[nameLen] = ' ';

                rip ++;

                size_t jumpPos = encodeJumpMarck (curstr -> string + nameLen, JumpMarkTable);
                if (counter == 1 && jumpPos == (size_t) -1)
                    printf ("ERROR: There is no jump marckers, called \"%s\"", curstr -> string + nameLen);
                * (size_t*) (buffer + rip) = jumpPos;
                
                rip += sizeof (size_t);
            }
            else //command without argument
                rip ++;
        }
    }

    HashTableDtr (JumpMarkTable);
    StringListDtor (strList);

    //making name for out file
    char * OutFileName = nullptr;
    OutFileName = mkAnotherFormatFileName (InFileName, COMPILE_FILE_FORMAT);

    //opening file to write compiled program
    FILE * OutFile = nullptr;
    OutFile = fopen (OutFileName, "wb");
    if (OutFile == nullptr)
    {
        printf ("File %s can not be created.", OutFileName);
        return;
    }

    strNum -= emptyStrNum;
    printf ("%lu\n", strNum);

    //writing compiled program
    fwrite (&strNum, sizeof (size_t), 1, OutFile);
    fwrite (buffer, sizeof (char), rip, OutFile);

    fclose (OutFile);

    free (OutFileName);

    free (CommandsSort);
    free (buffer);
}

static const Command** compileCommandsSort ()
{
    const Command** CommandsSort = (const Command**) calloc (CommandsCount, sizeof (Command*));

    for (int i = 0; i < CommandsCount; i ++)
        CommandsSort[i] = &Commands[i];

    qsort (CommandsSort, CommandsCount, sizeof (Command*), compileSortCmp);

    return CommandsSort;
}

static int compileSortCmp (const void* arg1, const void* arg2)
{
    return strcmp ((*(Command**)arg1) -> name, (*(Command**)arg2) -> name);
}

static int cmp (const void *key, const void *datum)
{
    return strcmp ((const char*) key, (*(Command**) datum) -> name);
}

unsigned long long int hash (const void* val)
{
    unsigned long long int hash = 0;

    const char* str = (const char*)val;

    while (*str != '\0')
    {
        hash *= 31;
        hash += *str;
        str ++;
    }

    return hash;
}

static size_t encodeArgCmd (unsigned char *buffer, const char *str)
{
    unsigned char* code = buffer;
    size_t ip = 1;
    char arg[MAX_ARGUMENT_SIZE] = {};

    *code |= 1 << 7;


    if (sscanf (str, "%*[ ]%[[]", arg) > 0)
    {
        *code |= 1 << 6;
    }
    if (sscanf (str, "%*[^A-Za-z]%[A-Za-z]", arg) > 0)
    {
        *code |= 1 << 5;

        if (arg[0] != 'r' || arg[1] > 'd' || arg[1] < 'a' || arg[2] != 'x' || arg[3] != '\0')
        {
            printf ("ERROR: unknown name of register \"%s\"", arg);
            return -1;
        }

        buffer[ip ++] = arg[1] - 'a';
    }
    if (sscanf (str, "%*[^-+0-9\n]%"arg_format,  (arg_type*) arg) > 0)
    {
        *code |= 1 << 4;
        memcpy (buffer + ip, arg, sizeof (arg_type));
        ip += sizeof (arg_type);
    }
    return ip;
}

static size_t encodeJumpMarck (const char *str, HashTable* JumpMarkTable, size_t rip)
{
    while (*str == ' ')
        str ++;

    for (size_t i = 0; str[i] != ':'; i ++)
        if (str[i] == '\0' || str[i] == ' ')
            return (size_t) -1;

    JumpMark* jumpMark = (JumpMark*) HashTableFind (JumpMarkTable, str, rip != (size_t) -1);

    if (jumpMark == nullptr)
        return (size_t) -1;

    if (rip != (size_t) -1)
    {
        if (jumpMark -> name != nullptr && jumpMark -> place != rip)
        {
            printf ("ERROR: double marckers with name %s\n", str);
        }

        jumpMark -> name = str;
        jumpMark -> place = rip;

        printf ("%lu\n", rip);
    }


    return jumpMark -> place;
}
