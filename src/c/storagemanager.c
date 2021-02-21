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
#include <math.h>

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
    union record_item **records;
};
typedef struct Page_S *Page;

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
typedef struct Buffer_S *Buffer;
Buffer BUFFER;
#define BUFFER_FILE "buffer_meta";
#define TABLE_FILE "table_";
#define COMPARE_EQUALS 0
#define COMPARE_LESS_THAN -1
#define COMPARE_GREATER_THAN 1

/**
 * Create a primary key from a row within a table.
 * @param row - The row to create a primary key.
 * @param table - The table containing key information.
 * @return A primary key.
 */
union record_item *get_primary_key(union record_item *row, Table table) {

    printf("getting primary key\n");
    union record_item *primary_key = malloc(sizeof(union record_item) * table.key_indices_size);
    for (int i = 0; i < table.key_indices_size; i++) {
        int location = -1;
        for (int j = 0; j < table.data_types_size; j++) {
            if (table.data_types[j] == table.key_indices[i]) {
                location = j;
                break;
            }
        }

        if (location < 0) {
            // couldn't find the right data type in the list, key cannot be created
            return NULL;
        }

        primary_key[i] = row[location];
    }
    printf("returning primary key: [%g, %s]\n", primary_key[0].d, primary_key[1].c);
    return primary_key;
}

/**
 * Determine if a pair of rows match, based on the primary key defined by the table
 * Iterate through each index of the keys and perform a check dependent
 * upon the value of the index of the table's key indices array.
 * @param table - The table containing key information.
 * @param row1 - The first row to compare.
 * @param row2 - The second row to compare
 * @return -1, 0, or 1 depending on if the first row should come before, is equal to, or should come after the second
 */
int compare(Table table, union record_item *row1, union record_item *row2) {

    //printf("comparing\n");

    // index through both keys then perform the selected operation to check.
    for (int i = 0; i < table.key_indices_size; i++) {
        printf("comparing keys: %d\n", table.key_indices[i]);

        int location = -1;
        // find where the desired data type is in the row
        for (int j = 0; j < table.data_types_size; j++) {
             if (table.data_types[j] == table.key_indices[i]) {
                 location = j;
                 break;
             }
        }
        if (location < 0) {
            // Desired primary key attribute does not exist in domain for table
            return -1;
        }
        switch (table.key_indices[i]) {
            case 0: // integer comparison
                if (row1[location].i < row2[location].i) {
                    return -1;
                } else if (row1[location].i == row2[location].i) {
                    break;
                } else {
                    return 1;
                }
            case 1: // double comparison
                if (row1[location].d < row2[location].d) {
                    return -1;
                } else if (row1[location].d == row2[location].d) {
                    break;
                } else {
                    return 1;
                }
            case 2: // boolean comparison
                // booleans can only be true or false, but false (0) is less than true (1)
                if (row1[location].b < row2[location].b) {
                    return -1;
                } else if (row1[location].b == row2[location].b) {
                    break;
                } else {
                    return 1;
                }
            case 3: // char comparison
                    // check if < or > than 0
                printf("row1: %s, row2: %s\n", row1[location].c, row2[location].c);
                if (strcmp(row1[location].c, row2[location].c) < 0) {
                    return -1;
                } else if (strcmp(row1[location].c, row2[location].c) == 0) {
                    break;
                } else {
                    return 1;
                }
            case 4: // varchar comparison
                if (strcmp(row1[location].v, row2[location].v) < 0) {
                    return -1;
                } else if (strcmp(row1[location].v, row2[location].v) == 0) {
                    break;
                } else {
                    return 1;
                }
            default:
                // This will cause it to reject the record, thinking they are equal
                return 0;
        }
    }
    return 0;
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

    if(file != NULL){
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
    }

    free(page_path);
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
    // allocate memory for for struct pointer
    // populate struct from page.
    return -1;
}

/**
 * Create a page.
 * param - page_index location to place page id into page_id array
 *
 * @return the page_id of the newly created page
 */
