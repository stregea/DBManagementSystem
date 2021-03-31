#include "../headers/table.h"
#include <string.h>
#include <stdlib.h>

struct Table *createTable(char *name) {
    struct Table *table_data = malloc(sizeof(struct Table));

    // set counts and allocate memory
    table_data->attribute_count = 0;
    table_data->primary_key_count = 0;
    table_data->key_indices_count = 0;
    table_data->data_type_size = 0;
    table_data->attributes = malloc(sizeof(struct Attribute));
    table_data->data_types = malloc(sizeof(int));
    table_data->key_indices = NULL;
    table_data->primary_key = NULL;
    table_data->name = malloc(strlen(name) + 1);
    table_data->name_size = strlen(name) + 1;
    strcpy(table_data->name, name);

    return table_data;
}

void freeKey(PrimaryKey key) {
    if (key != NULL) {
        if (key->attributes != NULL) {
            // there is no need to handle the freeing
            // of the attributes here since they are removed in
            // freeTable, thus we only need to free this pointer:
            free(key->attributes);
        }
        free(key);
    }
}

void freeAttribute(Attribute attr) {
    if (attr != NULL) {
        if (attr->name != NULL) {
            free(attr->name);
        }

        if (attr->constraints != NULL) {
            free(attr->constraints);
        }

        if (attr->foreignKey != NULL) {
            if (attr->foreignKey->referenced_table_name != NULL) {
                free(attr->foreignKey->referenced_table_name);
            }
            if (attr->foreignKey->referenced_column_name != NULL) {
                free(attr->foreignKey->referenced_column_name);
            }
            free(attr->foreignKey);
        }

        if (attr->default_value != NULL) {
            free(attr->default_value);
        }

        free(attr);
    }
}

void freeTable(Table table) {
    if (table != NULL) {
        if (table->name != NULL) {
            free(table->name);
        }
        if (table->data_types != NULL) {
            free(table->data_types);
        }

        // free primary key
        if (table->primary_key != NULL) {
            freeKey(table->primary_key);
        }

        // free attributes
        if (table->attributes != NULL) {
            for (int i = 0; i < table->attribute_count; i++) {
                if (table->attributes[i] != NULL) {
                    freeAttribute(table->attributes[i]);
                }
            }
            free(table->attributes);
        }


        // free key_indices
        if (table->key_indices) {
            free(table->key_indices);
        }

        free(table);
    }
}

int get_attribute_type(char *type) {

    if (strcasecmp(type, "integer") == 0) {
        return 0;
    } else if (strcasecmp(type, "double") == 0) {
        return 1;
    } else if (strcasecmp(type, "bool") == 0) {
        return 2;

    } else if (strcasecmp(type, "char") == 0) {
        return 3;

    } else if (strcasecmp(type, "varchar") == 0) {
        return 4;
    }
    return -1;
}

struct PrimaryKey *create_key(char *attribute_names, Table table) {
//    printf("parsing primary key\n");
    struct PrimaryKey *key = malloc(sizeof(struct PrimaryKey));
    key->attributes = NULL;
    key->size = 0;

    // split the attributes on space
    char *tokenizer = strtok(attribute_names, " ");
    struct Attribute **attributes = table->attributes;

    while (tokenizer != NULL) {
        for (int i = 0; i < table->attribute_count; i++) {

            if (strcasecmp(tokenizer, attributes[i]->name) == 0) {
                key->attributes = realloc(key->attributes, sizeof(struct Attribute *) * (key->size + 1));
                key->attributes[key->size] = attributes[i];
                key->size++;
            }
        }
        tokenizer = strtok(NULL, " ");
    }
//    printf("returning primary key with size: %d\n", key->size);
    table->key_indices_count = key->size;
    table->key_indices = malloc(sizeof(int) * key->size);
    for (int i = 0; i < key->size; i++) {
        table->key_indices[i] = key->attributes[i]->type;
    }
    return key;
}

PrimaryKey create_key_from_attr(Attribute attr, Table table) {
//    printf("parsing primary key in line\n");
    PrimaryKey key = malloc(sizeof(struct PrimaryKey));
    key->attributes = malloc(sizeof(struct Attribute *));
    key->attributes[0] = attr;
    key->size = 1;
    table->key_indices_count = key->size;
    table->key_indices = malloc(sizeof(int));
    table->key_indices[0] = attr->type;

    return key;
}

