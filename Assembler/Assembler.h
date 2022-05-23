#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#define ASSEMBLER 1

#include <stdlib.h>

#include "../Commands.h"
#include "Onegin/Onegin.h"


const char COMPILE_FILE_FORMAT[] = ".bin";
const char DECOMPILE_FILE_FORMAT[] = ".as";

const int MAX_COMMAND_SIZE = sizeof (char) + sizeof (char) + sizeof (arg_type);
const int MAX_ARGUMENT_SIZE = 100;

void Compile (char * fname);
void Decompile (char * fname);

char * mkAnotherFormatFileName (const char * fname, const char * format);

#endif
