
#include "foreign_key.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Foreign_Key create_foreign_key( int ref_table_num, char * ref_table_name,
								Attr * ref_table_attrs, Attr * cur_table_attrs, 
								int num_attrs){
	Foreign_Key fk = malloc(sizeof(struct f_key_));
	fk->ref_table_num = ref_table_num;
	fk->ref_table_name = malloc(strlen(ref_table_name) + 1);
	strcpy(fk->ref_table_name, ref_table_name);
	fk->ref_table_attrs = ref_table_attrs;
	fk->cur_table_attrs = cur_table_attrs;
	fk->num_attrs = num_attrs;
	return fk;
}

int get_ref_table( Foreign_Key fk ){
	return fk->ref_table_num;
}

char * get_ref_table_name( Foreign_Key fk ){
	return fk->ref_table_name;
}

Attr * get_ref_attrs( Foreign_Key fk ){
	return fk->ref_table_attrs;
}

Attr * get_cur_attrs( Foreign_Key fk ){
	return fk->cur_table_attrs;
}

int get_num_attrs( Foreign_Key fk ){
	return fk->num_attrs;
}

bool fk_contains_attr( Foreign_Key fk, Attr attr){
	for(int i = 0; i < fk->num_attrs; i++){
		if(strcmp(fk->cur_table_attrs[i]->name, attr->name) == 0)
			return true;
	}
	
	return false;
}

bool fk_contains_ref_attr( Foreign_Key fk, char * table, Attr attr){
	printf("Table2: %s\n", table);
	if(strcmp(fk->ref_table_name, table) != 0)
		return false;
	
	//NULL will assume match any.
	if(attr == NULL)
		return true;
	
	for(int i = 0; i < fk->num_attrs; i++){
		if(strcmp(fk->ref_table_attrs[i]->name, attr->name) == 0)
			return true;
	}
	
	return false;
}

void print_fk_schema( Foreign_Key fk ){
	printf("foreignkey( ");
	
	for(int i = 0; i < fk->num_attrs; i++){
		printf("%s(%d) ", fk->cur_table_attrs[i]->name, fk->cur_table_attrs[i]->position);
	}
	
	printf(") references %s( ", fk->ref_table_name);
	
	for(int i = 0; i < fk->num_attrs; i++){
		printf("%s(%d) ", fk->ref_table_attrs[i]->name, fk->ref_table_attrs[i]->position);
	}
	
	printf(")");
}

bool fk_equality( Foreign_Key fk1, Foreign_Key fk2 ){
	if(fk1->ref_table_num != fk2->ref_table_num){
		return false;
	}
	
	for(int i = 0; i < fk1->num_attrs; i++){
		if(fk1->ref_table_attrs[i]->position != fk2->ref_table_attrs[i]->position)
			return false;
	}
	
	for(int i = 0; i < fk1->num_attrs; i++){
		if(fk1->cur_table_attrs[i]->position != fk2->cur_table_attrs[i]->position)
			return false;
	}
	
	return true;
}