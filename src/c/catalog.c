#include "../headers/catalog.h"
#include <stdlib.h>
#include <string.h>

int initialize_catalog(char *db_loc, bool restart) {
    set_up_db_location(db_loc);

    if (restart) {
        read_catalog_from_disk(); // this has yet to be defined
    };

    return 0;
}

int set_up_db_location(char *db_loc) {
    GLOBAL_DB_LOCATION = malloc(strlen(db_loc) + 1);
    strcpy(GLOBAL_DB_LOCATION, db_loc);
    return 0;
}

int shutdown_catalog() {
    int result = write_catalog_to_disk();
    free(GLOBAL_DB_LOCATION);
    return result;
}

int createCatalog(Table table) {

    catalog = malloc(sizeof(struct Catalog));

    // allocate memory for struct attributes
    catalog->tables = malloc(sizeof(struct Table *));

    // add initial table
    catalog->tables[0] = table;
    catalog->table_count = 1;
    return 0;
}

void freeCatalog() {
    if (catalog != NULL) {
        if (catalog->tables != NULL) {
            for (int i = 0; i < catalog->table_count; i++) {
                if (catalog->tables[i] != NULL) {
                    freeTable(catalog->tables[i]);
                }
            }
            free(catalog->tables);
        }
        free(catalog);
    }
}

int write_catalog_to_disk() {
    if (catalog != NULL) {
        char *catalog_path = get_catalog_file_path();

        // catalog data
        int table_count = catalog->table_count;
        struct Table **tables = catalog->tables;

        // open file and write the catalog data
        FILE *catalog_file = fopen(catalog_path, "wb");

        // write table count
        fwrite(&table_count, sizeof(int), 1, catalog_file);

        // write each table struct and all its data
        for (int i = 0; i < catalog->table_count; i++) {
            write_table_to_disk(catalog_file, tables[i]);
        }

        fclose(catalog_file);
        free(catalog_path);
        freeCatalog();
    }
    return 0;
}

int read_catalog_from_disk() {
    char *catalog_path = get_catalog_file_path();

    // open file and read the catalog data
    FILE *catalog_file = fopen(catalog_path, "rb");

    // read table count
    int table_count;
    fread(&table_count, sizeof(int), 1, catalog_file);

    struct Table *current_table;

    // read each table struct and all its data
    for (int i = 0; i < table_count; i++) {
        current_table = read_table_from_disk(catalog_file);
        add_table_to_catalog(current_table);
    }

    fclose(catalog_file);
    free(catalog_path);
    return 0;
}

void display_catalog() {
    printf("\n******************Display the entire catalog bro***************************\n");

    // catalog data
    printf("\n\ntable count: %d\n", catalog->table_count);
    struct Table **tables = catalog->tables;
    struct Table *table;
    struct Attribute *attribute;

    // write each table struct and all its data
    for (int i = 0; i < catalog->table_count; i++) {
        table = tables[i];
        printf("\ntableId: %d\n", table->tableId);
        printf("name: %s\n", table->name);
        printf("name_size: %d\n", table->name_size);

        printf("attributes:\n");
        for (int j = 0; j < table->attribute_count; j++) {
            attribute = table->attributes[j];
            printf("    name: %s\n", attribute->name);
            printf("    name_size: %d\n", attribute->name_size);
            printf("    size: %d\n", attribute->size);
            printf("    default_size: %d\n", attribute->default_size);
            printf("    default: %s\n", attribute->default_value);
            printf("    contraints:\n");
            printf("        notnull: %d\n", attribute->constraints->notnull);
            printf("        primary_key: %d\n", attribute->constraints->primary_key);
            printf("        unique: %d\n", attribute->constraints->unique);
            if (attribute->foreignKey != NULL) {
                printf("    foreignKey:\n");
                printf("        column_name: %s\n", attribute->foreignKey->referenced_column_name);
                printf("        column_name_size: %d\n", attribute->foreignKey->referenced_column_name_size);
                printf("        table_name: %s\n", attribute->foreignKey->referenced_table_name);
                printf("        table_name_size: %d\n", attribute->foreignKey->referenced_table_name_size);
            }
        }

        printf("Data types:\n   (");
        for (int j = 0; j < table->data_type_size; j++) {
            printf("%d, ", table->data_types[j]);
        }
        printf(")\n");

        if (table->primary_key != NULL) {
            printf("primary_key:\n");
            printf(" size: %d\n", table->primary_key->size);

            for (int j = 0; j < table->primary_key->size; j++) {
                attribute = table->primary_key->attributes[j];
                printf("name: %s\n", attribute->name);
                printf("name_size: %d\n", attribute->name_size);
                printf("size: %d\n", attribute->size);
                printf("contraints:\n");
                printf("    notnull: %d\n", attribute->constraints->notnull);
                printf("    primary_key: %d\n", attribute->constraints->primary_key);
                printf("    unique: %d\n", attribute->constraints->unique);
                if (attribute->foreignKey != NULL) {
                    printf("foreignKey:\n");
                    printf("    column_name: %s\n", attribute->foreignKey->referenced_column_name);
                    printf("    column_name_size: %d\n", attribute->foreignKey->referenced_column_name_size);
                    printf("    table_name: %s\n", attribute->foreignKey->referenced_table_name);
                    printf("    table_name_size: %d\n", attribute->foreignKey->referenced_table_name_size);
                }
            }
        }

        printf("primary_key_count: %d\n", table->primary_key_count);
        printf("attribute_count: %d\n", table->attribute_count);
        printf("key_indices_count: %d\n", table->key_indices_count);
        printf("data_type_size: %d\n", table->data_type_size);
    }
}

