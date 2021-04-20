#include "ddl_parser.h"
#include <stdio.h>
#include <stdbool.h>
#include "catalog.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "foreign_key.h"
#include "unique.h"
#include "attribute.h"
#include "utils.h"

int parse_type_str( char * t_str );

int parse_alter_table( char * statement ){
	char * name;
	
	name = strtok(statement, " "); //drop (already verified in parse_ddl)
	name = strtok(NULL, " "); //table
	
	if(strcmp(name, "table") != 0){
		fprintf(stderr, "ERROR: invalid drop command: missing table\n");
		return -1;
	}
	name = strtok(NULL, " "); //table namespace
	
	if(!table_name_exists( name )){
		fprintf(stderr, "ERROR altering table: No such table exits: %s\n", name);
		return -1;
	}
	
	Table table = get_table_by_name(name);
	
	char * type = strtok(NULL, " ");
	
	if(strcmp(type, "add") == 0){
		char * attr_name = strtok(NULL, " ;");
		Attr attr = get_attr_by_name(table, attr_name);
		if(attr != NULL){
			fprintf(stderr, "ERROR: Attribute already exists: %s\n", attr_name);
			return -1;
		}
		
		char * t_str = strtok(NULL, " ;");
		int type_result = parse_type_str( t_str );
		if(type_result == -1){
			return -1;
		}
		
		int type_num = type_str_to_num( t_str);
	
		Type type = create_type( type_num, type_result );
		
		attr = create_attr( attr_name, -1, type, false );
		
		char * default_val = strtok(NULL, " ;");
		if(default_val != NULL && (strcmp(default_val, "default") != 0)){
			fprintf(stderr, "ERROR: Invalid alter statement\n");
			return -1;
		}
		else if( default_val != NULL){
			default_val = strtok(NULL, ";");
			//TODO: Handle default values
		}
		
		if(add_attr(table, attr) == -1){
			return -1;
		}
		
		//TODO Update storagemanger
		
		return 0;
	}
	else if(strcmp(type, "drop") == 0){
		char * attr_name = strtok(NULL, ";");
		Attr attr = get_attr_by_name(table, attr_name);
		if(attr == NULL){
			fprintf(stderr, "ERROR: No such attribute %s\n", attr_name);
			return -1;
		}
		if(unique_contains_attr(get_primary_key(table), attr)){
			fprintf(stderr, "ERROR: Cannot drop primarykey attribute %s\n", attr_name);
			return -1;
		}
		return drop_attr(table, attr);
	}
	else{
		fprintf(stderr, "ERROR: Invalid alter type\n");
		return -1;
	}
	
	return 0;
}

int parse_drop_table( char * statement ){
	char * name;
	
	name = strtok(statement, " "); //drop (already verified in parse_ddl)
	name = strtok(NULL, " "); //table
	
	if(strcmp(name, "table") != 0){
		fprintf(stderr, "ERROR: invalid drop command: missing table\n");
		return -1;
	}
	name = strtok(NULL, " "); //table namespace
	
	//check for and remove ;
	if(name[strlen(name)-1] != ';'){
		fprintf(stderr, "ERROR: missing semicolon\n");
		return -1;
	}
	else
		name[strlen(name)-1] = '\0';
	
	if(!table_name_exists( name )){
		fprintf(stderr, "ERROR dropping table: No such table exits: %s\n", name);
		return -1;
	}
	
	return drop_table_catalog(get_table_by_name(name)->num);
}

