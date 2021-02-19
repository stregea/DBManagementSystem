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
#define MAX_INT_32_DIGIT 10

/**
 * Append an integer to an existing char*.
 * Note: the newly created string must be freed once used.
 * @param original - The original char pointer text.
 * @param number - The number to append to a char*.
 * @return a newly allocated char* with the appended integer.
 */
char * appendIntToString( char * original, int number )
{
    // string length + len(MAX_INT_32 as string)
    size_t buffer_size = strlen(original) + MAX_INT_32_DIGIT;
    char * newString = malloc(buffer_size);
    // to prevent buffer overflow
    snprintf(newString, buffer_size, "%s%d", original, number);
    return newString;
}

/**
 * Determine if the page size and buffer size meet the required sizes.
 * @param page_size - The page size to check.
 * @param buffer_size - The buffer size to check.
 * @return true if both page_size and buffer_size are greater than or equal to zero; false otherwise.
 */
bool isProperSize( int page_size, int buffer_size ){
    return buffer_size >=0 && page_size >=0;
}

/**
 * Copy a string for a filepath.
 * Note: This needs to be tested on windows.
 * @param destination - The location to store the new string.
 * @param original_str - The original string that is used to check and copy information.
 */
void copyStringForFilePath(char* destination, char* original_str){
    strcpy(destination, original_str);
    
    // check if there is a '/' appended on the destination string
    // if not, append a '/'.
    if(destination[strlen(destination)-1] != '/'){
        strcat(destination, "/"); // need to test this on windows
    }
}

/**
 * Go to a specified directory and clear its contents but keep the parent directory..
 * @param dir_name - The directory to clear.
 */
void clearDirectory( char * dir_name ){

    if(dir_name[strlen(dir_name)-1] != '/'){
        strcat(dir_name, "/"); // need to test this on windows
    }

    // to avoid damaging the db path string
    char* directory = malloc(strlen(dir_name) + 1);
    strcpy(directory, dir_name);
    strcat(directory, "*");

    // concat command and directory
    char* baseCommand = "exec rm -rf ";   
    char* command = malloc(strlen(directory) + strlen(baseCommand));
    strcpy(command, baseCommand);
    strcat(command, directory);

    //printf("%s\n", command);

    // execute the command
    system(command);
    
    free(command);
    free(directory);
}

/**
 * Create a DB Store file to contain the information about the db.
 * @param db_loc - The database location to store.
 * @param page_size - The size of the pages to store.
 * @param buffer_size - The size of the buffer to store.
 */
void createDBStore( char * db_loc,  int page_size, int buffer_size ){
    // allocate memory.
    DBStore store  = malloc(sizeof(struct DBStore_S));
    char * directory = malloc(sizeof(char*));
    char * store_file = malloc(sizeof(char*));
    store->db_location = malloc(sizeof(char*));
    store->db_store_location = malloc(sizeof(char*));
    
    // copy db_loc into directory
    copyStringForFilePath(directory, db_loc);
      
    // create the filename
    strcpy(store_file, directory);
    strcat(store_file, "db_store");

    // build the struct
    strcpy(store->db_location, directory);
    strcpy(store->db_store_location, store_file);
    store->page_size = page_size;
    store->buffer_size = buffer_size;
    
    
    // write to the file
    FILE * file = fopen(store_file, "wb");
    if(file != NULL){
        fwrite(store, sizeof(struct DBStore_S), 1, file);
        fclose(file);
    }
    
    free(store);
    free(store_file);
    free(directory);
}

/**
 * Create a DBStore struct from an existing dbstore file.
 * Note: The struct returned must be freed using the freeStore() function.
 * @param store_loc - The location of the dbstore file.
 * @returns a struct with the previously created DBStore information.
 */
DBStore readDBStore(char * store_loc){
    DBStore store = malloc(sizeof(struct DBStore_S));
    
    // read the binary file
    FILE * file = fopen(store_loc, "rb");
    if(file != NULL){
        fread(store, sizeof(struct DBStore_S), 1, file);
        fclose(file);
    }
  
    return store;
}

/*
 * Free the previously allocated memory created for a DBStore.
 */
void freeStore(DBStore store){
    free(store->db_location);
    free(store->db_store_location);
    free(store);
}

Table getTable(int table_id, char * database_path){

    // convert int table_id to string for file path
    char* table_id_string = appendIntToString("table_", table_id);

    // create path to the table in the database
    // add null terminating btye
    size_t path_buffer_size = strlen(database_path) + strlen(table_id_string) + 1;
    char* table_path = malloc(path_buffer_size);

    // preventing buffer overflows
    strncpy(table_path, database_path, path_buffer_size);
    strncat(table_path, table_id_string, path_buffer_size - strlen(table_path));

    // open table and determine file size
    FILE* table_file = fopen (table_path, "rb");
    // move pointer to pass end of file
    fseek(table_file, 0, SEEK_END);
    // getting address first beyond end of file
    int file_size = (int)ftell(table_file);
    rewind(table_file);
    
    // read file contents into struct
    Table table;
    fread(&table.data_types_size, sizeof(int), 1, table_file);
    fread(&table.key_indices_size, sizeof(int), 1, table_file);
    fread(&table.page_ids_size, sizeof(int), 1, table_file);

    // allocate space for the int arrays
    table.data_types = malloc(sizeof(int) * table.data_types_size);
    table.key_indices = malloc(sizeof(int) * table.key_indices_size);
    table.page_ids = malloc(sizeof(int) * table.page_ids_size);

    fread(table.key_indices, sizeof(int), table.key_indices_size, table_file);
    fread(table.data_types, sizeof(int), table.data_types_size, table_file);
    fread(table.page_ids, sizeof(int), table.page_ids_size, table_file);

    // free all dynamic memory
    free(table_path);
    free(table_id_string);

    return table;
}

void printIntArray(int *array, int size){
    printf("{ ");
    for(int i = 0; i < size; i++){
        printf("%d, ", array[i]);
    }
    printf(" }\n");
}

void printTable(Table table) {

    printf("data_types_size = %d\n", table.data_types_size);
    printf("key_indices_size = %d\n", table.key_indices_size);
    printf("table.page_ids_size = %d\n", table.page_ids_size);
    
    printf("data_types = ");
    printIntArray(table.data_types, table.data_types_size);
    
    printf("key_indices = ");
    printIntArray(table.key_indices, table.key_indices_size);

    printf("page_ids = ");
    printIntArray(table.page_ids, table.page_ids_size);
}

int addPageIdToTable(int table_id, int page_id, char * database_path){

    // convert int table_id to string for file path
    char* table_id_string = appendIntToString("", table_id);

    // create path to the table in the database
    char* table_path = malloc(sizeof(char*) * (strlen(database_path) + strlen(table_id_string)));
    strcpy(table_path, database_path);
    strcat(table_path, table_id_string);

    // open table and determine file size
    FILE* table_file = fopen (table_path, "r");
    fseek(table_file, 0, SEEK_END);
    int file_size = (int)ftell(table_file);
    rewind(table_file);
    
    // read file contents into struct
    Table table;
    fread(&table, file_size, 1, table_file);

    int num_pages = ++table.page_ids_size;
    int temp[num_pages];
    for(int i = 0; i < num_pages - 1; i++)
    {
        temp[i] = table.page_ids[i];
    }
    temp[num_pages] = page_id;

    // may need to reallocate
    table.page_ids = temp;

    fwrite(&table, sizeof(table), 1, table_file);
    fclose(table_file);
    
    // free all dynamic memory
    free(table_path);
    free(table_id_string);

    return 0;
}