int create_page(int table_id, int page_index) {

    int buffer_index;
    Table table;
    table = getTable(table_id, BUFFER->db_location);

    Page newPage = malloc(sizeof(struct Page_S));
    newPage->table_id = table_id;
    newPage->page_id = BUFFER->page_count;
    newPage->num_records = 0;
    newPage->records = malloc(BUFFER->page_size);
    newPage->data_types_size = table.data_types_size;
    newPage->nextPage = NULL;

    printf("page_id: %d\n", newPage->page_id);


    // iterate through list of page id's
    // update the page links?
    if (bufferIsFull(BUFFER)) {
        printf("BUFFER FULL: ");
        // point to the LRU index of the buffer
        buffer_index = getLRUIndexForBuffer(BUFFER->cache);

        // purge LRU index of buffer onto disk
        Page pageToWrite = BUFFER->buffer[buffer_index];

        // write pageToWrite to disk.
        write_page_to_disk(pageToWrite);

        // remove memory space used for records
        // free(pageToWrite.records);

        // nullify the index. This shouldn't be necessary, just being safe.
        // BUFFER->buffer[buffer_index] = &(struct Page_S) {};
        BUFFER->buffer[buffer_index] = NULL;

        // create new page at that index
        BUFFER->buffer[buffer_index] = newPage;
    } else {
        buffer_index = BUFFER->pages_within_buffer;
        printf("buffer index?: %d\n", buffer_index);
        BUFFER->buffer[buffer_index] = newPage;
        BUFFER->pages_within_buffer++;
    }

    printf("Wrote page to buffer...\n");

    // reference the LRU page.
    // tell LRU we used the page
    referencePage(BUFFER->cache, buffer_index);

    // Add a page id to a table's page id array.
    addPageIdToTable(table_id, newPage->page_id, BUFFER->db_location, page_index);

    // increment the total page count
    BUFFER->page_count++;

    return newPage->page_id;
}

Page load_page(int page_id) {

    //printf("current num records: %zu\n", BUFFER->buffer[0]->num_records);

    Page page = NULL;
    for (int i = 0; i < BUFFER->pages_within_buffer; i++) {
        //printf("checking page id: %d\n", BUFFER->buffer[i]->page_id);
        //printf("looking for page id: %d\n", page_id);
        if (BUFFER->buffer[i]->page_id == page_id) {
            page = BUFFER->buffer[i];
            break;
            //referencePage(BUFFER->cache, i);
        }
    }

    //printf("", page->)

    if (page != NULL) {
        return page;
    }

    //TODO read page from memory if not found
    //check buffer
    //call read_page
    //add page to buffer
    //update LRU
}

/**
 * Write the Buffer and it's contents to disk.
 * @param filename - The filename to name the buffer file.
 * @param buffer - The buffer to write.
 * @return EXIT_SUCCESS if able to write to disk, EXIT_FAILURE otherwise.
 */
