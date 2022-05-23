//!@file   Onegin.h
//!@brief  Library, that working with poems.
//!
//!Onegin can work with poems:
//!     - read file and write array of strings
//!     - sort strings from beginning or ending
//!     - write array of strings in file
//!
//-----------------------------------------------------------------------------------------------------------
//!
//!@defgroup Errors Errors.
//!


#ifndef ONEGIN_H_INCLUDED
#define ONEGIN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

//!@ingroup     Errors.
//!@brief       Possible error cods.
enum OneginError
{
    NO_ERRORS            = 0, //!< successful execution of the function.
    CANT_OPEN_FILE_ERROR = 1, //!< the requested file cannot be opened.
    NO_DATA_ERROR        = 2, //!< empty file error.
    ALLOCATE_ERROR       = 3  //!< error in the memory function.
};


//!@brief   String.
//!
//!The structure stores an array of chars and it's length without '\0'.
typedef struct
{
    size_t length; //!< length of string.
    char * string; //!< actually string.
}
String;

//!@brief   List of strings.
//!
//!The structure stores an array of pointers to string and it's count.
typedef struct
{
    size_t length;  //!< count of string pointers in list.
    String ** items;//!< array of string pointers.
}
StringList;

//!
//!@brief   Get string list from file.
//!
//!@param   [in]    fname   name of file to read.
//!@param   [out]   err     parameter to write error of the function.
//!@note    if don't put the <b>err</b>, the error isn't write anywhere.
//!
//!@return  list of strings from file, created by calloc.
//!
//!Function read file with name fname and write all strings in file in string list.
//!Every string ends on '\0'.
//!
StringList * fGetStringList (const char * fname, OneginError * err = NULL);

//!
//!@brief   Print string list in file.
//!
//!@param   [in]  file     file to write in.
//!@param   [in]  strList  list of string for writing in file.
//!
//!@return  Code of error.
//!
//!Function print in oder all strings from <b>strlist</b> into <b>file</b>.
//!
OneginError fPrintStringList (StringList * strList, FILE * file);


//!
//!@brief   String list destructor
//!
//!@param   [in]  strList  pointer to string list that must be destructed
//!
//!Function completely frees each string of string list, array of pointers tos strings, and actually string list.
//!
//!@warning To avoid errors all fields of <b>strList</b> should be created by <b> malloc </b> , <b>calloc</b> or <b>realloc</b>.
void StringListDtor (StringList * strList);

//!
//!@brief   String list destructor
//!
//!@param   [in]  strList  pointer to string list that must be destructed
//!
//!Function completely destruct string list, created by @link fGetStringList @endlink .
//void FGetStringListDtor (StringList * strList);

//!@defgroup RuCharTipe Englis-russian functions from ctype.h.
//!Some analogs of ctype.h functions, that can work with english and russian letters.
//!@{
//bool isAlpha (const char c);
//char toLower (const char c);
//char toUpper (const char c);
//!@}

//!@brief   Check if string is informative.
//!
//!@note    If the string has at least one letter, this string is informative.
//!
//!@param   [in]  str  string to check.
//!
//!@return  True(1) or false(0).
//bool isInformativeString (const String * str);

//!@defgroup Comparators Comparators
//!
//!@brief   Compare two arguments
//!
//!@param   [in] arg1, arg2 @link String @endlink pointers to compare.
//!@return  difference between arguments
//!
//!Comparators compare two pointers to @link String @endlink . Comparators are used in qsort().

//!@{

//!@brief   Alphabet comparator.
//!@return  Difference of first various letters from beginning of string
//int AlphabetCmp (const void * arg1, const void * arg2);
//!@brief   Rhyming comparator.
//!@return  Difference of first various letters from ending of string
//int RhymingCmp (const void * arg1, const void * arg2);
//!@brief   Pointer comparator.
//!@return  Difference of pointers of string in Strings.
//int PtrCmp (const void * arg1, const void * arg2);
//!@}

//!@brief Quick sorter.
//!My analog of qsort from stdlib.h.
//void myqsort (void* base, size_t n, size_t size, int (*cmp)(const void*, const void*));

#endif
