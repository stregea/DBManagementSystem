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
#include "../headers/lru-queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// This will represent a Page that will store the table and record entries.
struct Page_S {

    /// the id of the table the page is associated with
    int table_id;

    /// The id/name of a page.
    int page_id;

    /// The current size of the records within the page.
    size_t num_records;

    /// The # of attributes within a record.
    size_t data_types_size;

    /// Pointer used to index to associated pages within a table.
    struct Page_S *nextPage;

    /// array that will be used to contain records.
    union record_item records[];
};
typedef struct Page_S * Page;

/// This will be the buffer to hold all of the pages and the DB location.
struct Buffer_S {

    /// The database location
    char *db_location;

    /// The buffer size
    int buffer_size;

    /// The max size of the pages.
    int page_size;

    /// Integer to keep track of the total number of pages that exist within the db.
    int page_count;

    /// Integer to keep track of the total number of tables that exist within the db.
    int table_count;

    /// int to keep track of the number of pages existing within the buffer.
    int pages_within_buffer;

    /// The cache to keep track of the LRU pages.
    LRU_Cache cache;

    /// Array that will contain the pages
//    Page buffer[];
    Page *buffer;

};
typedef struct Buffer_S * Buffer;
Buffer BUFFER;
#define BUFFER_FILE "buffer_meta"
/**
 * Create a primary key from a row within a table.
 * @param row - The row to create a primary key.
 * @param table - The table containing key information.
 * @return A primary key.
 */
union record_item *get_primary_key(union record_item *row, Table table) {
    union record_item *primary_key = malloc(sizeof(union record_item) * table.key_indices_size);
    for (int i = 0; i < table.key_indices_size; i++) {
        primary_key[i] = row[table.key_indices[i]];
    }
    return primary_key;
}

/**
 * Determine if a pair of keys match.
 * Iterate through each index of the keys and perform a check dependent
 * upon the value of the index of the table's key indices array.
 * @param table - The table containing key information.
 * @param key1 - The first key to compare.
 * @param key2 - The second key to compare
 * @return true (1) if the keys match, otherwise false (-1).
 */
bool keysMatch(Table table, union record_item *key1, union record_item *key2) {
    for (int i = 0; i < table.key_indices_size; i++) {
        switch (table.key_indices[i]) {
            case 0: // integer comparison
                if (key1[i].i < key2[i].i || key1[i].i > key2[i].i) {
                    return false;
                }
                break;
            case 1: // double comparison
                if (key1[i].d < key2[i].d || key1[i].d > key2[i].d) {
                    return false;
                }
                break;
            case 2: // boolean comparison
                if (key1[i].b < key2[i].b || key1[i].b > key2[i].b) {
                    return false;
                }
                break;
            case 3: // char comparison
                if (strcmp(key1[i].c, key2[i].c) < 0) {
                    return false;
                }
                break;
            case 4: // varchar comparison
                if (strcmp(key1[i].v, key2[i].v) < 0) {
                    return false;
                }
                break;
        }
    }
    return true;
}

/**
 * Write a page struct and it's contents to disk.
 * @param page - The page to write.
 */
void write_page_to_disk(Page page) {
    char *database_path = BUFFER->db_location;

    // create path to the table in the database
    char *page_file = appendIntToString("", page->page_id); // this was just used to test we will find this later
    char *page_path = malloc(sizeof(char *) * (strlen(database_path) + strlen(page_file)));
    strcpy(page_path, database_path);
    strcat(page_path, page_file);

    FILE *file = fopen(page_path, "wb");

    // write table id
    fwrite(&page->table_id, sizeof(int), 1, file);

    // write page id
    fwrite(&page->page_id, sizeof(int), 1, file);

    // write the total number of records
    fwrite(&page->num_records, sizeof(size_t), 1, file);

    // write the total number of attrs (5)
    fwrite(&page->data_types_size, sizeof(size_t), 1, file);

    // write all the records to the page.
    fwrite(page->records, sizeof(union record_item), page->num_records, file);

    fclose(file);
    free(page_file);
}

