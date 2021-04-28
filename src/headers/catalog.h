
#ifndef CATALOG_H
#define CATALOG_H

#include "table.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "storagemanager.h"

typedef struct catalog_{
	char * db_loc;
	Table * tables;
	int tables_size;
} *Catalog;

int create_catalog( char * db_loc, int page_size, int buffer_size );

bool table_name_exists(char * name);

Table get_table_by_num( int num );

Table get_table_by_name( char * name );

int drop_table_catalog( int id );

int add_tble_to_catalog( Table table);

void print_schema();

#endif