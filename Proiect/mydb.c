#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h> 
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
        "user_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"  \
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
        "message_id INTEGER PRIMARY KEY AUTOINCREMENT,"  \
        "sender_id INTEGER,"  \
        "receiver_id INTEGER,"  \
        "message_text TEXT NOT NULL,"  \
        "sent_time DATETIME,"  \
        "parent_id INTEGER,"  \
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
        "convo_id INTEGER PRIMARY KEY AUTOINCREMENT,"  \
        "user1_id INTEGER,"  \
        "user2_id INTEGER,"  \
        "last_message_id INTEGER,"  \
        "activity_time DATETIME,"  \
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

    sql = "SELECT COUNT(*) from USERS WHERE username = ?;";  

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    rc= sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind username: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    else fprintf(stdout, "Username checked.\n");

    int result= sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return result;
}

int check_account(sqlite3* db, char user[], char pass[])
{
    sqlite3_stmt *stmt;
    int rc;
    char *sql;

    sql = "SELECT COUNT(*) from USERS WHERE username = ? AND password = ?;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    rc = sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 1: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }
    rc = sqlite3_bind_text(stmt, 2, pass, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);   
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    else fprintf(stdout, "Account checked.\n");

    int result= sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return result;
}

void insert_into_users(sqlite3* db, char user[], char pass[])
{
    sqlite3_stmt* stmt;
    int rc;
    char *sql;

    sql = "INSERT INTO USERS(username, password) VALUES(?, ?);";  

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    rc = sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 1: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }
    rc = sqlite3_bind_text(stmt, 2, pass, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
    }
    else fprintf(stdout, "Data inserted!.\n");

    sqlite3_finalize(stmt);
}


void insert_into_messages(sqlite3* db, char sender[], char receiver[], char txt[])
{
    sqlite3_stmt* stmt;
    int rc;
    char *sql;

    sql = "SELECT user_id FROM USERS WHERE username = ?;";
    
    //luam id-ul senderului
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } 

    rc = sqlite3_bind_text(stmt, 1, sender, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    else fprintf(stdout, "Got sender's id.\n");

    int senderid = sqlite3_column_int(stmt, 0);
    fprintf(stderr, "Sender's id: %d\n", senderid);


    //luam id-ul receiverului
    rc = sqlite3_reset(stmt);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error resetting statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } 

    rc = sqlite3_bind_text(stmt, 1, receiver, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    else fprintf(stdout, "Got receiver's id.\n");

    int receiverid = sqlite3_column_int(stmt, 0);
    fprintf(stderr, "Receiver's id: %d\n", receiverid);


    //introducem datele in tabela messages
    rc = sqlite3_reset(stmt);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error resetting statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    sql = "INSERT INTO MESSAGES(sender_id, receiver_id, message_text, sent_time, parent_id) "
        "VALUES(?, ?, ?, CURRENT_TIMESTAMP, NULL);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    rc = sqlite3_bind_int(stmt, 1, senderid);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 1: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }
    rc = sqlite3_bind_int(stmt, 2, receiverid);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);   
    }
    rc = sqlite3_bind_text(stmt, 3, txt, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 3: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);   
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error executing queryy: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    else fprintf(stdout, "Message inserted.\n");


    //verficam daca exista conversatia intre cei doi useri
    rc = sqlite3_reset(stmt);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error resetting statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    sql="SELECT COUNT(convo_id) FROM CONVERSATIONS WHERE user1_id= ? and user2_id= ? ;";

    int user1id;
    if(senderid<receiverid) user1id=senderid;
        else user1id=receiverid;
    int user2id;

    if(senderid>receiverid) user2id=senderid;
        else user2id=receiverid;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    rc = sqlite3_bind_int(stmt, 1, user1id);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 1: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }
    rc = sqlite3_bind_int(stmt, 2, user2id);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);   
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    else fprintf(stdout, "Verificaton of conversation's existence completed.\n");

    int result = sqlite3_column_int(stmt,0);
    fprintf(stdout, "The existence is %d.\n", result);

    //avem nevoie de message_id si sent_time
    rc = sqlite3_reset(stmt);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error resetting statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    sql="SELECT message_id, sent_time FROM MESSAGES WHERE sender_id= ? and receiver_id= ? ORDER BY sent_time DESC LIMIT 1;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    rc = sqlite3_bind_int(stmt, 1, senderid);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 1: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }
    rc = sqlite3_bind_int(stmt, 2, receiverid);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);   
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    else fprintf(stdout, "We got the message_id and the sent_time.\n");

    int messageid=sqlite3_column_int(stmt, 0);
    const char* senttime=sqlite3_column_text(stmt,1);
    fprintf(stdout, "The message_id is %d, and the sent_time is %s.\n", messageid, senttime);


    if(result==0)
    {
        //introducem toate elementele in tabela conversations
        rc = sqlite3_reset(stmt);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Error resetting statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
        }

        sql="INSERT INTO CONVERSATIONS(user1_id, user2_id, last_message_id, activity_time) VALUES(?, ?, ?, NULL);";

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
        }

        rc = sqlite3_bind_int(stmt, 1, user1id);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 1: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
        }
        rc = sqlite3_bind_int(stmt, 2, user2id);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }
        rc = sqlite3_bind_int(stmt, 3, messageid);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 3: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }
        /*rc = sqlite3_bind_text(stmt, 4, senttime, -1, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 3: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }*/

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
        }
        else fprintf(stdout, "We inserted everything beside activity_time in the CONVERSATION table.\n");

        rc = sqlite3_reset(stmt);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Error resetting statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
        }

        sql="UPDATE CONVERSATIONS SET activity_time = (SELECT sent_time FROM MESSAGES WHERE message_id = ?) WHERE user1_id= ? AND user2_id= ?;" ;

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
        }

        rc = sqlite3_bind_int(stmt, 1, messageid);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 3: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }

        rc = sqlite3_bind_int(stmt, 2, user1id);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }
        rc = sqlite3_bind_int(stmt, 3, user2id);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 3: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
        }
        else fprintf(stdout, "NOW We inserted everything in the CONVERSATION table.\n");
    }
    else
    if(result==1)
    {
        //acutalizam last_message_id din tabela conversations
        rc = sqlite3_reset(stmt);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Error resetting statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
        }

        sql="UPDATE CONVERSATIONS SET last_message_id= ? WHERE user1_id= ? AND user2_id= ?;";

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
        }

        rc = sqlite3_bind_int(stmt, 1, messageid);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 1: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
        }
        rc = sqlite3_bind_int(stmt, 2, user1id);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }
        rc = sqlite3_bind_int(stmt, 3, user2id);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 3: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
        }
        else fprintf(stdout, "We inserted last_message_id in the CONVERSATION table.\n");

        //acutalizam activity_time din tabela conversations
        rc = sqlite3_reset(stmt);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Error resetting statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
        }

        //sql="UPDATE CONVERSATIONS SET activity_time= ? WHERE user1_id= ? AND user2_id= ?;";

        sql="UPDATE CONVERSATIONS SET activity_time = (SELECT sent_time FROM MESSAGES WHERE message_id = ?) WHERE user1_id= ? AND user2_id= ?;";

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
        }

        /*rc = sqlite3_bind_text(stmt, 1, senttime, -1, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 1: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
        }
        rc = sqlite3_bind_int(stmt, 2, user1id);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        } */
        rc = sqlite3_bind_int(stmt, 1, messageid); //user2id
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 3: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }
        rc = sqlite3_bind_int(stmt, 2, user1id);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }
        rc = sqlite3_bind_int(stmt, 3, user2id);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);   
        }  

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_close(db);
        }
        else fprintf(stdout, "We inserted activity_time in the CONVERSATION table.\n");
    
    }

    sqlite3_finalize(stmt);

}