struct PrimaryKey *create_key_from_disk(char *attribute_names, Table table, int key_size) {
    struct PrimaryKey *key = malloc(sizeof(struct PrimaryKey));
    key->attributes = malloc(sizeof(struct Attribute *) * key_size);
    key->size = key_size;
    int key_attribute_index = 0;

    // split the attributes on space
    char *tokenizer = strtok(attribute_names, " ");
    struct Attribute **attributes = table->attributes;

    while (tokenizer != NULL) {
        for (int i = 0; i < table->attribute_count; i++) {
            // check if token equal attribute name
            if (strcasecmp(tokenizer, attributes[i]->name) == 0) {
                key->attributes[key_attribute_index] = attributes[i];
                key_attribute_index++;
            }
        }
        tokenizer = strtok(NULL, " ");
    }

    table->key_indices_count = key->size;
    table->key_indices = malloc(sizeof(int) * key->size);
    for (int i = 0; i < key->size; i++) {

        table->key_indices[i] = key->attributes[i]->type;
    }
    return key;
}

int add_primary_key_to_table(Table table, PrimaryKey key) {
    if (table->primary_key_count == 0) {
        table->primary_key = key;
        table->primary_key_count++;
        return 0;
    }
    freeKey(key);
    return -1; // error since primary key already exists in table.
}

int write_table_to_disk(FILE *file, struct Table *table) {
    // write id
    fwrite(&table->tableId, sizeof(int), 1, file);

    // write name
    fwrite(&table->name_size, sizeof(int), 1, file);
    fwrite(table->name, table->name_size, 1, file);

    // write array sizes
    fwrite(&table->primary_key_count, sizeof(int), 1, file);
    fwrite(&table->attribute_count, sizeof(int), 1, file);
    fwrite(&table->key_indices_count, sizeof(int), 1, file);
    fwrite(&table->data_type_size, sizeof(int), 1, file);

    // write each attribute
    for (int i = 0; i < table->attribute_count; i++) {
        write_attribute_to_disk(file, table->attributes[i]);
    }

    // write data types array
    fwrite(table->data_types, sizeof(int), table->data_type_size, file);

    // write primary key
    write_primary_key_to_disk(file, table->primary_key);

    return 0;
}

int write_attribute_to_disk(FILE *file, struct Attribute *attribute) {
    int null_value = 0;

    fwrite(&attribute->name_size, sizeof(int), 1, file);
    fwrite(attribute->name, attribute->name_size, 1, file);

    // read size of arrays
    fwrite(&attribute->type, sizeof(int), 1, file);
    fwrite(&attribute->size, sizeof(int), 1, file);
    fwrite(&attribute->default_size, sizeof(int), 1, file);
    fwrite(attribute->default_value, attribute->default_size, 1, file);

    // write constaints
    fwrite(attribute->constraints, sizeof(struct Constraints), 1, file);

    write_foreign_key_to_disk(file, attribute->foreignKey);

    return 0;
}

int write_primary_key_to_disk(FILE *file, struct PrimaryKey *primaryKey) {
    int null_value = 0;

    // might not be needed if size 0 and null are treated the same
    if (primaryKey == NULL) {
        fwrite(&null_value, sizeof(int), 1, file);
    } else {
        fwrite(&primaryKey->size, sizeof(int), 1, file);

        if(primaryKey->size > 0) {
            // write each attribute name to a string
            char* attributes = malloc(sizeof(char));
            // null terminator
            size_t attributes_string_size = 1;
            struct Attribute *current_attribute;

            // append the name of each attribute to the string
            for (int i = 0; i < primaryKey->size; i++) {
                current_attribute = primaryKey->attributes[i];
                attributes_string_size += current_attribute->name_size;
                attributes = realloc(attributes, attributes_string_size);

                if(i == 0){
                    strcpy(attributes, current_attribute->name);
                }
                else{
                    strcat(attributes, current_attribute->name);
                }

                strcat(attributes, " ");
            }

            // write contents to disk
            fwrite(&attributes_string_size, sizeof(int), 1, file);
            fwrite(attributes, attributes_string_size, 1, file);

            free(attributes);
        }

    }
    return 0;
}

