#include "../headers/database.h"
#include "../headers/storagemanager.h"
#include "../headers/ddl_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_SIZE 20

/*
 * This function will be used to execute SQL statements that
 * do not return any data. For instance, schema modification,
 * insertion, delete, and update statements.
 *
 * @param statement - the SQL statement to execute.
 * @return 0 if the statement is executed successfully
           -1 otherwise
 */
int execute_non_query(char * statement){
    return 0;
}

/*
 * This function will be used to execute SQL statement that
 * return data. For instance, SQL select queries.
 *
 * @param query - the SQL query to execute
 * @param result - a 2d array of record_item (output variable).
 *                This will be used to output the values of the query.
                  This will be a pointer to the first item in the 2d array.
                  The user of the function will be resposible for freeing.
 * @return the number of tuples in the result, -1 if upon error.
 */
int execute_query(char * query, union record_item *** result){
    return 0;
}

/*
 * This function is resposible for safely shutdown the database.
 * It will store to hardware any data needed to restart the database.
 * @return 0 on success; -1 on failure.
 */
int shutdown_database(){
    return 0;
}

int main(int argc, char *argv[]) {
    
    if(argc < 3) {
        fprintf(stderr, "%s", "usage: ./database <db_loc> <page_size> <buffer_size>\n");
    }

    char *databasePath = argv[1];
    int pageSize = (int)strtol(argv[2], NULL, 10);
    int bufferSize = (int)strtol(argv[3], NULL, 10);

    //printf("db_loc: %s\npage_size: %d\nbuffer_size: %d\n", databasePath, pageSize, bufferSize);

    FILE *databaseFile = fopen(databasePath, "r");
    if(databaseFile) {
        fclose(databaseFile);
        //printf("restarting database ...\n");
        //restart_database(databasePath);
    } else {
        //printf("restarting database ...\n");
        new_database(databasePath, pageSize, bufferSize);
    }

    // add table
    int data_types_size = 3;
    int data_types[] = {3, 0, 1};
    int key_indices_size = 2;
    int key_indices[] = {3, 0};
    int table_id = add_table(data_types, key_indices, data_types_size, key_indices_size);

    /// testing different commands.
    // 'proper' statements
    parse_ddl_statement("DROP TABLE HELLO;THIS IS A TEST;drop table sam;");
    parse_ddl_statement("alter table foo drop bar;");
    parse_ddl_statement("alter table foo add gar double;");
    parse_ddl_statement("alter table foo add far double default 10.1");

    // bad statements
    parse_ddl_statement("DROP TABLE");
    parse_ddl_statement("alter table");
    parse_ddl_statement("alter table foo");
    parse_ddl_statement("alter table foo drop");
    parse_ddl_statement("alter table foo add");
    parse_ddl_statement("alter table foo add gar");
    parse_ddl_statement("alter table foo add far double default");
    parse_ddl_statement("alter table foo add far double blah"); // should we error if 'default' not read?

    //    terminate_database();

//    }

}