//void show_conversations(sqlite3* db, char username[], char *conversations[], int& count1)
void show_conversations(sqlite3* db, char username[], char conversations[256][256], int *count1)
{
    sqlite3_stmt* stmt;
    int rc;
    char *sql;

    sql = "SELECT user_id FROM USERS WHERE username = ?;";
    
    //luam id-ul clientului curent
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    } 

    rc = sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    else fprintf(stdout, "Got current user's id.\n");

    int userid = sqlite3_column_int(stmt, 0);
    fprintf(stderr, "Current user's id: %d\n", userid);

    rc = sqlite3_reset(stmt);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error resetting statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    sql="SELECT user1_id, user2_id FROM CONVERSATIONS WHERE user1_id= ? OR user2_id= ? ORDER BY activity_time DESC;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    rc = sqlite3_bind_int(stmt, 1, userid);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 1: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
    }
    rc = sqlite3_bind_int(stmt, 2, userid);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot bind parameter 2: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);   
    }

    int index=0;

    sqlite3_stmt* stmt1;
    int rc1, again=0;
    char *sql1;

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int user1_id = sqlite3_column_int(stmt, 0);
        int user2_id = sqlite3_column_int(stmt, 1);

        fprintf(stderr, "From the array, user1's id: %d\n", user1_id);
        fprintf(stderr, "From the array, user2's id: %d\n", user2_id);

        int other_userid;

        if(user1_id==userid)
            other_userid=user2_id;
        else if(user2_id==userid)
                other_userid=user1_id;

        fprintf(stderr, "From the array, the other user's id: %d\n", other_userid);

        if(again==1)
        {
            rc1 = sqlite3_reset(stmt1);
            if(rc1 != SQLITE_OK) {
                fprintf(stderr, "Error resetting statement: %s\n", sqlite3_errmsg(db));
                sqlite3_finalize(stmt1);
                sqlite3_close(db);
            }
        }

        //obtinem usernameul la other_userid
        sql1= "SELECT username FROM USERS WHERE user_id = ?;";

        rc1 = sqlite3_prepare_v2(db, sql1, -1, &stmt1, NULL);
        if (rc1 != SQLITE_OK) {
            fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
        }

        rc1 = sqlite3_bind_int(stmt1, 1, other_userid);
        if (rc1 != SQLITE_OK) {
            fprintf(stderr, "Cannot bind parameter 1: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt1);
        }

        rc1 = sqlite3_step(stmt1);
        if (rc1 != SQLITE_ROW) {
            fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt1);
            sqlite3_close(db);
        }
        else fprintf(stdout, "We have other_userid's username .\n");

        const char* other_username=sqlite3_column_text(stmt1,0);
        fprintf(stderr, "Other_userid's username: %s\n", other_username);

        strcpy(conversations[index], other_username);
        index++;

        again=1;
    }
    sqlite3_finalize(stmt1);

    *count1=index;

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        exit(1);
    }

    sqlite3_finalize(stmt);
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