int write_buffer_to_disk(char *filename, Buffer buffer) {
    int result = EXIT_SUCCESS;
    char *fileLocation = malloc(sizeof(char *));

    copyStringForFilePath(fileLocation, buffer->db_location);
    strcat(fileLocation, filename);

    FILE *file = fopen(fileLocation, "wb");
    if (file != NULL) {
        fwrite(buffer, sizeof(struct Buffer_S), 1, file);
        fclose(file);
    } else {
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
int read_buffer_from_disk(char *db_location, char *filename, Buffer buffer) {
    int result = EXIT_SUCCESS;
    buffer = malloc(sizeof(struct Buffer_S));
    char *fileLocation = malloc(sizeof(char *));

    copyStringForFilePath(fileLocation, db_location);
    strcat(fileLocation, filename);
    FILE *file = fopen(fileLocation, "wb");

    if (file != NULL) {
        fread(buffer, sizeof(struct Buffer_S), 1, file);
        fclose(file);
    } else {
        result = EXIT_FAILURE;
    }

    return result;
}

/**
 * Free the memory locations the buffer is using.
 * @param buffer - The buffer to free.
 */
void freeBuffer(Buffer buffer) {
    freeLRUCache(BUFFER->cache);
    free(buffer->db_location);
    for(int i = 0; i < buffer->buffer_size; i++){
        free(buffer->buffer[i]);
    }
    free(buffer->buffer);
    free(buffer);
}

/**
 * Free the memory location a page pointer is using.
 * @param page - The page to free.
 */
void freePage(Page page) {
    free(page->nextPage);
    free(page->records);
    free(page);
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

    if (result == EXIT_SUCCESS) {
        printf("Success!\n\n");
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
    printf("Restarting Database...\n");

    // re-populate buffer with pre-existing info.
    // The buffer file is created when terminate_database() is called.
    char *buffer_file = BUFFER_FILE;
    result = read_buffer_from_disk(db_loc, buffer_file, BUFFER);
    BUFFER->buffer = malloc(sizeof(struct Page_S) * BUFFER->buffer_size);

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
    printf("Create New Database...\n");

    // create db store file to store page and buffer size
    if (isProperSize(page_size, buffer_size)) {

        // initialize buffer
        BUFFER = malloc(sizeof(struct Buffer_S));

        // delete all contents in db_loc 
        // this doesn't work with windows needs to be fix
        //clearDirectory(db_loc);

        // set the db location
        BUFFER->db_location = malloc(sizeof(char *) * strlen(db_loc));
        strcpy(BUFFER->db_location, db_loc);

        // Set the max page size
        BUFFER->page_size = page_size;

        // set the max buffer size.
        BUFFER->buffer_size = buffer_size;

        // allocate memory for the buffer that will hold pages.
        // then initialize buffer with null values
        BUFFER->buffer = malloc(sizeof(struct Page_S) * BUFFER->buffer_size);

        // set null values
        for (int i = 0; i < BUFFER->buffer_size; i++) {
            BUFFER->buffer[i] = malloc(sizeof(struct Page_S));
        }

        BUFFER->page_count = 0;
        BUFFER->pages_within_buffer = 0;
        BUFFER->table_count = 0;


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

    Table table = getTable(table_id, BUFFER->db_location);

    if (table.page_ids <= 0) {
        // can't have any data without pages
        return -1;
    }

     Page current_page = load_page(table.page_ids[0]);

    while (current_page != NULL) {

        for (int i = 0; i < current_page->num_records; i++) {
            union record_item *test_values = get_primary_key(current_page->records[i], table);

            bool matches = true;

            for (int j = 0; j < table.key_indices_size; j++) {
                switch (table.key_indices[i]) {
                    case 0:
                        if (test_values[j].i != key_values[j].i) matches = false;
                        break;
                    case 1:
                        // double
                        if (test_values[j].d != key_values[j].d) matches = false;
                        break;
                    case 2:
                        // bool
                        if (test_values[j].b != key_values[j].b) matches = false;
                        break;
                    case 3:
                        // char
                        if (strcmp(test_values[j].c, key_values[j].c) != 0) matches = false;
                        break;
                    case 4:
                        // varchar
                        if (strcmp(test_values[j].v, key_values[j].v) != 0) matches = false;
                        break;
                    default:
                        return -1;
                }
            }

            if (matches) {
                data = &current_page->records[i];
                return 0;
            }
        }
        current_page = current_page->nextPage;
    }

    // get key_indices from table
    // get array of page id's from table
    // iterate through all the rows
    // for each row, create a primary key
    // compare created primary with key_values
    // if primary key matches, store data into data parameter.
    // return -1 if not found
    return -1;
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
    printf("Inserting record into table_%d: [%s, %d, %g]\n", table_id, record[0].c, record[1].i, record[2].d);

    Table table = getTable(table_id, BUFFER->db_location);

    int current_page_id = -1;
    Page current_page = NULL;

    // If the table does not have any existing pages, make a new one
    if (table.page_ids_size <= 0) {

        printf("making new page\n");
        // Create a new page, gets added to table
        current_page_id = create_page(table_id, 0);
        printf("current_page_id: %d\n", current_page_id);
    } else {
        current_page_id = table.page_ids[0];
        printf("found existing page: %d\n", current_page_id);
    }

    // get the desired page either from the buffer or from memory
    current_page = load_page(current_page_id);

    //union record_item *insert_key = get_primary_key(record, table);
    while (current_page != NULL) {
        printf("number of records on page %d: %zu\n", current_page_id, current_page->num_records);
        for (int i = 0; i < current_page->num_records; i++) {

            printf("checking against record %d\n", i);

            // create primary key array for record
            // compare primary key array of record to primary key array of new record
            //union record_item *temp_key = get_primary_key(current_page->records[i], table);

            int comparison = compare(table, record, current_page->records[i]);

            printf("comparison: %d\n", comparison);

            // if equal, stop and don't insert the record
            if (comparison== 0) {
                printf("equal, not inserting\n\n");
                freeTable(table);
                return -1;
            }
            // if more
            if(comparison < 0) {
                // does the page have room to fit the record we're about to add?
                size_t size_after_adding = (current_page->num_records + 1) * table.data_types_size * sizeof(union record_item);
                printf("page size after adding: %zu\n", size_after_adding);
                if (size_after_adding > BUFFER->page_size) {
                    printf("out of space in page\n");

                    // Get index after last item in first half of page, keeping more items in first half when necessary
                    int half = floor(((double) current_page->num_records + 1) / 2.0);
                    printf("index after first half: %d\n", half);

                    int current_page_index = -1;

                    for (int j = 0; j < table.page_ids_size; j++) {
                        if (table.page_ids[j] == current_page_id) {
                            current_page_index = j;
                        }
                    }
                    if (current_page_index < 0) {
                        // should not happen
                        return -1;
                    }

                    int new_page_id = create_page(table_id, current_page_index + 1);

                    // tables are created with exactly enough space to hold their current page ids
                    realloc(table.page_ids, (table.page_ids_size + 1) * sizeof(int));

                    for (int j = table.page_ids_size - 1; j > current_page_index; j--) {
                        table.page_ids[j+1] = table.page_ids[j];
                    }

                    // update page_id list and size of list
                    table.page_ids[current_page_index + 1] = new_page_id;
                    table.page_ids_size = table.page_ids_size + 1;

                    Page new_page = load_page(new_page_id);

                    // copy second half of records over to new page
                    for (int j = half; j < current_page->num_records; j++) {
                        new_page->records[j - half] = current_page->records[j];
                        current_page->records[j] = NULL;
                    }

                    // update next page references
                    new_page->nextPage = current_page->nextPage;
                    current_page->nextPage = new_page;

                    // figure out where to put new record after split
                    int new_pos = -1;
                    Page adding_to = NULL;
                    if (i < half) {
                        // insert on current page
                        new_pos = i;
                        adding_to = current_page;
                    } else {
                        // insert on new page
                        new_pos = i - half;
                        adding_to = new_page;
                    }

                    // update number of records for pages
                    new_page->num_records = current_page->num_records - half;
                    current_page->num_records = half;

                    printf("num_records: %zu\n", adding_to->num_records);

                    for (int j = (int) adding_to->num_records - 1; j >= new_pos; j--) {
                        adding_to->records[j+1] = adding_to->records[j];
                        printf("value at new pos %d: [%s, %d, %g]\n", j+1, adding_to->records[j+1][0].c, adding_to->records[j+1][1].i, adding_to->records[j+1][2].d);
                    }

                    printf("inserting record in position %d on page %d\n\n", new_pos, adding_to->page_id);
                    adding_to->records[new_pos] = record;
                    adding_to->num_records = adding_to->num_records + 1;

                } else {
                    // starting at the end of the list, move all records over 1 up to the one we want to insert before
                    for (int j = (int) current_page->num_records - 1; j >= i; j--) {
                        current_page->records[j+1] = current_page->records[j];
                        printf("value at new pos %d: [%s, %d, %g]\n", j+1, current_page->records[j+1][0].c, current_page->records[j+1][1].i, current_page->records[j+1][2].d);
                    }
                    printf("inserting record in position %d on page %d\n\n", i, current_page->page_id);
                    current_page->records[i] = record;
                    current_page->num_records = current_page->num_records + 1;
                }
                freeTable(table);
                return 0;
            }
            // else move on to comparing based on next index in primary key
        }
        // if reached here, did not insert yet and did not find existing row that matched

        // if next page exists
        if(current_page->nextPage != NULL){
            // change pointer to point to next page
            current_page = current_page->nextPage;
        } else {
            // does the page have room to fit the record we're about to add?
            size_t size_after_adding = (current_page->num_records + 1) * table.data_types_size * sizeof(union record_item);
            printf("page size after adding: %zu\n", size_after_adding);
            if (size_after_adding > BUFFER->page_size) {
                printf("out of space in page\n");

                // Get index after last item in first half of page, keeping more items in first half when necessary
                int half = floor(((double) current_page->num_records + 1) / 2.0);
                printf("index after first half: %d\n", half);

                int current_page_index = -1;

                for (int i = 0; i < table.page_ids_size; i++) {
                    if (table.page_ids[i] == current_page_id) {
                        current_page_index = i;
                    }
                }
                if (current_page_index < 0) {
                    // should not happen
                    return -1;
                }

                int new_page_id = create_page(table_id, current_page_index + 1);

                // tables are created with exactly enough space to hold their current page ids
                realloc(table.page_ids, (table.page_ids_size + 1) * sizeof(int));

                for (int i = table.page_ids_size - 1; i > current_page_index; i--) {
                    table.page_ids[i+1] = table.page_ids[i];
                }

                // update page_id list and size of list
                table.page_ids[current_page_index + 1] = new_page_id;
                table.page_ids_size = table.page_ids_size + 1;

                Page new_page = load_page(new_page_id);

                // copy second half of records over to new page
                for (int i = half; i < current_page->num_records; i++) {
                     new_page->records[i - half] = current_page->records[i];
                     current_page->records[i] = NULL;
                }

                // update next page references
                new_page->nextPage = current_page->nextPage;
                current_page->nextPage = new_page;

                printf("appending record to page %d\n\n", new_page->page_id);

                // add new record to end of new page
                new_page->records[current_page->num_records - half] = record;

                // update pages with new number of records
                new_page->num_records = current_page->num_records - half + 1;
                current_page->num_records = half;

                //printf("help %s", current_page->records[0]->c);

            } else {
                printf("appending record to page %d: [%s, %u, %g]\n\n", current_page->page_id, record[0].c, record[1].i, record[2].d);
                current_page->records[current_page->num_records] = record;
                current_page->num_records = current_page->num_records + 1;
            }
            freeTable(table);
            return 0;
        }
    }
    freeTable(table);
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
    Table table = getTable(table_id, BUFFER->db_location);
    char *db_location = malloc(sizeof(char *));
    strcpy(db_location, BUFFER->db_location);
    strcat(db_location, "/"); // need this to change for windows.

    // delete pages associated with table.
    for (int i = 0; i < table.page_ids_size; i++) {

        char *filename = appendIntToString(db_location, table.page_ids[i]);

        // file was unable to be deleted/was not found.
        if (remove(filename) != 0) {
            result = EXIT_FAILURE;
        }

        free(filename);
    }

    // delete the table.
    char *table_location = malloc((sizeof(char *)));
    strcpy(table_location, db_location);

    // build the file location
    char *table_name = TABLE_FILE;
    char *table_file = appendIntToString(table_name, table_id);

    // Table was unable to delete, thus error.
    if (remove(table_file) != 0) {
        result = EXIT_FAILURE;
    }

    BUFFER->table_count--;

    // free created strings.
    free(table_file);
    free(table_location);
    free(db_location);
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

    Table table = getTable(table_id, BUFFER->db_location);
    char *db_location = malloc(sizeof(char *));
    strcpy(db_location, BUFFER->db_location);
    strcat(db_location, "/"); // need this to change for windows.

    // delete pages associated with table.
    for (int i = 0; i < table.page_ids_size; i++) {
        char *filename = appendIntToString(db_location, table.page_ids[i]);

        // file was unable to be deleted/was not found.
        if (remove(filename) != 0) {
            result = EXIT_FAILURE;
        }

        free(filename);
    }

    table.page_ids_size = 0;

    // TODO: re-write to make cleaner, will want to make writing to table file a function.
    char *file_name = appendIntToString("table_", table_id);
    char *table_location = malloc((sizeof(char *)));
    strcpy(table_location, db_location);
    strcat(table_location, file_name);

    FILE *table_file = fopen(table_location, "wb");

    if (table_file != NULL) {
        // re-write the Table file.
        fwrite(&table.data_types_size, sizeof(int), 1, table_file);
        fwrite(&table.key_indices_size, sizeof(int), 1, table_file);
        fwrite(&table.page_ids_size, sizeof(int), 1, table_file);
        fwrite(table.key_indices, sizeof(int), table.key_indices_size, table_file);
        fwrite(table.data_types, sizeof(int), table.data_types_size, table_file);
        fwrite(table.page_ids, sizeof(int), table.page_ids_size, table_file);
        fclose(table_file);
    } else {
        result = EXIT_FAILURE;
    }

    free(file_name);
    free(table_location);

    free(db_location);
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

    printf("Adding table...\n");

    // get database path
    char *database_path = BUFFER->db_location;

    // get table id
    int table_id = BUFFER->table_count;

    // create path to the table in the database
    char *table_id_string = appendIntToString("table_", table_id); // this was just used to test we will find this later
    // refactor into function
    char *table_path = malloc(strlen(database_path) + strlen(table_id_string) + 1);
    strcpy(table_path, database_path);
    strcat(table_path, table_id_string);

    // build the struct for the table
    Table table_content = {
            .data_types_size = data_types_size,
            .key_indices_size = key_indices_size,
            .page_ids_size = 0,
            .key_indices = key_indices,
            .data_types = data_types
    };

    // write the table to disk
    // was storing address of int arrays before not actual data
    FILE *table_file = fopen(table_path, "wb");
    if (table_file != NULL) {
        fwrite(&table_content.data_types_size, sizeof(int), 1, table_file);
        fwrite(&table_content.key_indices_size, sizeof(int), 1, table_file);
        fwrite(&table_content.page_ids_size, sizeof(int), 1, table_file);
        fwrite(table_content.key_indices, sizeof(int), table_content.key_indices_size, table_file);
        fwrite(table_content.data_types, sizeof(int), table_content.data_types_size, table_file);
        fwrite(table_content.page_ids, sizeof(int), table_content.page_ids_size, table_file);

        fclose(table_file);
    } else {
        table_id = -1;
    }

    free(table_path);
    free(table_id_string);

    BUFFER->table_count++;

    printf("Success!\n");
    printTable(table_content);
    return table_id;
}

/*
 * This will purge the page buffer to disk.
 * @return 0 on success, -1 on failure.
 */
int purge_buffer() {
    int result = EXIT_SUCCESS;

    // foreach page in buffer
    for (int i = 0; i < BUFFER->buffer_size; i++) {
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
   // result = purge_buffer();

    // write buffer info to disk
    char *buffer_file = BUFFER_FILE;
  //  result = write_buffer_to_disk(buffer_file, BUFFER); // breaks valgrind

    // perform proper memory wipes.
    freeBuffer(BUFFER);
    return result;
}
