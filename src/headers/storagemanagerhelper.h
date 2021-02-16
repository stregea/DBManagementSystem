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
struct DBStore_S{
    char * db_location;
    char * db_store_location;
    int page_size;
    int buffer_size;
}; typedef struct DBStore_S * DBStore;

/// This will represent a Table within a linked list of tables.
struct Table_S{

    int data_types_size;
    int key_indices_size;
    int page_ids_size;
    int * data_types;
    int * key_indices;
    int * page_ids;
    
}; typedef struct Table_S Table;

#define true 1
#define false 0

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

/**
 * Create a DB Store file to contain the information about the db.
 * @param db_loc - The database location to store.
 * @param page_size - The size of the pages to store.
 * @param buffer_size - The size of the buffer to store.
 */
void createDBStore( char * db_loc,  int page_size, int buffer_size );

/**
 * Create a DBStore struct from an existing dbstore file.
 * Note: The struct returned must be freed using the freeStore() function.
 * @param store_loc - The location of the dbstore file.
 * @returns a struct with the previously created DBStore information.
 */
DBStore readDBStore(char * store_loc);

/*
 * Free the previously allocated memory created for a DBStore.
 */
void freeStore(DBStore store);

Table getTable(int table_id, char * database_path);

int addPageIdToTable(int table_id, int page_id, char * database_path);
#endif /* storagemanagerhelper_h */
