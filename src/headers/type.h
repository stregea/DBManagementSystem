/**
Type.h - defines the type struct and its functionality
**/

#ifndef TYPE_H
#define TYPE_H

/*
Types have a type:
-integer(0)
-double(1)
-boolean(2)
-char(3)
-varchar(4)

char/varchar need a character count
*/
typedef struct type_{
	int type_num;
	int num_chars;
} *Type;

/*
This will create a new type pointer.
@param: type - integer reprsenting the type.
@param: num_chars - integer reprsenting the number of characters this type can store.
                    only needed for chars/varchars
@return a Type pointer to the type created. The caller is responsible for freeing.
*/
Type create_type(int type, int num_chars);

/*
Will return the integer representing the type of the Type passed in.
@param type - the Type pointer to get the type of,
@return the integer representing the type (see above)
*/
int get_type_num( Type type );

/*
Will return the string representing the type of the Type passed in.
@param type - the Type pointer to get the type of,
@return the string representing the type (see above)
*/
char * get_type_string( Type type );

/*
Will return the integer representing the number of characters of the Type passed in.
@param type - the Type pointer to get the type of,
@return the integer representing the number of characters in the type
*/
int get_num_chars( Type type );

void print_type_schema( Type type );

int type_str_to_num( char * t_str);

#endif
