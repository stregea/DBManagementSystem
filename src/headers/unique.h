
#ifndef UNIQUE_H
#define UNIQUE_H

#include "attribute.h"

typedef struct unique_{
	Attr * attrs;
	int attrs_size;
	bool is_pk;
} *Unique;

Unique create_unique( Attr * attrs, int attr_size, bool pk );

bool unique_contains_attr( Unique unique, Attr attr);

Attr * get_unique_attrs( Unique unique );

int get_unique_attrs_size( Unique unique );

bool is_primary_key( Unique unique );

bool unique_equality( Unique u1, Unique u2 );

void print_unique_schema( Unique unique );

#endif