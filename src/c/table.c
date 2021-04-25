
#include "../headers/table.h"
#include <string.h>
#include <stdio.h>
#include "../headers/catalog.h"
#include <stdlib.h>
#include "../headers/storagemanager.h"

Table create_table( int num, char * name ){
	Table table = malloc(sizeof(struct table_));
	table->num = num;
	table->name = malloc(strlen(name) + 1);
	strcpy(table->name, name);
	table->attrs = 0; //NULL
	table->attrs_size = 0;
	table->primary_key = 0; //NULL
	table->uniques = 0; //NULL
	table->uniques_size = 0;
	table->fks = 0; //NULL
	table->fks_size = 0;
	table->ref_fks = 0; //NULL
	table->ref_fks_size = 0;
	return table;
}

char * get_name( Table table ){
	return table->name;
}

int get_num( Table table ){
	return table->num;
}

void set_num( Table table, int num ){
	table->num = num;
}

Attr * get_attrs( Table table ){
	return table->attrs;
}	

Attr get_attr_by_position( Table table, int position ){
	return table->attrs[position];
}

Attr get_attr_by_name( Table table, char * name ){
	for(int i = 0; i < table->attrs_size; i++){
		if(strcmp(name, table->attrs[i]->name) == 0)
			return table->attrs[i];
	}
	return 0;
}

Unique * get_uniques( Table table ){
	return table->uniques;
}

int get_uniques_size( Table table ){
	return table->uniques_size;
}

Unique get_primary_key( Table table ){
	return table->primary_key;
}

void set_primary_key ( Table table, Unique pk ){
	table->primary_key = pk;
}

Foreign_Key * get_foreign_keys( Table table ){
	return table->fks;
}

int get_foreign_keys_size( Table table ){
	return table->fks_size;
}

int add_attr( Table table, Attr attr ){
	//look for attr with same name
	//return -1 (error)
	if(get_attr_by_name(table, attr->name) != 0){
		fprintf(stderr, "ERROR: Table %s already has attr named %s\n", table->name, attr->name);
		return -1;
	}
	
	table->attrs = realloc(table->attrs, sizeof(Attr) * (table->attrs_size + 1));
	table->attrs[table->attrs_size] = attr;
	set_attr_postion( attr, table->attrs_size );
	table->attrs_size++;
	
	return 0;
}

void update_uniques( Table table, Attr attr ){
	int count = 0;
	for(int i = 0; i < table->uniques_size; i++){
		if(unique_contains_attr(table->uniques[i], attr)){
			table->uniques[i] = 0; // null it for now
		}
		else{
			count++;
		}
	}
	
	if(count == 0){
		table->uniques = NULL;
	}
	else{
		Unique * temp = malloc(sizeof(Unique) * count);
		int index = 0;
		for(int i = 0; i < table->uniques_size; i++){
			if(table->uniques[i] == 0){
				continue;
			}
			temp[index] = table->uniques[i];
			index++;
		}
		table->uniques = temp;
		table->uniques_size = count;
	}
}

void update_ref_fks( Table table, Table refer_table, Attr attr ){
	int count = 0;
	for(int i = 0; i < table->fks_size; i++){
		if(fk_contains_ref_attr(table->fks[i], refer_table->name, attr)){
			table->fks[i] = 0; // null it for now
		}
		else{
			count++;
		}
	}
	
	if(count == 0){
		table->fks = NULL;
		table->fks_size = 0;
	}
	else{
		Foreign_Key * temp = malloc(sizeof(Foreign_Key) * count);
		int index = 0;
		for(int i = 0; i < table->fks_size; i++){
			if(table->fks[i] == 0){
				continue;
			}
			temp[index] = table->fks[i];
			index++;
		}
		table->fks = temp;
		table->fks_size = count;
	}
}

void update_fks( Table table, Attr attr ){
	int count = 0;
	for(int i = 0; i < table->fks_size; i++){
		if(fk_contains_attr(table->fks[i], attr)){
			table->fks[i] = 0; // null it for now
		}
		else{
			count++;
		}
	}
	
	if(count == 0){
		table->fks = 0;
	}
	else{
		Foreign_Key * temp = malloc(sizeof(Foreign_Key) * count);
		int index = 0;
		for(int i = 0; i < table->fks_size; i++){
			if(table->fks[i] == 0){
				continue;
			}
			temp[index] = table->fks[i];
			index++;
		}
		table->fks = temp;
		table->fks_size = count;
	}
	
	for(int i = 0; i < table->ref_fks_size; i++){
		Table ref = get_table_by_name(table->ref_fks[i]);
		update_ref_fks(ref , table, attr);
	}
}

int drop_attr( Table table, Attr attr ){
	if(unique_contains_attr(table->primary_key, attr)){
		return -1;
	}
	//assumes the attr exists
	for(int i = attr->position; i < table->attrs_size - 1; i++){
		table->attrs[i]->position--;
		table->attrs[i] = table->attrs[i+1];
	}
	table->attrs_size--;
	table->attrs = realloc(table->attrs, sizeof(Attr) * table->attrs_size);

	update_uniques(table, attr);
	update_fks(table, attr);
	
	union record_item ** records = NULL;
	
	int size = get_records(table->num, &records);
	drop_table(table->num);
	
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
	
	for(int i = 0; i < size; i++){
		int index = 0;
		union record_item * new_record = malloc(sizeof(union record_item) * table->attrs_size);
		for(int j = 0; j <= table->attrs_size; j++){
			if(j != attr->position){
				new_record[index] = records[i][j];
				index++;
			}
		}
		
		insert_record(table_number, new_record);
	}
	
	return 0;
}

int add_unique( Table table, Unique unique ){
	//check for duplicate unique
	for(int i = 0; i < table->uniques_size; i++){
		if(unique_equality(table->uniques[i], unique))
			return -1;
	}
	
	table->uniques = realloc(table->uniques, sizeof(Unique) * (table->uniques_size + 1));
	table->uniques[table->uniques_size] = unique;
	table->uniques_size++;
	return 0;
}

void add_fk_ref( Table table, Table reffering){
	if(table->ref_fks == NULL)
		table->ref_fks = malloc(sizeof(char *));
	else
		table->ref_fks = realloc(table->ref_fks, sizeof(char *) * (table->ref_fks_size + 1));
	table->ref_fks[table->ref_fks_size] = reffering->name;
	table->ref_fks_size++;
}

int add_foreign_key( Table table, Foreign_Key fk ){
	for(int i = 0; i < table->fks_size; i++){
		if(fk_equality(table->fks[i], fk))
			return -1;
	}
	
	if(table->fks == NULL)
		table->fks = malloc(sizeof(Foreign_Key));
	else
		table->fks = realloc(table->fks, sizeof(Foreign_Key) * (table->fks_size + 1));
	table->fks[table->fks_size] = fk;
	table->fks_size++;
	
	Table ref_table = get_table_by_name( fk->ref_table_name );
	add_fk_ref( ref_table, table);
	
	return 0;
}

void print_table_schema( Table table ){
	printf("Table %s(%d):\n", table->name, table->num);
	for(int i = 0; i < table->attrs_size; i++){
		printf("\t");
		print_attr_schema(table->attrs[i]);
		printf("\n");
	}
	for(int i = 0; i < table->uniques_size; i++){
		printf("\t");
		print_unique_schema(table->uniques[i]);
		printf("\n");
	}
	for(int i = 0; i < table->fks_size; i++){
		printf("\t");
		print_fk_schema(table->fks[i]);
		printf("\n");
	}
}
	