int write_foreign_key_to_disk(FILE *file, struct ForeignKey *foreignKey) {
    int null_value = 0;
    if (foreignKey == NULL) {
        fwrite(&null_value, sizeof(int), 1, file);
    } else {
        // write the corresponding table and column name
        fwrite(&foreignKey->referenced_table_name_size, sizeof(int), 1, file);
        fwrite(&foreignKey->referenced_column_name_size, sizeof(int), 1, file);
        fwrite(foreignKey->referenced_table_name, foreignKey->referenced_table_name_size, 1, file);
        fwrite(foreignKey->referenced_column_name, foreignKey->referenced_column_name_size, 1, file);
    }

    return 0;
}

struct ForeignKey *read_foreign_key_from_disk(FILE *file) {
    struct ForeignKey *foreignKey;

    // read in the corresponding table and column name
    int first_value;
    fread(&first_value, sizeof(int), 1, file);

    // check if the foreign key is actually null
    if (first_value == 0) {
        foreignKey = NULL;
    } else {
        foreignKey = malloc(sizeof(struct ForeignKey));
        foreignKey->referenced_table_name_size = first_value;
        fread(&foreignKey->referenced_column_name_size, sizeof(int), 1, file);
        foreignKey->referenced_table_name = malloc(foreignKey->referenced_table_name_size);
        fread(foreignKey->referenced_table_name, sizeof(foreignKey->referenced_table_name), 1, file);
        foreignKey->referenced_column_name = malloc(foreignKey->referenced_column_name_size);
        fread(foreignKey->referenced_column_name, sizeof(foreignKey->referenced_column_name), 1, file);
    }

    return foreignKey;
}

struct Attribute *read_attribute_from_disk(FILE *file) {

    struct Attribute *attribute = malloc(sizeof(struct Attribute));

    fread(&attribute->name_size, sizeof(int), 1, file);
    attribute->name = malloc(attribute->name_size);
    fread(attribute->name, attribute->name_size, 1, file);

    // read size of arrays
    fread(&attribute->type, sizeof(int), 1, file);
    fread(&attribute->size, sizeof(int), 1, file);

    fread(&attribute->default_size, sizeof(int), 1, file);

    if(attribute->default_size == 0) {
        attribute->default_value = NULL;
    }
    else {
        attribute->default_value = malloc(attribute->default_size);
        fread(attribute->default_value, attribute->default_size, 1, file);
    }

    // write constraints
    attribute->constraints = malloc(sizeof(struct Constraints));
    fread(attribute->constraints, sizeof(struct Constraints), 1, file);

    // write foreign key
    attribute->foreignKey = read_foreign_key_from_disk(file);

    return attribute;
}

struct PrimaryKey *read_primary_key_from_disk(FILE *file, struct Table *table) {
    int key_size;
    struct PrimaryKey *primaryKey;
    int attributes_string_size;
    char* names;
    fread(&key_size, sizeof(int), 1, file);

    if (key_size == 0) {
        primaryKey = NULL;
    } else {

        fread(&attributes_string_size, sizeof(int), 1, file);

        if(attributes_string_size == 0){
            return NULL;
        }

        names = malloc(attributes_string_size);

        fread(names, attributes_string_size, 1, file);

        primaryKey = create_key_from_disk(names, table, key_size);
        free(names);
    }

    return primaryKey;
}

struct Table *read_table_from_disk(FILE *file) {
    int tableID;
    int name_size;

    fread(&tableID, sizeof(int), 1, file);
    fread(&name_size, sizeof(int), 1, file);
    char *name = malloc(name_size);
    fread(name, name_size, 1, file);

    // only adds name and name size to struct
    struct Table *table = createTable(name);
    table->tableId = tableID;
    free(name);

    // write array sizes
    fread(&table->primary_key_count, sizeof(int), 1, file);
    fread(&table->attribute_count, sizeof(int), 1, file);
    fread(&table->key_indices_count, sizeof(int), 1, file);
    fread(&table->data_type_size, sizeof(int), 1, file);

    // read each attribute
    for (int i = 0; i < table->attribute_count; i++) {
        table->attributes[i] = read_attribute_from_disk(file);
    }

    // write data types array
    table->data_types = realloc(table->data_types, sizeof(int) * table->data_type_size);
    fread(table->data_types, sizeof(int), table->data_type_size, file);

    // read primary key
    table->primary_key = read_primary_key_from_disk(file, table);

    return table;
}