/**
 * CSCI-421 Project: Phase1
 * @file storagemanager.c
 * Description:
 *         Header file for ../c/boolhelpers.c. This file creates functions for boolean operations
 *         that will be used multiple times throughtout the project.
 *
 * @author Kyle Collins  (kxc1981@rit.edu)
 * @author Geoffrey Moss (gbm2613@rit.edu)
 * @author Sam Tillinghast  (sft6463@rit.edu)
 * @author Samuel Tregea  (sdt1093@rit.edu)
 */
#ifndef boolhelpers_h
#define boolhelpers_h

#include <stdio.h>
#include <stdbool.h>
#endif

/**
 * Determine if the page size and buffer size meet the required sizes.
 * @param page_size - The page size to check..
 * @param buffer_size - The buffer size to check..
 * @return true if both page_size and buffer_size are greater than or equal to zero; false otherwise.
 */
bool isProperSize( int page_size, int buffer_size );
