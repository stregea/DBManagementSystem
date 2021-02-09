/**
 * CSCI-421 Project: Phase1
 * @file phase1.c
 * Description:
 *      This file serves as the main driver for Phase 1 and will serve as the 'storage manager' which calls functions
 *      implemented within '../c/storagemanager.c'.
 *
 * @author Kyle Collins  (kxc1981@rit.edu)
 * @author Geoffrey Moss (gbm2613@rit.edu)
 * @author Sam Tillinghast  (sft6463@rit.edu)
 * @author Samuel Tregea  (sdt1093@rit.edu)
 */
#include "../headers/storagemanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PAGE_SIZE 10   
#define BUFFER_SIZE 1


int main(int argc, char * argv[]){

    // check that the database path is passed in as an argument to the driver
    if (argc < 2) {
        fprintf(stderr, "usage: %s /<DatabasePath>\n", argv[0]), exit(1);
    }

    char* databasePath = argv[1];
    
    new_database(databasePath, PAGE_SIZE, BUFFER_SIZE);
//    sprintf(number_str, "%d", number);
//    printf("Converted to string : %s\n", appendInt("hello", 999));
    
//    return 0;
    printf("Hideous Jimmies!!\n");
    return EXIT_SUCCESS;
}
