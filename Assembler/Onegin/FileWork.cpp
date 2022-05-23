#include "Onegin.h"

#include <sys/stat.h>

const int STR_MEM_STEP = 100;


StringList * fGetStringList (const char * fname, OneginError * err)
{
    if (err) *err = NO_ERRORS;

    FILE * file = fopen(fname, "rb");
    if (file == nullptr)
    {
        if (err) *err = CANT_OPEN_FILE_ERROR;
        return nullptr;
    }

    struct stat stbuff = {};
    stat (fname, &stbuff);

    off_t buffLength = stbuff.st_size;
    char * buffer = (char*) calloc(buffLength + 1, sizeof(char));

    if (buffLength <= 0)
    {
        if (err) *err = NO_DATA_ERROR;
        return nullptr;
    }

    if (buffer == nullptr)
    {
        if (err) *err = ALLOCATE_ERROR;
        return nullptr;
    }


    fread (buffer, sizeof(char), buffLength, file);
    buffer[buffLength] = '\0';


    fclose (file);


    StringList * strList = (StringList*) calloc(1, sizeof(StringList));
    strList -> items = (String**) calloc(STR_MEM_STEP, sizeof(String*));

    if (strList == nullptr || strList -> items == nullptr)
    {
        if (err) *err = ALLOCATE_ERROR;
        return nullptr;
    }
    strList -> length = STR_MEM_STEP;

    size_t strNum = 0;
    char * strBegin = buffer;
    char * strEnd = buffer;

    do
    {
        /*
        if (*strBegin == '\n')
        {
            *strBegin = '\0';

            if (strBegin != buffer)
                strEnd = ++ strBegin;
        }
        else if (*strBegin == '\0' && strBegin != buffer)
        {
            strEnd = ++ strBegin;
        }
        else if (*strEnd == '\n' || *strEnd == '\0')
        {
            if (*strEnd == '\n') *strEnd == '\0';

            if (strNum >= strList -> length)
            {
                String ** reallocPtr = (String**)
                    realloc( strList -> items, (strList -> length + STR_MEM_STEP) * sizeof(String*));
                if(reallocPtr == nullptr)
                {
                    if (err) *err = ALLOCATE_ERROR;
                    return strList;
                }
                strList -> items = reallocPtr;
                strList -> length += STR_MEM_STEP;
            }
            if ((strList -> items[strNum] = (String*) calloc(1, sizeof(String))) == nullptr)
            {
                if (err) *err = ALLOCATE_ERROR;
                return strList;
            }

            strList -> items[strNum] -> string = strBegin;
            strList -> items[strNum] -> length = strEnd - strBegin;

            strNum ++;

            strBegin = strEnd;
        }
        */

        if (*strEnd == '\n' || *strEnd == '\0')
        {
            *strEnd = '\0';

            if (strNum >= strList -> length)
            {
                String ** reallocPtr = (String**)
                    realloc( strList -> items, (strList -> length + STR_MEM_STEP) * sizeof(String*));
                if(reallocPtr == nullptr)
                {
                    if (err) *err = ALLOCATE_ERROR;
                    return strList;
                }
                strList -> items = reallocPtr;
                strList -> length += STR_MEM_STEP;
            }
            if ((strList -> items[strNum] = (String*) calloc(1, sizeof(String))) == nullptr)
            {
                if (err) *err = ALLOCATE_ERROR;
                return strList;
            }

            strList -> items[strNum] -> string = strBegin;
            strList -> items[strNum] -> length = strEnd - strBegin;

            strNum ++;

            strBegin = strEnd + 1;
        }
    }
    while (++ strEnd <= buffer + buffLength);

    if (strNum < strList -> length)
    {
        String ** reallocPtr = (String**)
            realloc (strList -> items, strNum * sizeof(String*));
        if(reallocPtr == nullptr)
        {
            if (err) *err = ALLOCATE_ERROR;
            return strList;
        }
        strList -> items = reallocPtr;
        strList -> length = strNum;
    }

    return strList;
}


OneginError fPrintStringList (StringList * strList, FILE * file)
{
    if (file == nullptr) file = stdout;

    for (size_t strNum = 0; strNum < strList -> length; strNum ++)
    {
        strList -> items[strNum] -> string[strList -> items[strNum] -> length] = '\n';
        fwrite (strList -> items[strNum] -> string,
                sizeof(char),
                strList -> items[strNum] -> length + 1,
                file);
        strList -> items[strNum] -> string[strList -> items[strNum] -> length] = '\0';
    }

    return NO_ERRORS;
}

