/**
 * CSCI-421 Project: Phase1
 * @file storagemanager.c
 * Description:
 *      This file contains the function definitions defined in '../headers/storagemanager.h'.
 *      This file will serve as the interface to the database, ultimately allowing for a client program to connect
 *      and interact with the database and serve as the 'storage manager'.
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
#include <dirent.h>

#define PAGE_NAME "page"

/// This will represent a Table within a linked list of tables.
struct Table_Node{
    int * primary_key;
    
    int * data_types;
    
    /// The id of a table.
    int table_id;
    
    int data_types_size;
    int key_indices_size;
    /// pointer for a table that may exist on another page.
    struct Table_Node * nextTable;
    
    /// array that will be used to contain records.
    union record_item records[];
}; typedef struct Table_Node * Table;

/// This will represent a Page that will store the table and record entries.
struct Page_S{
    
    /// The id of a page.
    int page_id;
    
    /// The current size of the records within the page.
    size_t record_size;
    
    /// The max size of the page.
    size_t max_record_size;
    
    /// Linked list of tables.
    Table head;
}; typedef struct Page_S Page;

/// This will be the buffer to hold all of the pages and the DB location.
struct Buffer_S{
    
    /// The database location
    char* db_location;
    
    /// Array that will contain the pages
    Page* buffer;
}; typedef struct Buffer_S Buffer;

Buffer BUFFER;
int PAGE_INDEX = 0;
int TABLE_ID = 0;

/*
 * Create or restarts an instance of the database at the
 * provided database location.
 * If restart is true this function will call the restart_database function.
 * If restart is false this function will call the new database function.
 * @param db_loc - the absolute path for the database.
 * @param page_size - the pagezs size for the database.
 * @param buffer_size - the maximum number of pages the
 *                      database can hold in its page buffer at one time.
 * @param restart - if true it will attempt to restart the database at the
 *                  provided database location.
 * @return the result of either the restart_database or new_database function calls.
 */
int create_database( char * db_loc, int page_size, int buffer_size, bool restart){
    int result = EXIT_SUCCESS;
    
    if( restart ){
        result = restart_database( db_loc );
    }else{
        result = new_database( db_loc, page_size, buffer_size );
    }
    
    return result;
}

/*
 * This function will try to restart the database at the provided database location.
 * @param db_loc - the absolute path for the database to restart.
 * @return 0 if the database is restarted successfully, otherwise -1;
 */
int restart_database( char * db_loc ){
    int result = EXIT_SUCCESS;
    PAGE_INDEX = 0;
    
    return result;
}

/*
 * Create an instance of the database at the provided database location.
 * @param db_loc - the absolute path for the database.
 * @param page_size - the pages size for the database.
 * @param buffer_size - the maximum number of pages the
 *                      database can hold in its page buffer at one time.
 * @return 0 if the database is started successfully, otherwise -1;
 */
int new_database( char* db_loc, int page_size, int buffer_size ){
    int result = EXIT_SUCCESS;
    
    clearDirectory(db_loc);
    // delete all contents in db_loc
    // create db store file to store page and buffer size
    if( isProperSize( page_size, buffer_size ) ){
        PAGE_INDEX = 0;
        // initialize a buffer
        Page tmpBuffer[buffer_size];
        
        // initialize a page
        Page new_page = { .page_id=0, .record_size=0, .max_record_size=page_size, .head=NULL };
        
        tmpBuffer[PAGE_INDEX] = new_page;
        
        // set the buffer to point to a location in memory.
        BUFFER.db_location = db_loc;
        BUFFER.buffer = tmpBuffer;
        
    }else{ // bad page size or buffer size
        result = EXIT_FAILURE;
    }
    
    return result;
}

/*
 * Returns all of the records in the table with the provided id.
 * The records must be in order that they are stored on hardware.
 * The user of this function is responsible for freeing the data.
 * @param table_id - the id of the table to get the records for.
 * @param table - a 2d array of record_item (output variable)
 *                this will be used to output the values in the table.
 This will be a pointer to the first item in the 2d array.
 * @return the number of records in the output, -1 upon error
 */
int get_records( int table_id, union record_item *** table ){
    int result = EXIT_SUCCESS;
    return result;
}

/*
 * Returns all of the records in the page with the provided id.
 * The records must be in order that they are stored on hardware.
 * The user of this function is responsible for freeing the data.
 * @param page_id - the id of the page to get the records for.
 * @param page - a 2d array of record_item (output variable)
 *                this will be used to output the values in the page.
 This will be a pointer to the first item in the 2d array.
 * @return the number of records in the output, -1 upon error
 */
int get_page( int page_id, union record_item *** page ){
    int result = EXIT_SUCCESS;
    return result;
}

