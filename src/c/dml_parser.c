#include "../headers/dml_parser.h"

/*
 * This function handles the parsing of DML statments
 * that return nothing, such as insertion, deletion, etc.
 *
 * @param statement - the DML statement to execute
 * @return 0 on sucess; -1 on failure
 */
int parse_dml_statement( char * statement ){
    return 0;
}

/*
* This function handles the parsing of DML statments
* that return data, such as select queries.
*
* @param query - the SQL query to execute
* @param result - a 2d array of record_item (output variable).
*                This will be used to output the values of the query.
                 This will be a pointer to the first item in the 2d array.
                 The user of the function will be resposible for freeing.
* @return the number of tuples in the result, -1 if upon error.
*/
int parse_dml_query(char * query, union record_item *** result){
    return 0;
}
