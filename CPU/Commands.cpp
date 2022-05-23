#ifndef DEF_COMMAND

#include "CPU.h"

#include <assert.h>
#include <math.h>
#include <kms/Logs.h>

#define DEF_COMMAND(HARG, CODE, NAME, DEFINITION) int NAME (CPU* CPU) {DEFINITION}

#endif


DEF_COMMAND (1, 0, push,
{
    assert (CPU);

    arg_type arg = getArg (CPU);

    StackPush (CPU -> stack, &arg);

    return 0;
})

DEF_COMMAND (1, 1, pop, 
{
    assert (CPU);

    arg_type* arg = getArgPtr (CPU);

    if (arg == nullptr)
    {
        arg_type trash = 0;
        StackPop (CPU -> stack, &trash);
    }
    else
        StackPop (CPU -> stack, arg);

    return 0;
})

DEF_COMMAND (0, 0, hlt,
{
    Logs.debug ("Registers state:");
    
    for (int i = 0; i < sizeof (CPU -> reg) / sizeof (CPU -> reg[0]); i ++)
        Logs.print ("r%cx = %"arg_format"\n", 'a' + i, CPU -> reg[i]);

    Logs.print ("\n");

    StackDump (CPU -> stack, ALL_INFO);

    CPU -> rip ++;

    return 1;
})

DEF_COMMAND (0, 1, in,
{
    assert (CPU);

    arg_type arg = 0;

    scanf ("%"arg_format, &arg);

    StackPush (CPU -> stack, &arg);

    CPU -> rip ++;

    return 0;
})

DEF_COMMAND (0, 2, out,
{
    assert (CPU);

    arg_type arg = 0;

    StackPop (CPU -> stack, &arg);

    printf ("%"arg_format"\n", arg);

    CPU -> rip ++;

    return 0;
})

DEF_COMMAND (0, 3, add,
{
    assert (CPU);

    arg_type arg1 = 0;
    arg_type arg2 = 0;

    StackPop (CPU -> stack, &arg1);
    StackPop (CPU -> stack, &arg2);

    arg1 += arg2;

    StackPush (CPU -> stack, &arg1);

    CPU -> rip ++;

    return 0;
})

DEF_COMMAND (0, 4, sub,
{
    assert (CPU);

    arg_type arg1 = 0;
    arg_type arg2 = 0;

    StackPop (CPU -> stack, &arg2);
    StackPop (CPU -> stack, &arg1);

    arg1 -= arg2;

    StackPush (CPU -> stack, &arg1);

    CPU -> rip ++;

    return 0;
})

DEF_COMMAND (0, 5, mul,
{
    assert (CPU);

    arg_type arg1 = 0;
    arg_type arg2 = 0;

    StackPop (CPU -> stack, &arg1);
    StackPop (CPU -> stack, &arg2);

    arg1 *= arg2;

    StackPush (CPU -> stack, &arg1);

    CPU -> rip ++;

    return 0;
})

DEF_COMMAND (0, 6, div,
{
    assert (CPU);

    arg_type arg1 = 0;
    arg_type arg2 = 0;

    StackPop (CPU -> stack, &arg2);
    StackPop (CPU -> stack, &arg1);

    arg1 /= arg2;

    StackPush (CPU -> stack, &arg1);

    CPU -> rip ++;

    return 0;
})

DEF_COMMAND (0, 7, sqrt,
{
    assert (CPU);

    arg_type arg = 0;

    StackPop (CPU -> stack, &arg);

    arg = sqrt(arg);

    StackPush (CPU -> stack, &arg);

    CPU -> rip ++;

    return 0;
})

#ifndef IS_JUMP_CODE
#define IS_JUMP_CODE(CODE) CODE >= 8 && CODE <= 8
#endif

DEF_COMMAND (0, 8, jmp,
{
    assert (CPU);

    CPU -> rip = * (size_t*) (CPU -> code + CPU -> rip + 1);
    //printf ("jump %lu\n", * (size_t*) (CPU -> code + CPU -> rip + 1));

    //CPU -> rip += sizeof (size_t);

    return 0;
})

