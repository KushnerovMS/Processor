#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <stdlib.h>
#include "Stack/Stack.h"


const unsigned REGISTERS_COUNT          = 8;
const unsigned CASH_SIZE                = 256;
const unsigned INITIAL_STACK_CAPACITY   = 8;

#include "../Commands.h"

struct CPU
{
    unsigned char* code;
    size_t rip;
    Stack* stack;
    arg_type reg[REGISTERS_COUNT];
    arg_type cash[CASH_SIZE];
};

#define CPU struct CPU

#include "../Commands.h"

#undef CPU

void runProgram (char * InFileName);

arg_type getArg (CPU* CPU);
arg_type* getArgPtr (CPU* CPU);

#endif
