#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 
#include "mydb.h"


static int callback(void *data, int argc, char **argv, char **azColName) {
   int i;
   fprintf(stderr, "%s: ", (const char*)data); 
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

sqlite3* open_database(const char* dbname)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Callback function called";

    /* Open database */
    rc = sqlite3_open(dbname, &db);  /*dbname = "test.db"*/
    
   if( rc ) 
   {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stdout, "Opened database successfully\n");
      return db;
   }
}

void  create_tables(sqlite3* db)
{
    int rc;
    char *sql;
    char *zErrMsg = 0;
    const char* data = "Callback function called";

    sql = "CREATE TABLE IF NOT EXISTS USERS("  \
        "user_id INT PRIMARY KEY AUTOINCREMENT,"  \
        "username CHAR(30) NOT NULL,"  \
        "password CHAR(15) NOT NULL);";  

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
          fprintf(stdout, "Table USERS has been created.\n");
    }

    sql = "CREATE TABLE IF NOT EXISTS MESSAGES("  \
        "message_id INT PRIMARY KEY AUTOINCREMENT,"  \
        "sender_id INT,"  \
        "receiver_id INT,"  \
        "message_text TEXT NOT NULL,"  \
        "sent_time DATETIME NOT NULL,"  \
        "parent_id INT,"  \
        "FOREIGN KEY (sender_id) REFERENCES USERS(user_id),"  \
        "FOREIGN KEY (receiver_id) REFERENCES USERS(user_id),"  \
        "FOREIGN KEY (parent_id) REFERENCES MESSAGES(message_id) );";

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
          fprintf(stdout, "Table MESSAGES has been created.\n");
    }

    sql = "CREATE TABLE IF NOT EXISTS CONVERSATIONS("  \
        "convo_id INT PRIMARY KEY AUTOINCREMENT,"  \
        "user1_id INT,"  \
        "user2_id INT,"  \
        "last_message_id INT,"  \
        "activity_time DATETIME NOT NULL,"  \
        "FOREIGN KEY (user1_id) REFERENCES USERS(user_id),"  \
        "FOREIGN KEY (user2_id) REFERENCES USERS(user_id),"  \
        "FOREIGN KEY (last_message_id) REFERENCES MESSAGES(message_id) );";

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
          fprintf(stdout, "Table CONVERSATIONS has been created.\n");
    }
}

int check_username(sqlite3* db, char username[])
{
    sqlite3_stmt *stmt;
    int rc;
    char *sql;
    char *zErrMsg = 0;
    const char* data = "Callback function called";

    sql = "SELECT COUNT(*) from USERS WHERE username = ?;";  

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return -1;
    }

    int result= sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return result;
}

void insert_into_users(sqlite3* db)
{

}

void close_database(sqlite3* db) {
    sqlite3_close(db);
    fprintf(stdout, "The database has been closed.\n");
}

/*


   /* Create SQL statement using create
   sql = "CREATE TABLE COMPANY("  \
      "ID INT PRIMARY KEY     NOT NULL," \
      "NAME           TEXT    NOT NULL," \
      "AGE            INT     NOT NULL," \
      "ADDRESS        CHAR(50)," \
      "SALARY         REAL );"; */

    /* Create SQL statement using insert into
    sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
         "VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
         "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
         "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
         "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );"; */

    /* Create SQL statement using select 
   sql = "SELECT * from COMPANY"; */

   /* Create merged SQL statement using update and select
   sql = "UPDATE COMPANY set SALARY = 25000.00 where ID=1; " \
         "SELECT * from COMPANY"; */

    /* Create merged SQL statement using delete
   sql = "DELETE from COMPANY where ID=3 OR ID=4; " \
         "SELECT * from COMPANY";  */

    /* Delete SQL table using drop 
   sql = "DROP table COMPANY"; */

   /* Execute SQL statement 
   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Operation done successfully\n");
   }
   sqlite3_close(db); 
   return 0;
} */
