#include <stdlib.h>
#include <string.h>

#include "Onegin.h"

int PtrCmp (const void * arg1, const void * arg2)
{
    String * str1 = *((String**) arg1);
    String * str2 = *((String**) arg2);

    return str1 -> string - str2 -> string;
}

void StringListDtor (StringList * strList)
{
    //qsort(strList -> items, strList -> length, sizeof(String*), PtrCmp);

    size_t length = strList -> length;
    while (length -- > 0)
    {
        String* item = strList -> items [length];

        memset (item -> string, 0, item -> length);
        
        if (length > 0)
        {
            String* preitem = strList -> items [length - 1];
            if (item -> string > preitem -> string + preitem -> length + 1)
                free (item -> string);
        }
        else
            free (item -> string);

        item -> length = 0;
        item -> string = nullptr;
        free (item);
    }
    
    memset (strList -> items, 0, strList -> length * sizeof (String*));
    free (strList -> items);
    strList -> items = nullptr;
    strList -> length = 0;
    free (strList);
}