bool bufferIsFull(Buffer buffer) {
    return buffer->pages_within_buffer == buffer->buffer_size;
}

/**
 * Read in a page.
 * Note a Page* that is populated must be freed.
 * @param page_id - The id of the page to read in.
 * @param page - The Page to populate
 * @return EXIT_SUCCESS if page exists, EXIT_FAILURE if page doesn't exist.
 */
int read_page(int page_id, Page *page) {
    // search through all pages
    // if page id isn't found -> return -1
    // else find page with matching page id
    // allocate memory for for struct pointer
    // populate struct from page.
    return -1;
}

/**
 * Create a page.
 */
int write_page(int table_id) {
    int buffer_index;
    Table table = getTable(table_id, BUFFER->db_location);
    Page newPage = &(struct Page_S) {
            .table_id=table_id,
            .page_id=BUFFER->page_count,
            .num_records=0,
            .data_types_size=table.data_types_size // 5 -- may want to just insert that?
    };

    // iterate through list of page id's
    // update the page links?
    if (bufferIsFull(BUFFER)) {
        // point to the LRU index of the buffer
        buffer_index = getLRUIndexForBuffer(BUFFER->cache);

        // purge LRU index of buffer onto disk
        Page pageToWrite = BUFFER->buffer[buffer_index];

        // write pageToWrite to disk.
        write_page_to_disk(pageToWrite);

        // remove memory space used for records
//        free(pageToWrite.records);

        // nullify the index. This shouldn't be necessary, just being safe.
//        BUFFER->buffer[buffer_index] = &(struct Page_S) {};
        BUFFER->buffer[buffer_index] = NULL;

        // create new page at that index
        BUFFER->buffer[buffer_index] = newPage;
    } else {
        buffer_index = BUFFER->pages_within_buffer;
        BUFFER->buffer[buffer_index] = newPage;
        BUFFER->pages_within_buffer++;
    }

    // reference the LRU page.
    referencePage(BUFFER->cache, buffer_index);

    // Add a page id to a table's page id array.
    addPageIdToTable(table_id, newPage->page_id, BUFFER->db_location);

    // increment the total page count
    BUFFER->page_count++;

    return EXIT_SUCCESS;
}

/**
 * Write the Buffer and it's contents to disk.
 * @param filename - The filename to name the buffer file.
 * @param buffer - The buffer to write.
 * @return EXIT_SUCCESS if able to write to disk, EXIT_FAILURE otherwise.
 */
int write_buffer_to_disk(char *filename, Buffer buffer){
    int result = EXIT_SUCCESS;
    char *fileLocation = malloc(sizeof(char*));

    copyStringForFilePath(fileLocation, buffer->db_location);
    strcat(fileLocation, filename);

    FILE *file = fopen(fileLocation, "wb");
    if(file != NULL){
        fwrite(buffer, sizeof(struct Buffer_S), 1, file);
        fclose(file);
    }else{
        result = EXIT_FAILURE;
    }

    return result;
}

/**
 * Read in a meta file to populate the buffer.
 * @param db_location - The location the buffer exists.
 * @param buffer - The Buffer to populate.
 * @return EXIT_SUCCESS if able to populate, EXIT_FAILURE otherwise.
 */
int read_buffer_from_disk(char *db_location, char *filename, Buffer buffer){
    int result = EXIT_SUCCESS;
    buffer = malloc(sizeof(struct Buffer_S));
    char *fileLocation = malloc(sizeof(char*));

    copyStringForFilePath(fileLocation, db_location);
    strcat(fileLocation, filename);
    FILE *file = fopen(fileLocation, "wb");

    if(file != NULL){
        fread(buffer, sizeof(struct Buffer_S), 1, file);
        fclose(file);
    }else{
        result = EXIT_FAILURE;
    }

    return result;
}

/**
 * Free the memory locations the buffer is using.
 * @param buffer - The buffer to free.
 */
