#ifndef DBMANAGEMENTSYSTEM_TUPLE_H
#define DBMANAGEMENTSYSTEM_TUPLE_H
struct Tuple{
    int size;
    char ** tuple;
}; typedef struct Tuple * Tuple;

void free_tuple(Tuple tuple);

Tuple create_tuple(char * tuple_string);
#endif //DBMANAGEMENTSYSTEM_TUPLE_H
