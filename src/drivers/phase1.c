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
#include "../headers/storagemanagerhelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define PAGE_SIZE 4096
#define BUFFER_SIZE 1


int main(int argc, char * argv[]){

    // check that the database path is passed in as an argument to the driver
    if (argc < 2) {
        fprintf(stderr, "usage: %s /<DatabasePath>\n", argv[0]), exit(1);
    }

    char* databasePath = argv[1];

    // create new database
    create_database(databasePath, PAGE_SIZE, BUFFER_SIZE, false);
    
    // add table
    int data_types_size = 3;
    int data_types[] = {3, 0, 1};
    int key_indices_size = 2;
    int key_indices[] = {0, 1};
    int table_id = add_table(data_types, key_indices, data_types_size, key_indices_size);

    // table(name, age, weight)
    union record_item person_1[] = { {.c = "Bob"}, {.i = 31}, {.d = 168.23} };
    union record_item person_6[] = { {.c = "Bob"}, {.i = 31}, {.d = 168.23} };
    union record_item person_2[] = { {.c = "John"}, {.i = 23}, {.d = 180.20} };
    union record_item person_3[] = { {.c = "George"}, {.i = 24}, {.d = 180.20} };
    union record_item person_4[] = { {.c = "Paul"}, {.i = 26}, {.d = 180.20} };
    union record_item person_5[] = { {.c = "Bob"}, {.i = 28}, {.d = 180.20} };

    //insert_record(table_id, person_2);
    insert_record(table_id, person_1);
    //insert_record(table_id, person_6);
    insert_record(table_id, person_3);
    //insert_record(table_id, person_4);
    insert_record(table_id, person_5);




  //  insert_record(table_id, person_1); -- mem leaks in here
   // insert_record(table_id, person_2);

    terminate_database(); // write_buffer_to_disk() crashes on linux
    printf("\nHideous Jimmies!!\n");
    return EXIT_SUCCESS;
}