int parse_foreignkey( Table table, char * fk_str){
	char * fk_cpy = malloc(strlen(fk_str) + 1);
	strcpy(fk_cpy, fk_str);
	
	char * curr_attr_str;
	curr_attr_str = strtok(fk_cpy, " "); // throw away unique( / primarykey( already checked in parse_attr_constraint
	curr_attr_str = strtok(NULL, ")");
	
	char * reference_name;
	reference_name = strtok(NULL, " "); // "references"
	
	if(strcmp(reference_name, "references") != 0){
		fprintf(stderr, "ERROR: fk missing references keyword: %s\n", fk_str);
		return -1;
	}
	
	reference_name = strtok(NULL, "("); // get ref table name
	
	Table ref_table = get_table_by_name( reference_name );
	
	if(ref_table == NULL){
		fprintf(stderr, "ERROR: no such reference table: %s\n", fk_str);
		return -1;
	}
	
	char * ref_attr_str;
    ref_attr_str = strtok(NULL, ")");
	
	if(strtok(NULL, ") (") != NULL){
		fprintf(stderr, "ERROR: invalid fk: %s\n", fk_str);
		return -1;
	}
	
	//parse attrs
	char * attr_str = strtok( curr_attr_str, " ");
	Attr * cur_attrs = malloc(sizeof(Attr));
	Attr attr = get_attr_by_name( table, attr_str );
	if(attr == NULL){
		fprintf(stderr, "ERROR parsing fk: No such attr exits: %s\n", attr_str);
		return -1;
	}
	cur_attrs[0] = attr;
	int cur_attrs_size = 1;
	while( (attr_str = strtok(NULL, " ")) != NULL){
		cur_attrs = realloc(cur_attrs, sizeof(Attr) * (cur_attrs_size + 1));
		attr = get_attr_by_name( table, attr_str );
		if(attr == NULL){
			fprintf(stderr, "ERROR parsing pk/unique: No such attr exits: %s\n", attr_str);
			return -1;
		}
		cur_attrs[cur_attrs_size] = attr;
		cur_attrs_size++;
	}
	
	attr_str = strtok( ref_attr_str, " ");
	Attr * ref_attrs = malloc(sizeof(Attr));
	attr = get_attr_by_name( ref_table, attr_str );
	if(attr == NULL){
		fprintf(stderr, "ERROR parsing fk: No such attr exits: %s\n", attr_str);
		return -1;
	}
	ref_attrs[0] = attr;
	if(get_type_num(get_attr_type(attr)) != get_type_num(get_attr_type(cur_attrs[0]))){
		fprintf(stderr, "ERROR: data types of fk attrs do not match\n");
		return -1;
	}
	int ref_attrs_size = 1;
	while( (attr_str = strtok(NULL, " ")) != NULL){
		ref_attrs = realloc(cur_attrs, sizeof(Attr) * (cur_attrs_size + 1));
		attr = get_attr_by_name( ref_table, attr_str );
		if(attr == NULL){
			fprintf(stderr, "ERROR parsing fk: No such attr exits: %s\n", attr_str);
			return -1;
		}
		if(get_type_num(get_attr_type(attr)) != get_type_num(get_attr_type(cur_attrs[ref_attrs_size]))){
			fprintf(stderr, "ERROR: data types of fk attrs do not match\n");
			return -1;
		}
	    ref_attrs[cur_attrs_size] = attr;
		ref_attrs_size++;
	}
	
	if(cur_attrs_size != ref_attrs_size){
		fprintf(stderr, "ERROR: fk number of attrs different: %s\n", fk_str);
		return -1;
	}
	
	Foreign_Key fk = create_foreign_key( get_num( ref_table), get_name(ref_table),
								ref_attrs, cur_attrs, ref_attrs_size);
	if(add_foreign_key( table, fk ) != 0){
		fprintf(stderr, "ERROR: failed to add fk: %s\n", fk_str);
		return -1;
	}
	
	return 0;
}

int parse_unique( Table table, char * u_str, bool pk){
	//copy u_str because strtok can destroy it
	char * u_cpy = malloc(strlen(u_str) + 1);
	strcpy(u_cpy, u_str);
	if(pk && get_primary_key(table) != 0){
		fprintf(stderr, "ERROR parsing pk: pk already exists\n");
		return -1;
	}
	
	char * token;
	token = strtok(u_cpy, " "); // throw away unique( / primarykey( already checked in parse_attr_constraint
	token = strtok(NULL, ")");
	
	char * attr_str = strtok( token, " ");
	Attr * cur_attrs = malloc(sizeof(Attr));
	Attr attr = get_attr_by_name( table, attr_str );
	if(attr == NULL){
		fprintf(stderr, "ERROR parsing pk/unique: No such attr exits: %s\n", attr_str);
		return -1;
	}
	cur_attrs[0] = attr;
	int cur_attrs_size = 1;
	while( (attr_str = strtok(NULL, " ")) != NULL){
		cur_attrs = realloc(cur_attrs, sizeof(Attr) * (cur_attrs_size + 1));
		attr = get_attr_by_name( table, attr_str );
		if(attr == NULL){
			fprintf(stderr, "ERROR parsing pk/unique: No such attr exits: %s\n", attr_str);
			return -1;
		}
		cur_attrs[cur_attrs_size] = attr;
		cur_attrs_size++;
	}

	Unique unique = create_unique( cur_attrs, cur_attrs_size, pk );
	
	if(add_unique(table, unique) != 0){
		//error
		fprintf(stderr, "unique already exists: %s\n", u_str);
		return -1;
	}
	if(pk)
		set_primary_key( table, unique );
	
	return 0;
}

