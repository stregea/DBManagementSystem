/**
 * CSCI-421 Project: Phase1
 * @file storagemanager.c
 * Description:
 *      This file allows external functions to be called within storagemanager.c
 *
 * @author Kyle Collins  (kxc1981@rit.edu)
 * @author Geoffrey Moss (gbm2613@rit.edu)
 * @author Sam Tillinghast  (sft6463@rit.edu)
 * @author Samuel Tregea  (sdt1093@rit.edu)
 */
#include "../headers/storagemanagerhelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

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

/**
 * Determine if the page size and buffer size meet the required sizes.
 * @param page_size - The page size to check..
 * @param buffer_size - The buffer size to check..
 * @return true if both page_size and buffer_size are greater than or equal to zero; false otherwise.
 */
bool isProperSize( int page_size, int buffer_size ){
    return buffer_size >=0 && page_size >=0;
}

/**
 * Go to a specified directory and clear its contents but keep the parent directory..
 * @param dir_name - The directory to clear.
 */
void clearDirectory( char * dir_name ){
    char * directory = malloc(strlen(dir_name)+2);
    
    strcat(directory, dir_name);
    
    // check if there is a '/' appended on the
    if(dir_name[strlen(dir_name)-1] != '/'){
        strcat(directory, "/*/"); // need to test this on windows
    }
    
    // build the command
    char * command = malloc(strlen("exec rm -rf ")+strlen(directory));
    strcat(command, "exec rm -rf ");
    strcat(command, directory);
    
    // execute the command
    system(command);
    
    free(directory);
    free(command);
}
