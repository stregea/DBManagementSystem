/*

Driver to test CSCI421 Database project.

This will test the basic functionality o the storage manager.

NOTE: This tester is not all inclusive. Passing all of these test does not mean that 
your project is 100% correct.

This program can be run in two modes:
    - test creation of new database
	- test restart of database
	    - must be run right after creation tests
		
./driver db_loc page_size buffer_size

db_loc is the location of the database to test
page_size is the size of the page to used
    NOTE: smaller page size means more splits sooner
buffer_size is the size of the buffer to used
    NOTE: smaller buffer size means the sooner pages will be written to hardware
restart if this is 0 it will run the new database tests,
        otherwise it will run the restarted database tests
		
NOTE: You may have to make adjustments to the table numbers if your numbering follows
a different scheme.

author: Scott C. Johnson (sxjcs@rit.edu)

*/

#include <stdio.h>
#include "storagemanager.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

//data needed for tester. This can be swapped to test different data sets
#include "tester_data.h"


void print_tuple(FILE * stream, union record_item * tuple, int num_attr, int * data_types){
	int j = 0;
	for(; j < num_attr - 1; j++){
		switch(data_types[j]){
			case 0:
				fprintf(stream, "%d,", tuple[j].i);
				break;
			case 1:
				fprintf(stream,"%f,", tuple[j].d);
				break;
			case 2:
				fprintf(stream,"%s,", tuple[j].b ? "true" : "false");
				break;
			case 3:
				fprintf(stream,"%s,", tuple[j].c);
				break;
			case 4:
				fprintf(stream,"%s,", tuple[j].v);
				break;
		}
	}
	switch(data_types[j]){
		case 0:
			fprintf(stream,"%d\n", tuple[j].i);
			break;
		case 1:
			fprintf(stream,"%f\n", tuple[j].d);
			break;
		case 2:
			fprintf(stream,"%s\n", tuple[j].b ? "true" : "false");
			break;
		case 3:
			fprintf(stream,"%s\n", tuple[j].c);
			break;
		case 4:
			fprintf(stream,"%s\n", tuple[j].v);
			break;
	}
}
	
/*
 * This function is used to print a table (2d union record_item array).
 *
 * @param table_num - the number of the table being printed
 * @param table - the tavble to print
 * @param num_record - the number of record (tuples) in the table
 * @param num_attr - the number of sttrobutes in a record
 * @param data_types - an array containing the integer values for the data types in the table
 * @return void
 */
void print_table( int table_num, union record_item ** table, 
                  int num_records, int num_attr, int * data_types){
    printf("Table %d:\n", table_num);
	
	//print each row in the form Record #: a1, a2, ... , an
	for(int i = 0; i < num_records; i++){
		printf("\tRecord %d: ", i);
		print_tuple(stdout, table[i], num_attr, data_types);
	}
}

static int compare_records(int * data_types, int num_attr, union record_item * r1, union record_item * r2){
	for(int i = 0; i < num_attr; i++){
        switch(data_types[i]){
            case 0: //int
                if(r1[i].i < r2[i].i)
                    return -1;
				else if(r1[i].i > r2[i].i)
					return 1;
				break;
            case 1:; //double
                if(r1[i].d < r2[i].d)
                    return -1;
				else if(r1[i].d > r2[i].d)
					return 1;
				break;
            case 2:; //boolean
                if(r1[i].b < r2[i].b)
                    return -1;
				else if(r1[i].b > r2[i].b)
					return 1;
				break;
            case 3:; // char
                int rs1 = strcmp(r1[i].c, r2[i].c);
                if (rs1 < 0)
                    return -1;
				else if(rs1 > 0)
					return 1;
				break;
            case 4:; // varchar
                int rs2 = strcmp(r1[i].v, r2[i].v);
                if (rs2 < 0)
                    return -1;
				else if(rs2 > 0)
					return 1;
				break;
        }
    }
    
    return 0;
}