void freeBuffer(Buffer buffer){
    freeLRUCache(BUFFER->cache);
    free(buffer->db_location);
    free(buffer->buffer);
    free(buffer);
}

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
int create_database(char *db_loc, int page_size, int buffer_size, bool restart) {
    int result = EXIT_SUCCESS;

    if (restart) {
        result = restart_database(db_loc);
    } else {
        result = new_database(db_loc, page_size, buffer_size);
    }

    return result;
}

/*
 * This function will try to restart the database at the provided database location.
 * @param db_loc - the absolute path for the database to restart.
 * @return 0 if the database is restarted successfully, otherwise -1;
 */
int restart_database(char *db_loc) {
    int result = EXIT_SUCCESS;

    // re-populate buffer with pre-existing info.
    // The buffer file is created when terminate_database() is called.
    result = read_buffer_from_disk(db_loc, BUFFER_FILE, BUFFER);
    BUFFER->buffer = malloc(sizeof(struct Page_S)*BUFFER->buffer_size);

    // would the buffer array need to
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
int new_database(char *db_loc, int page_size, int buffer_size) {
    int result = EXIT_SUCCESS;

    // create db store file to store page and buffer size
    if (isProperSize(page_size, buffer_size)) {

        // initialize buffer
//        BUFFER = &(struct Buffer_S) {};
        BUFFER = malloc(sizeof (struct Buffer_S));

        // delete all contents in db_loc // this doesn't work with windows.
        //clearDirectory(db_loc);

        // set the db location
        BUFFER->db_location = malloc(sizeof(char*)*strlen(db_loc));
        strcpy(BUFFER->db_location, db_loc);

        // Set the max page size
        BUFFER->page_size = page_size;

        // set the max buffer size.
        BUFFER->buffer_size = buffer_size;

        // allocate memory for the buffer that will hold pages.
        // then initialize buffer with null values
//        BUFFER->buffer[buffer_size] = &(struct Page_S) {};
        BUFFER->buffer = malloc(sizeof (struct Page_S)* BUFFER->buffer_size);

        // set null values
        for(int i = 0; i < BUFFER->buffer_size; i++){
            BUFFER->buffer[i] = NULL;
        }

        // set up the cache
        BUFFER->cache = createCache(buffer_size);
    } else { // bad page size or buffer size
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
int get_records(int table_id, union record_item ***table) {
    int result = EXIT_SUCCESS;
    // get table by searching through buffer and grabbing the new page if not found
    // get array of page id's
    // calculate total number of rows
    // cols = 5
    // iterate through all pages, storing each row into table array.
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
int get_page(int page_id, union record_item ***page) {
    int result = EXIT_SUCCESS;
    // not required for phase1.
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
int get_record(int table_id, union record_item *key_values, union record_item **data) {
    int result = EXIT_SUCCESS;

    // get key_indices from table
    // get array of page id's from table
    // iterate through all the rows
    // for each row, create a primary key
    // compare created primary with key_values
    // if primary key matches, store data into data parameter.
    // return -1 if not found
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
int insert_record(int table_id, union record_item *record) {
    int result = EXIT_SUCCESS;
    // get array of page id's from table
    // iterate through pages

    Table table = getTable(table_id, BUFFER->db_location);

    union record_item * insert_key = get_primary_key(record, table);

    // If the table does not have any existing pages, make a new one
    if(table.page_ids_size <= 0)
    {
        // Create a new page, gets added to table
        write_page(table_id);
    }

    Page page = &(struct Page_S) {};

    //Check stuff starting from here

    // search through buffer
    for(int i = 0; i < BUFFER->buffer_size; i++)
    {
        Page temp = BUFFER->buffer[i];

        // Check if page is the first page in the desired table (should exist but don't count on it)
        if(temp->page_id == table.page_ids[0])
        {
            page = temp;
            referencePage(BUFFER->cache, i);
            break; // Will this break from the if or the for loop? Doesn't matter for functionality, just speed
        }
    }

    if (page == NULL) {

    }

    // check if page is null
    // iterate through all pages on disk and assign.
    // if page is still null, return error

    // find first available page with record space. | Need to insert record

    // create 2d array of records

    // iterate through array

    // create primary key from prev_records in page and from record passed in
    // compare the records via primary keys
    // when at desired location insert the record

    // increment page record size

    // if record size > max_record_size
    // split page records
    // create new page and insert other half of pages there
    // resort pages.



    // Psuedocode for algorithm

    // Create primary key array based on record and primary key
    // Starting at the first page in the table, compare to primary key arrays of rows

    // Comparison algorithm (looks like O(n^2) but isn't I promise:
    // Start: pointer to page references first page in table
    // comparing based on first index in primary key

    // while pointer to page is not null:
        // for record in record size:
            // create primary key array for record
            // compare primary key array of record to primary key array of new record
            // if less, move on
            // if more
                // step back one
                // if less again, insert and return 0
                // if equal
                    // if at last index in primary key, return -1 (row already exists)
                    // else move on to comparing based on next index in primary key
            // if equal
                // if at last index in primary key, return -1 (row already exists)
                // else move on to comparing based on next index in primary key
        // if reached here, did not insert yet and did not find existing row that matched
        // if next page exists
            // change pointer to point to next page
        // else insert at end and return 0

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
int update_record(int table_id, union record_item *record) {
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
int remove_record(int table_id, union record_item *key_values) {
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
int drop_table(int table_id) {
    int result = EXIT_SUCCESS;
    // read in array of page id's
    // delete pages with page ids
    // delete table.
    return result;
}

/*
 * This function will clear the table with the provided id
 * from the database. This will remove all data but not the table.
 * @param table_id - the id of the table to clear
 * @return 0 if table successfully cleared, -1 otherwise.
 */
int clear_table(int table_id) {
    int result = EXIT_SUCCESS;
    // read in array of page id's
    // remove associated pages
    // reset table file.
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
int add_table(int *data_types, int *key_indices, int data_types_size, int key_indices_size) {

    // get database path
    char *database_path = BUFFER->db_location;

    // create path to the table in the database
    char *table_id = appendIntToString("", BUFFER->table_count); // this was just used to test we will find this later
    char *table_path = malloc(sizeof(char *) * (strlen(database_path) + strlen(table_id)));
    strcpy(table_path, database_path);
    strcat(table_path, table_id);

    // build the struct for the table
    Table table_content = {
            .data_types_size = data_types_size,
            .key_indices_size = key_indices_size,
            .page_ids_size = 0,
            .key_indices = key_indices,
            .data_types = data_types
    };

    // write the table to disk
    FILE *tableFile = fopen(table_path, "wb");
    fwrite(&table_content, sizeof(table_content), 1, tableFile);
    fclose(tableFile);

    free(table_path);
    free(table_id);

    BUFFER->table_count++;
    return EXIT_SUCCESS;
}



/*
 * This will purge the page buffer to disk.
 * @return 0 on success, -1 on failure.
 */
int purge_buffer() {
    int result = EXIT_SUCCESS;

    // foreach page in buffer
    for(int i = 0; i < BUFFER->buffer_size; i++){
        write_page_to_disk(BUFFER->buffer[i]);
//        BUFFER->buffer[i] = &(struct Page_S) {}; // null out the index
        BUFFER->buffer[i] = NULL;
    }

    BUFFER->pages_within_buffer = 0;

    return result;
}


/*
 * This function will safely shutdown the storage manager.
 * @return 0 on success, -1 on failure.
 */
int terminate_database() {
    int result = EXIT_SUCCESS;
    // purge the buffer
    result = purge_buffer();

    // write buffer info to disk
    result = write_buffer_to_disk(BUFFER_FILE, BUFFER);

    // perform proper memory wipes.
    freeBuffer(BUFFER);
    return result;
}
