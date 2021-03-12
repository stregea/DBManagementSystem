#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef DBMANAGEMENTSYSTEM_DDL_MANAGER_H
#define DBMANAGEMENTSYSTEM_DDL_MANAGER_H

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
}; typedef struct Constraints* Constraints;

/**
 * Struct to contain information in regards to a attribute/column.
 */
struct Attribute {
    char *name;
    int name_size;
    int type; // the type of data within the column (0-4) / int-varchar
    int size; // used to determine the size of a char or varchar.
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
 * Struct used to represent the catalog.
 *
 * This is created within the create catalog command.
 */
struct Catalog {
    int table_count;
	struct Table **tables;
};typedef struct Catalog *Catalog;

/**
 * This will allocate memory in the catalog to allow for the storage of Tables.
 * 
 * @return 0 on success; -1 on error.
 */
int initialize_ddl_parser(char *db_loc, bool restart);

/**
 * TODO
 * This will terminate the parser, write the catalog to disk, and then free memory.
 * @return 0 on success; -1 on error.
 */
int terminate_ddl_parser();

/**
 * Create catalog for first table
 * @param table - pointer to initial table
 */
int createCatalog(Table table);

/**
 * TODO
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
struct ForeignKey * read_foreign_key_from_disk(FILE *file);

/**
 * Read an attribute and all of its contents from disk.
 * @param file - pointer to catalog file
 * @return pointer to key struct
 */
struct Attribute * read_attribute_from_disk(FILE *file);

/**
 * Read a key and all of its contents from disk.
 * @param file - pointer to catalog file
 * @return pointer to key struct
 */
struct PrimaryKey* read_primary_key_from_disk(FILE *file);

/**
 * Read a table and all of its contents from disk.
 * @param file - pointer to catalog file
 * @return pointer to table struct
 */
struct Table* read_table_from_disk(FILE *file);

/**
 * Read a catalog from disk.
 * 
 * @return 0 on success; -1 on error.
 */
int read_catalog_from_disk();

/**
 * Add a Table to the catalog.
 * 
 * @param table - The table to add.
 * @return 0 on success; -1 on error.
 */
int add_table_to_catalog(Table table);

/**
 * TODO
 * Remove a Table to the catalog.
 * @param table_name - The name of the table to remove.
 * @return 0 on success; -1 on error.
 */
int remove_table_from_catalog(char *table_name);

/**
* TODO
* Get a Table to the catalog.
* @param table_name - The name of the table to retrieve.
* @return 0 on success; -1 on error.
*/
Table get_table_from_catalog(char *table_name);

/**
 * Free a table struct from memory.
 * @param table - The table to free.
 */
struct Table * createTable(char *name);

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
 * Return an integer associated with an attribute type.
 *      0 - Integer
 *      1 - Double
 *      2 - Bool
 *      3 - Char
 *      4 - Varchar
 * @param type - The attribute type to parse.
 * @return a number between 0-4; -1 if an invalid type.
 */
int get_attribute_type(char *type);

/**
 * TODO - test
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
 * TODO
 * Generate key_indices to be stored in an array that holds the inique keys found within the table..
 * @param attribute_names - A string of multiple attribute names '<a_1> ... <a_N>'. This is created while parsing.
 * @param table - table that contains all of the current columns/attributes existing within a table.
 * @return int* on success; null otherwise.
 */
int* create_unique_key(char *attribute_names, Table table);

/**
 * TODO - test
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
 * TODO
 * Safely add a unique key to a table.
 * This function will check to see if a table already has an existing key.
 *
 * If the table doesn't have an existing unique key -> success, add the new key.
 * Otherwise a table already has a key -> error.
 * @param table - The table to reference.
 * @param key - The unique key to store within the table.
 * @return 0 on success; -1 on error.
 */
int add_unique_key_to_table(Table table, PrimaryKey key);

/**
 * Parse through the SQL statement the user entered.
 * @param statement - the statement to parse.
 * @return 0 upon success, -1 upon error.
 */
int parseStatement(char *statement);

/**
 * TODO
 * Parse through the Create Table command.
 * @param tokenizer - the tokenizer to parse a command.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseCreate(char *tokenizer, char **token);

/**
 * TODO
 * Parse through the Drop Table command.
 * @param command - the command to parse.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseDrop(char *tokenizer, char **token);

/**
 * TODO
 * Parse through the Alter Table command.
 * @param tokenizer - the tokenizer to parse a command.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseAlter(char *tokenizer, char **token);

/**
 * Given a query:
 *  create table <name>(
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      primarykey( <a_1> ... <a_N> ),
 *      unique( <a_1> ... <a_N> ),
 *      foreignkey( <a_1> ... <a_N> ) references <r_name>( <r_1> ... <r_N> )
 *  );
 *
 * This function will handle the parsing of 'primarykey( <a_1> ... <a_N> )' and will generate a
 * key_indices[] for a table.
 * @param table - The table to reference.
 * @param names - The tokenizer containing the string of attribute names to be parsed.
 * @return 0 on success; -1 on error.
 */
int parsePrimaryKey(Table table, char *names);

/**
 * TODO
 * Given a query:
 *  create table <name>(
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      primarykey( <a_1> ... <a_N> ),
 *      unique( <a_1> ... <a_N> ),
 *      foreignkey( <a_1> ... <a_N> ) references <r_name>( <r_1> ... <r_N> )
 *  );
 *
 * This function will handle the parsing of 'foreignkey( <a_1> ... <a_N> )' and will generate a foreign key to
 * reference another table.
 * @param table - The table to reference.
 * @param tokenizer - The tokenizer containing the string to be parsed.
 * @return 0 on success; -1 on error.
 */
int parseForeignKey(Table table, char *tokenizer, char **token);

/**
 * TODO
 * Given a query:
 *  create table <name>(
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      primarykey( <a_1> ... <a_N> ),
 *      unique( <a_1> ... <a_N> ),
 *      foreignkey( <a_1> ... <a_N> ) references <r_name>( <r_1> ... <r_N> )
 *  );
 *
 * This function will handle the parsing of 'foreignkey( <a_1> ... <a_N> )' and will generate a foreign key to
 * reference another table.
 * @param table - The table to reference.
 * @param tokenizer - The tokenizer containing the string to be parsed.
 * @return 0 on success; -1 on error.
 */
int parseUniqueKey(Table table, char *names);

/**
 * TODO
 * Given a query:
 *  create table <name>(
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      primarykey( <a_1> ... <a_N> ),
 *      unique( <a_1> ... <a_N> ),
 *      foreignkey( <a_1> ... <a_N> ) references <r_name>( <r_1> ... <r_N> )
 *  );
 *
 * This function will handle the parsing of '<a_name> <a_type> <constraint_1> ... <constraint_N>,
 *                                           <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *                                           <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *                                           <a_name> <a_type> <constraint_1> ... <constraint_N>,'
 * and properly store the values within the table.
 * @param table - The table to reference.
 * @param tokenizer - The tokenizer containing the string to be parsed.
 * @return 0 on success; -1 on error.
 */
int parseAttributes(Table table, char *tokenizer);

/**
 * get the catalog file path
 * 
 * @return catalog file path
 */
char* get_catalog_file_path();

void display_catalog();

/**
 * Determine if 'char' or 'varchar' exist in a string.
 * @param tokenizer - The tokenizer containing the string to be parsed.
 * @param token - The token used for string tokenizing.
 * @return a new string; null if not char/varchar
 */
int char_or_varchar(char*tokenizer);
#endif
