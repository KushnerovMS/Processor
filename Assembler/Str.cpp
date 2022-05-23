#include "Assembler.h"

#include <string.h>

char * mkAnotherFormatFileName (const char * fname, const char * format)
{
    int fnameLen = strlen (fname);
    int newFnameLen = fnameLen - 1;

    for (; newFnameLen >= 0 && fname[newFnameLen] != '\\' && fname[newFnameLen] != '/'; newFnameLen --)
    {
        if (fname[newFnameLen] == '.')
            break;
    }

    char* newFname = (char*) calloc (newFnameLen + strlen (format) + 1, sizeof (char));

    //if (fnameLen - strlen (newFname) < strlen (format))
    //    newFname = (char*) realloc (newFname, (strlen (newFname) + strlen (format) + 1) * sizeof (char));

    sprintf (newFname, "%.*s%s", newFnameLen, fname, format);

    return newFname;
}
