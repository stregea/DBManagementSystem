#include <stdbool.h>
#include <stdio.h>

#ifndef DBMANAGEMENTSYSTEM_TABLE_H
#define DBMANAGEMENTSYSTEM_TABLE_H
struct Attribute;
typedef struct Attribute *Attribute;

struct PrimaryKey {
    struct Attribute **attributes; // array with the attributes that make up the primary key
    int size; // count used to keep track of the # of attributes within a primary key.
};
typedef struct PrimaryKey *PrimaryKey;

/**
 * Struct to contain foreign key information.
 */
struct ForeignKey {
    int referenced_table_name_size;
    int referenced_column_name_size;
    char *referenced_table_name; // name of referenced table
    char *referenced_column_name; // name of referenced column
};
typedef struct ForeignKey *ForeignKey;


/**
 * Struct to contain booleans to determine if an attribute uses any or all of the constraints.
 */
struct Constraints {
    bool notnull;
    bool primary_key;
    bool unique;
};
typedef struct Constraints *Constraints;

/**
 * Struct to contain information in regards to a attribute/column.
 */
struct Attribute {
    char *name;
    int name_size;
    int type; // the type of data within the column (0-4) / int-varchar
    int size; // used to determine the size of a char or varchar.
    int default_size;
    char *default_value;
    struct Constraints *constraints;
    struct ForeignKey *foreignKey;
};

/**
 * Struct used to represent a Table.
 *
 * This is created within the create table command, or read from the catalog.
 */
struct Table {
    int tableId;
    char *name; // the name of the table
    int name_size;
    struct Attribute **attributes; // Array to hold the attributes/columns of a table.
    int *data_types; // array to contain the data types found within a table.
    int *key_indices;
    struct PrimaryKey *primary_key;
    int primary_key_count; // count used to keep track of the # of primary keys that exist within a table. 1 Max.
    int attribute_count; // count used to keep track of the # of attributes/columns that exist within a table.
    int key_indices_count; // count used to keep track of the # of attributes/columns that exist within a table.
    int data_type_size; // count used to keep track of the # of datatypes within the table.
};
typedef struct Table *Table;

/**
 * Free a table struct from memory.
 * @param table - The table to free.
 */
struct Table *createTable(char *name);

/**
 * Frees all memory referenced by an Attribute, then the struct itself
 * @param attr
 */
void freeAttribute(Attribute attr);

/**
 * Free a table struct from memory.
 * @param table - The table to free.
 */
void freeTable(struct Table *table);

/**
 * Free a PrimaryKey struct from memory.
 * @param key - The key to free.
 */
void freeKey(PrimaryKey key);

/**
 * Generate key_indices to be stored in a table to be used to generate a primary key.
 * @param attribute_names - A string of multiple attribute names '<a_1> ... <a_N>'. This is created while parsing.
 * @param table - table that contains all of the current columns/attributes existing within a table.
 * @return key_indices[] on success; null otherwise.
 */
PrimaryKey create_key(char *attribute_names, Table table);

/**
 * Generates a primary key that was declared as part of an attribute and adds that attribute to the table
 * @param attribute_name
 * @param table
 * @return
 */
PrimaryKey create_key_from_attr(Attribute attr, Table table);

/**
 * Generate key_indices to be stored in a table to be used to generate a primary key.
 * @param attribute_names - A string of multiple attribute names '<a_1> ... <a_N>'. This is created while parsing.
 * @param table - table that contains all of the current columns/attributes existing within a table.
 * @param key_size - number of attributes to make up key
 * @return key_indices[] on success; null otherwise.
 */
struct PrimaryKey *create_key_from_disk(char *attribute_names, Table table, int key_size);

/**
 * Safely add a primary key to a table.
 * This function will check to see if a table already has an existing key.
 *
 * If the table doesn't have a primary key -> success, add the new key.
 * Otherwise a table already has a key -> error.
 * @param table - The table to reference.
 * @param key_indices - The key_indices to store within the table.
 * @return 0 on success; -1 on error.
 */
int add_primary_key_to_table(Table table, PrimaryKey key);

/**
 * Write a table and all of its contents to disk.
 *
 * @return 0 on success; -1 on error.
 */
int write_table_to_disk();

/**
 * Write an attribute and all of its contents to disk.
 *
 * @return 0 on success; -1 on error.
 */
int write_attribute_to_disk();

/**
 * Write a key and all of its contents to disk.
 * @return 0 on success; -1 on error.
 */
int write_primary_key_to_disk();

/**
 * Write a foreign key and all of its contents to disk.
 *
 * @return 0 on success; -1 on error.
 */
int write_foreign_key_to_disk();

/**
 * Read a foreign key and all of its contents from disk.
 * @param file - pointer to catalog file
 * @return pointer to key struct
 */
struct ForeignKey *read_foreign_key_from_disk(FILE *file);

/**
 * Read an attribute and all of its contents from disk.
 * @param file - pointer to catalog file
 * @return pointer to key struct
 */
struct Attribute *read_attribute_from_disk(FILE *file);

/**
 * Read a key and all of its contents from disk.
 * @param file - pointer to catalog file
 * @return pointer to key struct
 */
struct PrimaryKey *read_primary_key_from_disk(FILE *file, struct Table *table);

/**
 * Read a table and all of its contents from disk.
 * @param file - pointer to catalog file
 * @return pointer to table struct
 */
struct Table *read_table_from_disk(FILE *file);

/**
 * Retrive an attribute from a table.
 * @param table - The table to search.
 * @param attribute_name - The specified attribute.
 * @return an Attribute. NULL if not found.
 */
Attribute get_attribute_from_table(Table table, char *attribute_name);

#endif
