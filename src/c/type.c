
#include "type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

Type create_type(int type_num, int num_chars){
	Type new_type = malloc(sizeof(struct type_));
	new_type->type_num = type_num;
	new_type->num_chars = num_chars;
	return new_type;
}

int get_type_num( Type type ){
	return type->type_num;
}

char * get_type_string( Type type ){
	switch(type->type_num){
		case 0:
		    return "integer";
		case 1:
		    return "double";
		case 2:
		    return "boolean";
		case 3:
		    return "char";
		case 4:
		    return "varchar";
		default:
		    return "error";
	}
}

int get_num_chars( Type type ){
	return type->num_chars;
}

void print_type_schema( Type type ){
	switch(type->type_num){
		case 0:
		    printf("integer");
			return;
		case 1:
		    printf("double");
			return;
		case 2:
		    printf("boolean");
			return;
		case 3:
		    printf("char(%d)", type->num_chars);
			return;
		case 4:
		    printf("varchar(%d)", type->num_chars);
			return;
		default:
		    printf("error");
	}
}

int type_str_to_num( char * t_str){
	if(strcmp(t_str, "integer") == 0)
		return 0;		
	if(strcmp(t_str, "double") == 0)
		return 1;
	if(strcmp(t_str, "boolean") == 0)
		return 2;
	if(prefix("char(", t_str))
		return 3;
	if(prefix("varchar(", t_str))
		return 4;
		
	return -1;
}