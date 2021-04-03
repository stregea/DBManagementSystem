#ifndef DBMANAGEMENTSYSTEM_DML_MANAGER_H
#define DBMANAGEMENTSYSTEM_DML_MANAGER_H

#include "storagemanager.h"
#include "table.h"

/**
 * Parse the 'insert' dml query.
 *
 * This will create a record and then store the record into
 * the storage manager.
 * @param statement - The statement to parse.
 * @return 0 if successful, -1 on error.
 */
int parse_insert_statement(char *statement);

/**
 * Parse the 'update' dml query.
 *
 * This will create a record and then store the record into
 * the storage manager.
 * @param statement
 * @return 0 if successful, -1 on error.
 */
int parse_update_statement(char *statement);

/**
 * Parse the 'delete from' dml query.
 *
 * This will create a record and then store the record into
 * the storage manager.
 * @param statement
 * @return 0 if successful, -1 on error.
 */
int parse_delete_from_statement(char *statement);

/**
 * Parse the 'select' dml query.
 *
 * This will parse a select statement and then return the records
 * that were specified via the query.
 *
 * Note: This directly interacts with with select parser.h.
 * @param statement - The statement to parse.
 * @return 0 if successful, -1 on error.
 */
int parse_select_statement(char *statement);

/**
 * Free an array of unions.
 * @param record - The record to free.
 */
void freeRecord(Table table, union record_item * record);

/**
 * Create a record dynamically based on the statement sent in through
 * the parameters.
 *
 * @param table - The table to get the attribute information from.
 * @param tuple - The values to place within a record.
 * @return an array of record items.
 */
union record_item * create_record_from_statement(Table table, char *tuple);
#endif
