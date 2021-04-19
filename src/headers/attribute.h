
#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "type.h"
#include <stdbool.h>

typedef struct attr_{
	char * name;
	int position;
	Type type;
	bool notnull;
} *Attr;





Attr create_attr( char * name, int position,
                  Type type, bool notnull);
				  
char * get_attr_name( Attr attr );

int get_attr_position( Attr attr );

void set_attr_postion( Attr attr, int position );

Type get_attr_type( Attr attr );

bool get_attr_notnull( Attr attr );

void print_attr_schema( Attr attr );

#endif