

#include "unique.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Unique create_unique( Attr * attrs, int attr_size, bool pk ){
	Unique unique = malloc(sizeof(struct unique_));
	unique->attrs = attrs;
	unique->attrs_size = attr_size;
	unique->is_pk = pk;
	return unique;
}

Attr * get_unique_attrs( Unique unique ){
	return unique->attrs;
}

bool unique_contains_attr( Unique unique, Attr attr){
	for(int i = 0; i < unique->attrs_size; i++){
		if(strcmp(unique->attrs[i]->name, attr->name) == 0)
			return true;
	}
	
	return false;
}

int get_unique_attrs_size( Unique unique ){
	return unique->attrs_size;
}

bool is_primary_key( Unique unique ){
	return unique->is_pk;
}

bool unique_equality( Unique u1, Unique u2 ){
	//same if they have the same attr in the same order
	if(u1->attrs_size != u2->attrs_size)
		return false;
	
	for(int i = 0; i < u1->attrs_size; i++){
		if(u1->attrs[i]->position != u2->attrs[i]->position)
			return false;
	}
	
	return true;
}

void print_unique_schema( Unique unique ){
	if(unique->is_pk)
		printf("primarykey( ");
	else
		printf("unique( ");
	
	for(int i = 0; i < unique->attrs_size; i++){
		printf("%s(%d) ", unique->attrs[i]->name, unique->attrs[i]->position);
	}
	
	printf(")");
}
	