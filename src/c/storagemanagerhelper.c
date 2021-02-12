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

/**
 * Append an integer to an existing char*.
 * Note: the newly created string must be freed once used.
 * @param original - The original char pointer text.
 * @param number - The number to append to a char*.
 * @return a newly allocated char* with the appended integer.
 */
char * appendIntToString( char * original, int number )
{
    char * newString = malloc( sizeof( char* ) );
    sprintf(newString, "%s%d", original, number);
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
    
    /// variable used to build file path for command to execute
    char * directory = malloc(sizeof(char*));
    
    // copy dir_name into directory
    copyStringForFilePath(directory, dir_name);

    strcat(directory, "*"); // append '*' to delete all folders and files

    // build the command
    char * command = malloc(strlen("exec rm -rf ")+strlen(directory));
    strcat(command, "exec rm -rf ");
    strcat(command, directory);

    // execute the command
    system(command);
    
    free(directory);
    free(command);
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
