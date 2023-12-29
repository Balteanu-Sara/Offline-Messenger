#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

sqlite3* open_database( const char* dbname);

void close_database(sqlite3* db);

void create_tables(sqlite3* db);