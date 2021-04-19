
#include "catalog.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "storagemanager.h"
#include "table.h"

Catalog catalog = NULL;

int create_catalog( char * db_loc, int page_size, int buffer_size ){
	
	//todo add restart
	catalog = malloc(sizeof(struct catalog_));
	catalog->db_loc = malloc(strlen(db_loc) + 1);
	strcpy(catalog->db_loc, db_loc);
	catalog->tables = NULL;
	catalog->tables_size = 0;
	
	create_database( db_loc, page_size, buffer_size, false );
	
	return 0;
}

bool table_name_exists(char * name){
	return get_table_by_name(name) != NULL;
}

Table get_table_by_num( int num ){
	if(catalog->tables_size == 0)
		return NULL;
	for(int i = 0; i < catalog->tables_size; i++){
		int t_num = get_num( catalog->tables[i]);
		if(t_num == num)
			return catalog->tables[i];
	}
	
	return NULL;
}

Table get_table_by_name( char * name ){
	if(catalog->tables_size == 0)
		return NULL;
	for(int i = 0; i < catalog->tables_size; i++){
		char * t_name = get_name( catalog->tables[i]);
		if(strcmp(t_name, name) == 0)
			return catalog->tables[i];
	}
	
	return NULL;
}

void remove_fks_drop(Table table){
	for(int i = 0; i < table->ref_fks_size; i++){
		Table ref_table = get_table_by_name(table->ref_fks[i]);
		update_ref_fks( ref_table, table, NULL );
	}
}

int drop_table_catalog( int id ){
	for(int i = 0; i < catalog->tables_size; i++){
		if(get_num(catalog->tables[i]) == id){
			remove_fks_drop(catalog->tables[i]);
			catalog->tables[i] = NULL;
			return 0;
		}
	}
	return -1;
}

int add_tble_to_catalog( Table table){
	if(catalog->tables == NULL)
		catalog->tables = malloc(sizeof(Table));
	
	int table_index = -1;
	
	for(int i = 0; i < catalog->tables_size; i++){
		if(catalog->tables[i] == NULL){
			table_index = i;
			break;
		}
	}
	
	if(table_index == -1){
		catalog->tables = realloc(catalog->tables, sizeof(Table) * (catalog->tables_size + 1));
		table_index = catalog->tables_size;
		catalog->tables_size++;
	}
	//assumes table is valid to add
	catalog->tables[table_index] = table;
	
	int * data_types = malloc(sizeof(int) * table->attrs_size);
	for(int i = 0; i < table->attrs_size; i++)
		data_types[i] = get_type_num(get_attr_type(table->attrs[i]));
	
	Unique pk = get_primary_key(table);
	int key_indices_size = get_unique_attrs_size( pk );
	int * key_indices = malloc(sizeof(int) * key_indices_size);
	
	for(int i = 0; i < key_indices_size; i++)
		key_indices[i] = get_attr_position(pk->attrs[i]);
	
	int table_number = add_table(data_types, key_indices, table->attrs_size, key_indices_size);
	
	if(table_number == -1){
		//error
		return -1;
	}
	
	table->num = table_number;
	
	return table_number;
}

void print_schema(){
	printf("Database Schema:\n");
	printf("Location: %s\n", catalog->db_loc);
	
	if(catalog->tables_size == 0){
		printf("No tables in the database\n");
	}
	else{
		for(int i = 0; i < catalog->tables_size; i++){
			if(catalog->tables[i] != NULL)
				print_table_schema(catalog->tables[i]);
		}
	}
}