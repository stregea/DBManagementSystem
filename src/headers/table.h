
#ifndef TABLE_H
#define TABLE_H

#include "attribute.h"
#include "unique.h"
#include "foreign_key.h"


typedef struct table_{
	int num;
	char * name;
	Attr * attrs;
	int attrs_size;
	Unique primary_key; //just a special unique
	Unique * uniques;
	int uniques_size;
	Foreign_Key * fks;
	int fks_size;
	char ** ref_fks; //tables that have fks that refer to this table
	int ref_fks_size;
} * Table;


Table create_table( int num, char * name );

char * get_name( Table table );

int get_num( Table table );

void set_num( Table table, int num );

Attr * get_attrs( Table table );

Attr get_attr_by_position( Table table, int position );

Attr get_attr_by_name( Table table, char * name );

Unique * get_uniques( Table table );

int get_uniques_size( Table table );

Unique get_primary_key( Table table );

void set_primary_key ( Table table, Unique pk );

Foreign_Key * get_foreign_keys( Table table );

int get_foreign_keys_size( Table table );

int add_attr( Table table, Attr attr );

int drop_attr( Table table, Attr attr );

int add_unique( Table table, Unique unique );

int add_foreign_key( Table table, Foreign_Key fk );

void print_table_schema( Table table );

void update_ref_fks( Table table, Table refer_table, Attr attr );

#endif