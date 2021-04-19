
#ifndef FORGEIGN_KEY_H
#define FORGEIGN_KEY_H

#include <stdbool.h>
#include "attribute.h"

typedef struct f_key_{
	int ref_table_num;
	char * ref_table_name;
	Attr * ref_table_attrs;
	Attr * cur_table_attrs;
	int num_attrs;
}* Foreign_Key;

Foreign_Key create_foreign_key( int ref_table, char * ref_table_name, 
								Attr * ref_table_attrs, Attr * cur_table_attrs, 
								int num_attrs);

int get_ref_table( Foreign_Key fk );

char * get_ref_table_name( Foreign_Key fk );

Attr * get_ref_attrs( Foreign_Key fk );

Attr * get_cur_attrs( Foreign_Key fk );

int get_num_attrs( Foreign_Key fk );

bool fk_contains_attr( Foreign_Key fk, Attr attr);

bool fk_contains_ref_attr( Foreign_Key fk, char * table, Attr attr);

void print_fk_schema( Foreign_Key fk );

bool fk_equality( Foreign_Key fk1, Foreign_Key fk2 );

#endif