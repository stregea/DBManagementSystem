/**
 * CSCI-421 Project
 * @file stringhelpers.h
 * Description:
 *      Header file for ../c/stringhelpers.c. This file allows for string operations to perform
 *      that aren't native to the C language.
 *
 * @author Kyle Collins  (kxc1981@rit.edu)
 * @author Geoffrey Moss (gbm2613@rit.edu)
 * @author Sam Tillinghast  (sft6463@rit.edu)
 * @author Samuel Tregea  (sdt1093@rit.edu)
 */
#ifndef stringhelpers_h
#define stringhelpers_h

/**
 * Append an integer to an existing char*.
 * - Note: the newly created string must be freed once used.
 * @param original - The original char pointer text.
 * @param number - The number to append to a char*.
 * @return a newly allocated char* with the appended integer.
 */
char * appendIntToString( char * original, int number );
#endif