/*
 * This function will return a record from the table with the provided
 * id that matched the key_values provided.
 * @param table_id - the id of the table to find the record in.
 * @param key_values - an array of record_items that make up the key of the
 record to be found. The order of the values matches
 the primary key indices order of the table.
 * @param data - a pointer to an array of record_item values that represent the tuple matching
 the key values provided. (output parameter)
 The user is responsible for freeing this.
 * @return 0 if successful, -1 otherwise
 */
int get_record( int table_id, union record_item * key_values, union record_item ** data ){
    int result = EXIT_SUCCESS;
    return result;
}

/*
 * Inserts the provided record into the table with the provided id.
 * Records must be inserted in primary key order.
 * Two records in a table cannot have the same primary key or unique constraints.
 * @param table_id - the id of the table to insert the record into.
 * @param record - the record to insert into the table.
 * @return 0 if successfully inserted, -1 otherwise
 */
int insert_record( int table_id, union record_item * record ){
    int result = EXIT_SUCCESS;
    
    // be sure to increment page record size here.
    
    // if record size > max_record_size
    // create new page and insert information there
    
    return result;
}

/*
 * Updates the provided record into the table with the provided id.
 * Primary key of a record will never be updated.
 * Two records in a table cannot have the same primary key or unique constraints.
 * @param table_id - the id of the table to update the record in.
 * @param record - the record to update in the table.
 * @return 0 if successfully updated, -1 otherwise
 */
int update_record( int table_id, union record_item * record ){
    int result = EXIT_SUCCESS;
    return result;
}

/*
 * Removes the provided record from the table with the provided id.
 * @param table_id - the id of the table to remove the record from.
 * @param key_values - an array of record_items that make up the key of the
 record to be removed.
 * @return 0 if successfully removed, -1 otherwise
 */
int remove_record( int table_id, union record_item * key_values ){
    int result = EXIT_SUCCESS;
    return result;
}


/*
 * This function will drop the table with the provided id
 * from the database. This will remove all data as well as information
 * about the table.
 * @param table_id - the id of the table to drop
 * @return 0 if table successfully dropped, -1 otherwise.
 */
int drop_table( int table_id ){
    int result = EXIT_SUCCESS;
    return result;
}

/*
 * This function will clear the table with the provided id
 * from the database. This will remove all data but not the table.
 * @param table_id - the id of the table to clear
 * @return 0 if table successfully cleared, -1 otherwise.
 */
int clear_table( int table_id ){
    int result = EXIT_SUCCESS;
    return result;
}

/*
 * This will add a table to the database with the provided data types and
 * primary key.
 * @param data_types - an integer array representing the data types stored
 in a tuple in the table.
 * @param key_indices - an integer array representing the indices that
 make up the primary key. The order of the indices
 in this array determine the ordering of the attributes
 in the primary key.
 * @param data_types_size - the size of the data types array
 * @param key_indices_size - the size of the key indices array.
 * @return the id of the table created, -1 upon error.
 */
int add_table( int * data_types, int * key_indices, int data_types_size, int key_indices_size ){
    Table cursor = BUFFER.buffer[PAGE_INDEX].head;
    // move to next available table slot
    while(cursor->nextTable){
        cursor = cursor->nextTable;
    }
    
    // creating the new table.
    Table newTable = malloc(sizeof(struct Table_Node) + data_types_size + key_indices_size);
    
    // not to sure how much memory to allocate atm (due to my rustiness of C), that's why one line variation is commented out.
    
    // need to free these later on within the clear_table() and drop_table() functions (or db shutdown/restart) to prevent memory leaks
    newTable->primary_key = malloc(key_indices_size);
    //    newTable->primary_key = malloc(key_indices_size * sizeof(int));
    newTable->data_types = malloc(data_types_size);
    //    newTable->data_types = malloc(data_types_size * sizeof(int));
    
    // store the values into memory
    memcpy(newTable->primary_key, key_indices, key_indices_size); // store the primary key
//    memcpy(newTable->primary_key, key_indices, key_indices_size * sizeof(int));
    memcpy(newTable->data_types, data_types, data_types_size); // store the data types
//    memcpy(newTable->data_types, data_types, data_types_size * sizeof(int));
    
    newTable->table_id = ++TABLE_ID;
        
    cursor->nextTable = newTable;
    return TABLE_ID;
}

/*
 * This will purge the page buffer to disk.
 * @return 0 on success, -1 on failure.
 */
int purge_buffer(){
    int result = EXIT_SUCCESS;
    return result;
}

/*
 * This function will safely shutdown the storage manager.
 * @return 0 on success, -1 on failure.
 */
int terminate_database(){
    int result = EXIT_SUCCESS;
    return result;
}