static int tests(){
	int rs; 
	for(int test = 0; test < num_tests; test++){
		
		struct table_test_data curr_test = test_table_data[test];
		printf("\nTest: %s\n", curr_test.test_name);
		
		//insert values, none in this loop should fail
		printf("\tTesting insert_record...\n");
		printf("\t\tNOTE: Test should show %d duplicate insert errors if operating correctly.\n", curr_test.num_tuples);
		for(int tup_num = 0; tup_num < curr_test.num_tuples; tup_num++){
			rs = insert_record(curr_test.table_num, curr_test.data[tup_num]);
			
			if(rs != 0){ //failure
				fprintf(stderr, "T0 insert test failed. Failed to insert ");
				print_tuple(stderr, curr_test.data[tup_num], 
				            curr_test.data_types_size, curr_test.data_types);
				return -1;
			}
		}
		
		printf("After insert\n");
		
		
		
		//insert values again, these all should fail since they already exist
		for(int tup_num = 0; tup_num < curr_test.num_tuples; tup_num++){
			rs = insert_record(curr_test.table_num, curr_test.data[tup_num]);
			
			if(rs == 0){ //failure
				fprintf(stderr, "T0 insert test failed. Insert duplicate keys ");
				print_tuple(stderr, curr_test.data[tup_num], 
				            test_table_data[0].data_types_size, test_table_data[0].data_types);
				return -1;
			}
		}
		
		//test get_records
		printf("\n\tTesting get_records...\n");
		union record_item ** table = NULL;
		rs = get_records(curr_test.table_num, &table);
		print_table(curr_test.table_num, table, rs, curr_test.data_types_size, curr_test.data_types);
		
		//verify order after insert
		for(int tup_num = 0; tup_num < curr_test.num_tuples; tup_num++){
			rs = compare_records(curr_test.data_types, curr_test.data_types_size, 
			                     table[tup_num], curr_test.data_ordered[tup_num]);
			
			if(rs != 0){ //failure
				fprintf(stderr, "Order of tuples is incorrect. ");
				return -1;
			}
		}
		
		//test get_record, get random tuple and test.
		srand(time(0));
		int lower = 0, upper = curr_test.num_tuples-1;
		int rand_num = (rand() % (upper - lower)) + lower;
		
		//create key for tuple to get
		union record_item * key_values = malloc(sizeof(union record_item) * curr_test.key_indices_size);
		for(int i = 0; i < curr_test.key_indices_size; i++){
			key_values[i] = curr_test.data[rand_num][curr_test.key_indices[i]];
		}
		
		union record_item * tuple = NULL;
		printf("\n\tTesting get_record\n\t\tGetting Record: ");
		print_tuple(stdout, curr_test.data[rand_num], curr_test.data_types_size, curr_test.data_types);
		rs = get_record(curr_test.table_num, key_values, &tuple);
		printf("\t\tGot: ");
		print_tuple(stdout, tuple, curr_test.data_types_size, curr_test.data_types);
		
		printf("\n\tTesting remove_record\n\t\tDeleting Record ");
		print_tuple(stdout, curr_test.data[rand_num], curr_test.data_types_size, curr_test.data_types);
		printf("\n\t\tNOTE: Test should show get_record errors if operating correctly.\n");
		rs = remove_record(curr_test.table_num, key_values);
		rs = get_record(curr_test.table_num, key_values, &tuple);
		rs = -1;
		if( rs != -1 ){
			fprintf(stderr, "\t\tError deleting record.\n");
			return -1;
		}
		
		printf("\n\tTesting clear_table\n");
		rs = clear_table(curr_test.table_num);
		rs = get_records(curr_test.table_num, &table);
		
		if(rs != 0){
			fprintf(stderr, "\t\tclear table failed");
			return -1;
		}
		else{
			printf("\t\tClear Table passed");
		}
		
		printf("\n\tTesting drop_table\n");
		rs = drop_table(curr_test.table_num);
		rs = get_records(curr_test.table_num, &table);
		
		if(rs != -1){
			fprintf(stderr, "\t\tdrop table failed");
			return -1;
		}
		else{
			printf("\t\tDrop Table passed");
		}
		
	}
	
	return 0;
}

void new_database_tests(char * db_loc, int page_size, int buffer_size){
	
	populate_tests();
	
	printf("after pop tests\n");
	
	//create new database instance
	int rs = create_database(db_loc, page_size, buffer_size, false);
	
	if(rs != 0){
		fprintf(stderr, "Failed to create new database at %s\n", db_loc);
		exit(-1);
	}
		
	
	//create the 5 tables. All should be inserted successfully
	
	printf("Table Creation... will create %d tables.\n", num_tests);
	fflush(NULL);
	for(int i = 0; i < num_tests; i++){
		printf("\tCreating Table: %s\n", test_table_data[i].test_name);
		int table_num = add_table( test_table_data[i].data_types, test_table_data[i].key_indices, 
		                           test_table_data[i].data_types_size, test_table_data[i].key_indices_size );
		test_table_data[i].table_num = table_num;
								   
	    if(table_num < 0){
			fprintf(stderr, "Failed to create table: test_table_data[%d]\n", i);
			return;
		}
		else{
			printf("\t\tCreated test_data_table[%d] as table[%d]\n", i, table_num);
		}
	}
	
	tests();
}
	
int main(int argc, char ** argv){
	
	int page_size = strtol(argv[2], NULL, 10);
	int buffer_size = strtol(argv[3], NULL, 10);
	
	new_database_tests(argv[1], page_size, buffer_size);
	
	return 0;
}