#ifndef DEF_COMMAND

#ifdef ASSEMBLER
#define DEF_COMMAND(HARG, CODE, NAME, DEFINITION) {HARG, CODE, #NAME},
#endif

#ifdef CPU

#define DEF_COMMAND(HARG, CODE, NAME, DEFINITION) int NAME (CPU*);
#include "CPU/Commands.cpp"
#undef DEF_COMMAND


#define DEF_COMMAND(HARG, CODE, NAME, DEFINITION) {HARG, CODE, NAME},

#endif

#endif


#ifndef arg_type

typedef double arg_type;
#define arg_format "lf"

#endif


//structure of command code
/*
                                                    *
          |  command haven't argument               *
          |                                         *
          |  actually, code of command              *
          |  _________________________              *
          | /                         \             *
[ ]       | [ ] [ ] [ ] [ ] [ ] [ ] [ ]             *
 ^        |  ^   ^   ^  \_____________/             *
command   |  c   r   c     actually,                *
work with |  a   e   o  code of Command             *
arguments |  s   g   n                              *
          |  h   i   s                              *
(1 - work |      s   t                              *
          |      t                                  *
 0 - does |      e                                  *
     not  |      r                                  *
     work)|                                         *
          |  command have argument                  *
                                                    *
*/

#ifndef COMMAND_STRUCTURE
#ifdef DEF_COMMAND

#define COMMAND_STRUCTURE

typedef struct
{
    unsigned char harg;
    unsigned char code;
#ifdef ASSEMBLER
    const char * name;
#endif
#ifdef CPU
    int (*cmd)(CPU*);
#endif
}
Command;


const Command Commands[] =
{
    #include "CPU/Commands.cpp"
};

const int CommandsCount = sizeof (Commands) / sizeof (Command);



#endif
#endif