int parse_type_str( char * t_str ){
	if(strcmp(t_str, "integer") == 0 
	          || strcmp(t_str, "double") == 0 
			  || strcmp(t_str, "boolean") == 0)
		return 0;
	if(prefix("char(", t_str) || prefix("varchar(", t_str)){
		int pos = (int)(strchr(t_str, '(') - t_str) + 1;
		char * ptr;
		long value = strtol(t_str + pos, &ptr, 10);
		if(ptr == t_str){
			fprintf(stderr, "ERROR char/varchar: missing character count\n");
			return -1;
		}
		
		if(strcmp(ptr, ")") != 0){
			fprintf(stderr, "ERROR parsing char/varchar: missing closing )\n");
			return -1;
		}
		
		return value;
	}
	
	fprintf(stderr, "ERROR parsing data type: No such type: %s\n", t_str);
	return -1;
}
		

int parse_attr( Table table, char * attr_str ){
	bool is_pk = false;
	bool is_notnull = false;
	bool is_unique = false;
	char * token;
	char * name;
	char * t_str;
	name = strtok(attr_str, " ");
	
	//todo validate name
	
	t_str = strtok(NULL, " ");
	
	//todo validate type
	
	int type_result = parse_type_str(t_str);
	
	if(type_result < 0){
		return -1;
	}
	
	int type_num = type_str_to_num( t_str);
	
	Type type = create_type( type_num, type_result );
	
	while((token = strtok(NULL, " ")) != NULL){
		if(strcmp(token, "primarykey") == 0)
			is_pk = true;
		else if(strcmp(token, "unique") == 0)
			is_unique = true;
		else if(strcmp(token, "notnull") == 0)
			is_notnull = true;
		else{
			fprintf(stderr, "ERROR parsing attr: Ivalid constraint %s\n", token);
			return -1;
		}
	}
	
	Attr attr = create_attr( name, -1, type, is_notnull );
	if(add_attr(table, attr) < 0){
		fprintf(stderr, "ERROR parsing attr: failed to add attr %s\n", name);
		return -1;
	}
	
	if(is_pk){
		char * pk_str = malloc(strlen(name) + 15);
		sprintf(pk_str, "primarykey( %s )", name);
		int pk_result = parse_unique(table, pk_str, true);
		if(pk_result == -1){
			return -1;
		}
	}
	
	if(is_unique){
		char * u_str = malloc(strlen(name) + 15);
		sprintf(u_str, "unique( %s )", name);
		int u_result = parse_unique(table, u_str, false);
		if(u_result == -1){
			fprintf(stderr, "unique already exists: %s\n", u_str);
			return -1;
		}
	}

	return 0;
}

int parse_attr_constraint( Table table, char * token ){
	if(prefix("primarykey(", token))
		return parse_unique(table, token, true);
	if(prefix("unique(", token))
		return parse_unique(table, token, false);
	if(prefix("foreignkey(", token))
		return parse_foreignkey(table, token);
	return parse_attr( table, token);
}

int parse_create_table( char * statement ){
	char * token;
	char * name;
	char ** attr_constraints = NULL;
	int attr_constraints_size = 0;

	token = strtok(statement, " "); //create (already verified in parse_ddl)
	token = strtok(NULL, " "); //table
	name = strtok(NULL, "("); //table namespace
	name = trimwhitespace(name);
	
	if(get_table_by_name( name ) != NULL){
		fprintf(stderr, "ERROR parsing create table: table name must be unique %s\n", name);
		return -1;
	}
	
	Table table = create_table(-1, name);
	
	while((token = strtok(NULL, ",")) != NULL){
		if(attr_constraints_size == 0)
			attr_constraints = malloc(sizeof(char *));
		else
			attr_constraints = realloc(attr_constraints, sizeof(char *) * (attr_constraints_size + 1));
		attr_constraints[attr_constraints_size] = trimwhitespace(token);
		attr_constraints_size++;
	}
	
	attr_constraints[attr_constraints_size-1][strlen(attr_constraints[attr_constraints_size-1]) - 2] = '\0';
	
	int result = 0;
	for(int i = 0; i < attr_constraints_size; i++){
		result = parse_attr_constraint(table, attr_constraints[i]);
		if(result == -1)
			return -1;
	}
	
	add_tble_to_catalog(table);
	print_table_schema(table);
	return result;
}

int parse_ddl_statement( char * statement ){
	printf("%s\n", statement);
	if(prefix("create table", statement))
		return parse_create_table(statement);
	else if(prefix("drop table", statement))
		return parse_drop_table(statement);
	else if(prefix("alter table", statement))
		return parse_alter_table(statement);
	else{
		fprintf(stderr, "ERROR parsing ddl statement: Invalid ddl statement %s\n", statement);
		return -1;
	}
}