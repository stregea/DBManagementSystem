/**
 * CSCI-421 Project: Phase1
 * @file storagemanager.c
 * Description:
 *      This file allows for string operations to perform that aren't native to the C language.
 *
 * @author Kyle Collins  (kxc1981@rit.edu)
 * @author Geoffrey Moss (gbm2613@rit.edu)
 * @author Sam Tillinghast  (sft6463@rit.edu)
 * @author Samuel Tregea  (sdt1093@rit.edu)
 */
#include "../headers/stringhelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Append an integer to an existing char*.
 * Note: the newly created string must be freed once used.
 * @param original - The original char pointer text.
 * @param number - The number to append to a char*.
 * @return a newly allocated char* with the appended integer.
 */
char * appendIntToString( char * original, int number )
{
    char * newString = malloc( sizeof( char* ) );
    sprintf(newString, "%s%d", original, number);
    return newString;
}
