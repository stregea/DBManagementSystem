#include "table.h"

#ifndef DBMANAGEMENTSYSTEM_CATALOG_H
#define DBMANAGEMENTSYSTEM_CATALOG_H

/**
 * Struct used to represent the catalog.
 *
 * This is created within the create catalog command.
 */
struct Catalog {
    int table_count;
    struct Table **tables;
};typedef struct Catalog *Catalog;

static char *GLOBAL_DB_LOCATION;
static Catalog catalog = NULL;

/**
 * Create catalog for first table
 * @param table - pointer to initial table
 */
int createCatalog(Table table);

/**
 * Free the catalog and its contents from memory.
 */
void freeCatalog();

/**
 * Write a catalog and all of its contents to disk.
 *
 * @return 0 on success; -1 on error.
 */
int write_catalog_to_disk();

/**
 * Read a catalog from disk.
 *
 * @return 0 on success; -1 on error.
 */
int read_catalog_from_disk();


void display_catalog();

/**
 * get the catalog file path
 *
 * @return catalog file path
 */
char* get_catalog_file_path();

/**
 * Add a Table to the catalog.
 *
 * @param table - The table to add.
 * @return 0 on success; -1 on error.
 */
int add_table_to_catalog(Table table);

/**
 * Remove a Table to the catalog.
 * @param table_name - The name of the table to remove.
 * @return 0 on success; -1 on error.
 */
int remove_table_from_catalog(char *table_name);

/**
* Get a Table from the catalog.
* @param table_name - The name of the table to retrieve.
* @return 0 on success; -1 on error.
*/
Table get_table_from_catalog(char *table_name);
#endif //DBMANAGEMENTSYSTEM_CATALOG_H