char *get_catalog_file_path() {
    char *catalog_file_name = "/catalog";
    char *catalog_path;

    // format the catalog file name and path
    size_t database_path_length = strlen(GLOBAL_DB_LOCATION);
    char last_character = GLOBAL_DB_LOCATION[database_path_length - 1];
    if (last_character == '\'' || last_character == '/') {
        GLOBAL_DB_LOCATION[database_path_length - 1] = 0;
    }

    catalog_path = malloc(strlen(GLOBAL_DB_LOCATION) + strlen(catalog_file_name) + 1);

    strcpy(catalog_path, GLOBAL_DB_LOCATION);
    strcat(catalog_path, catalog_file_name);

    return catalog_path;

}

int add_table_to_catalog(Table table) {

    if (catalog == NULL) {
        return createCatalog(table);
    }

    catalog->tables = realloc(catalog->tables, sizeof(struct Table *) * (catalog->table_count + 1));
    catalog->tables[catalog->table_count] = table;
    catalog->table_count++;

    return 0;
}

int remove_table_from_catalog(char *table_name) {

    int loc = -1;
    for (int i = 0; i < catalog->table_count; i++) {
        if (strcmp(catalog->tables[i]->name, table_name) == 0) {
            // Nullify position in list of tables
            catalog->tables[i] = NULL;
            // Keep track of where the position was
            loc = i;
        }
    }

    // If table was found in list
    if (loc >= 0) {
        for (int i = loc; i < catalog->table_count - 1; i++) {
            // Move everything over one to fill in space
            catalog->tables[i] = catalog->tables[i + 1];
        }

        // iterate through tables
        for (int i = 0; i < catalog->table_count; i++) {

            if (catalog->tables[i] != NULL) {
                // iterate though all attributes
                for (int j = 0; j < catalog->tables[i]->attribute_count; j++) {
                    if (catalog->tables[i]->attributes[j]->foreignKey != NULL) {
                        // free foreign key
                        if (strcasecmp(catalog->tables[i]->attributes[j]->foreignKey->referenced_table_name,
                                       table_name) == 0) {
                            free(catalog->tables[i]->attributes[j]->foreignKey->referenced_table_name);
                            free(catalog->tables[i]->attributes[j]->foreignKey->referenced_column_name);
                            free(catalog->tables[i]->attributes[j]->foreignKey);
                            catalog->tables[i]->attributes[j]->foreignKey = NULL;
                        }
                    }
                }
            }
        }
        catalog->table_count--;
        // Success
        return 0;
    } else {
        // Table was not found in list
        return -1;
    }
}

Table get_table_from_catalog(char *table_name) {
    if (catalog != NULL) {
        for (int i = 0; i < catalog->table_count; i++) {
            if (strcasecmp(catalog->tables[i]->name, table_name) == 0) {
                return catalog->tables[i];
            }
        }
    }
    return NULL;
}

int drop_attribute_from_table(Table table_to_alter, Attribute attribute_to_drop) {

    // Search for references to the attribute in foreign keys of all other tables
    for (int j = 0; j < catalog->table_count; j++) {
        // Go through attributes and check if they have a foreign key
        for (int k = 0; k < catalog->tables[j]->attribute_count; k++) {
            Attribute checking = catalog->tables[j]->attributes[k];
            if (checking->foreignKey != NULL
                &&
                strcasecmp(table_to_alter->name, checking->foreignKey->referenced_table_name) ==
                0
                &&
                strcasecmp(attribute_to_drop->name, checking->foreignKey->referenced_column_name) == 0) {
                // This attribute referenced the table and attribute we were given,
                // so need to drop that foreign key as we're dropping the attribute
                checking->foreignKey = NULL;
//              printf("Removed foreign key from attribute %s in table %s\n", checking->name, catalog->tables[j]->name);
            }
        }
    }
    freeAttribute(attribute_to_drop);
    return 0;
}

int table_name_is_unique(char *table_name) {
    if (catalog != NULL) {
        for (int i = 0; i < catalog->table_count; i++) {
            if (strcasecmp(catalog->tables[i]->name, table_name) == 0) {
                return -1;
            }
        }
    }
    return 0;
}