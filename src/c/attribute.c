/**
defines the functionality for an Attribute

most of these functions are just accessing parts of the
struct and can be done outside of this file directly, but
this helps will self-documenting.
**/
#include "attribute.h"
#include "type.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Attr create_attr( char * name, int position,
                  Type type, bool notnull){
    Attr attr = malloc(sizeof(struct attr_));
	attr->name = malloc(strlen(name) + 1);
	stpcpy(attr->name, name);
	attr->position = position;
	attr->type = type;
	attr->notnull = notnull;
	return attr;
}

char * get_attr_name( Attr attr ){
	char * name = malloc(strlen(attr->name) + 1);
	stpcpy(name, attr->name);
	return name;
}

int get_attr_position( Attr attr ){
	return attr->position;
}

void set_attr_postion( Attr attr, int position ){
	attr->position = position;
}

Type get_attr_type( Attr attr ){
	return attr->type;
}

bool get_attr_notnull( Attr attr ){
	return attr->notnull;
}

void print_attr_schema( Attr attr ){
	printf("%s(%d): ", attr->name, attr->position);
	print_type_schema(attr->type);
	if(attr->notnull){
		printf(" notnull");
	}
}