/**
 * CSCI-421 Project: Phase1
 * @file storagemanager.c
 * Description:
 *         Header file for ../c/storagemanagerhelper.c. This file allows external functions to be called within storagemanager.c
 *
 * @author Kyle Collins  (kxc1981@rit.edu)
 * @author Geoffrey Moss (gbm2613@rit.edu)
 * @author Sam Tillinghast  (sft6463@rit.edu)
 * @author Samuel Tregea  (sdt1093@rit.edu)
 */
#ifndef storagemanagerhelper_h
#define storagemanagerhelper_h

#include <stdio.h>
#include <stdbool.h>

/**
 * Append an integer to an existing char*.
 * - Note: the newly created string must be freed once used.
 * @param original - The original char pointer text.
 * @param number - The number to append to a char*.
 * @return a newly allocated char* with the appended integer.
 */
char * appendIntToString( char * original, int number );

/**
 * Determine if the page size and buffer size meet the required sizes.
 * @param page_size - The page size to check..
 * @param buffer_size - The buffer size to check..
 * @return true if both page_size and buffer_size are greater than or equal to zero; false otherwise.
 */
bool isProperSize( int page_size, int buffer_size );

/**
 * Go to a specified directory and clear its contents but keep the parent directory..
 * @param dir_name - The directory to clear.
 */
void clearDirectory( char * dir_name );
#endif /* storagemanagerhelper_h */
