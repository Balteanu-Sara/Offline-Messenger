#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

sqlite3* open_database( const char* dbname);

void close_database(sqlite3* db);

void create_tables(sqlite3* db);

//functii pentru comenzile register si login
int check_username(sqlite3* db, char s[]);
int check_account(sqlite3* db, char username[], char password[]);
void insert_into_users(sqlite3* db, char username[], char password[]);

//pentru trimite_mesaj 
void insert_into_messages(sqlite3* db, char username1[], char username2[], char text[]);

//pentru conversatii
void show_conversations(sqlite3* db, char username[], char conversations[256][256], int *count1);

//pentru istoric_mesaje
void show_messages(sqlite3* db, char username1[], char username2[], char messages[256][256], int count);


