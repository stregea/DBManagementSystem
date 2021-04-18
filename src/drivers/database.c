/**
The source file for the main of the database.

It will start/restart the database.
Prompt the user for commands.
Send those commands to the proper parser.

Author: Scott C. Johnson (sxjcs@rit.edu)

FUTURE WORK: Add memory management
**/

#include "../headers/database.h"
#include "../headers/ddl_parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "catalog.h"
#include <string.h>

int execute_non_query(char * statement){
	//just make everything a ddl in phase2
	return parse_ddl_statement( statement );
}

int execute_query(char * query, union record_item *** result){
	//implement in phase 3
	return -1;
}

int shutdown_database(){
	//TODO: Implement shutdown_database
	return -1;
}

int main(int argc, char ** argv ){
	if(argc != 4){
		fprintf(stderr, "Usage: ./database <dbloc> <page_size> <buffer_size>\n");
		return -1;
	}
	
	long page_size = strtol(argv[2], NULL, 10);
	long buffer_size = strtol(argv[3], NULL, 10);
	if(create_catalog(argv[1], page_size, buffer_size) == -1){
		fprintf(stderr, "ERROR creating/restarting database\n");
		return -1;
	}
	
	printf("Database started successfully\n\n");
	printf("To print the database schema type 'print;'\n");
	char * stmt = malloc(sizeof(char));
	stmt[0] = '\0';
	while(true){
		printf(" SQL> ");
		char * line;
		size_t len = 0;
		ssize_t line_size = 0;
		
		line_size = getline(&line, &len, stdin);
		stmt = realloc(stmt, strlen(stmt) + line_size + 1);
		line[line_size-1] = '\0';
		if(strlen(stmt) != 0)
			strcat(stmt, " ");
		strcat(stmt, line);
		
		if(stmt[strlen(stmt) - 1] == ';'){
			if(strcmp(stmt, "quit;") == 0)
				break;
			else if(strcmp(stmt, "print;") == 0){
				print_schema();
			}
			else{
				//process only ddl for phase 2
				int result = parse_ddl_statement(stmt);
				if(result != 0){
					fprintf(stderr, "ERROR\n");
				}
				else{
					printf("SUCCESS\n");
				}
			}
			free(stmt);
			stmt = malloc(sizeof(char));
			stmt[0] = '\0';
		}
	}
	
	printf("Exiting the database\n");
	
	return 0;
}