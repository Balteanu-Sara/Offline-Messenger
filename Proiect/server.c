/* servTCPConcTh2.c - Exemplu de server TCP concurent care deserveste clientii
   prin crearea unui thread pentru fiecare client.
   Asteapta un numar de la clienti si intoarce clientilor numarul incrementat.
	Intoarce corect identificatorul din program al thread-ului.
  
   
   Autor: Lenuta Alboaie  <adria@info.uaic.ro> (c)
*/
#include "mydb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
/* portul folosit */
#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);
ssize_t write_wrapper(int socketdesc, const void *buffer, size_t count) {
    ssize_t total = 0;
    ssize_t bytes_written;

    while (total < count) {
        bytes_written = write(socketdesc, buffer + total, count - total);

        if (bytes_written >= 0) {
            total += bytes_written;
        } else {
            return bytes_written;
        }
    }

    return total;
}

int main ()
{
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;	
    char msg[101];		//mesajul primit de trimis la client 
    int sd;		//descriptorul de socket 
    int pid;
    pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	  int i=0;
  
    sqlite3* db = open_database("my_database.db"); //deschide database ul aplicatiei
    create_tables(db); //creeaza tabele
    close_database(db);

  /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }
  /* utilizarea optiunii SO_REUSEADDR */
    int on=1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 2) == -1)
    {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }
  /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        int client;
        thData * td; //parametru functia executata de thread     
        int length = sizeof (from);

        printf ("[server]Asteptam la portul %d...\n",PORT);
        fflush (stdout);

      // client= malloc(sizeof(int));
      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
        {
            perror ("[server]Eroare la accept().\n");
            continue;
        }
	
        /* s-a realizat conexiunea, se astepta mesajul */
    
	// int idThread; //id-ul threadului
	// int cl; //descriptorul intors de accept

	    td=(struct thData*)malloc(sizeof(struct thData));	
	    td->idThread=i++;
	    td->cl=client;

	    pthread_create(&th[i], NULL, &treat, td);

			
	}//while  
 
};
				
static void *treat(void * arg)
{		
	struct thData tdL; 
	tdL= *((struct thData*)arg);	
	printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
	fflush (stdout);		 
	pthread_detach(pthread_self());		
	raspunde((struct thData*)arg);
	  		
};


void raspunde(void *arg)
{
    int continua=1, in=0, in_convo=0, load=0;
    char current_user[35]="";
    char a_user[35];
    while(continua)
    {
        int nr;
        char msg[101];
        struct thData tdL; 
        tdL= *((struct thData*)arg);
        if (read (tdL.cl, msg, sizeof(msg)) <= 0)
        {
            printf("[Thread %d]\n",tdL.idThread);
            perror ("Eroare la read() de la client.\n");
                
        }
        
        printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, msg);
                
        /*pregatim mesajul de raspuns */
        if(strcmp("quit", msg)!=0)
        {
            if(strcmp("register", msg)==0 && in==0)
            {
              in_convo=0;
              char response[]="trebuie username";
              response[16]='\0';

              printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

              if (write (tdL.cl, response, sizeof(response)) <= 0)
              {
                  printf("[Thread %d] ",tdL.idThread);
                  perror ("[Thread]Eroare la write() catre client.\n");
              }

              char msg1[101];
              if (read (tdL.cl, msg1, sizeof(msg1)) <= 0)
              {
                  printf("[Thread %d]\n",tdL.idThread);
                  perror ("Eroare la read() de la client.\n");
                      
              }
              
              printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, msg1);

              sqlite3* db = open_database("my_database.db");
              int exists;
              exists= check_username(db, msg1);
              close_database(db);

              if(exists!=0)
              {
                  while(exists)
                  {
                      char warning[]="username-ul este deja existent";
                      warning[30]='\0'; 

                      printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, warning);

                      if (write (tdL.cl, warning, sizeof(warning)) <= 0)
                      {
                          printf("[Thread %d] ",tdL.idThread);
                          perror ("[Thread]Eroare la write() catre client.\n");
                      }

                      if (read (tdL.cl, msg1, sizeof(msg1)) <= 0)
                      {
                          printf("[Thread %d]\n",tdL.idThread);
                          perror ("Eroare la read() de la client.\n");
                              
                      }
                      
                      printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, msg1);

                      sqlite3* db = open_database("my_database.db");
                      exists = check_username(db, msg1);
                      close_database(db);
                  }
              }

              char response1[]="trebuie password";
              response1[16]='\0';

              printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response1);

              if (write (tdL.cl, response1, sizeof(response1)) <= 0)
              {
                  printf("[Thread %d] ",tdL.idThread);
                  perror ("[Thread]Eroare la write() catre client.\n");
              }

              char msg2[101];
              if (read (tdL.cl, msg2, sizeof(msg2)) <= 0)
              {
                  printf("[Thread %d]\n",tdL.idThread);
                  perror ("Eroare la read() de la client.\n");
                      
              }
              
              printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, msg2);	

              open_database("my_database.db");
              //printf("%s, %s\n", msg1, msg2);
              insert_into_users(db, msg1, msg2);
              close_database(db);

              char response2[]="Inregistrat!";
              response2[12]='\0';

              printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response2);

              if (write (tdL.cl, response2, sizeof(response2)) <= 0)
              {
                  printf("[Thread %d] ",tdL.idThread);
                  perror ("[Thread]Eroare la write() catre client.\n");
              }
            }

            if(strcmp("login", msg)==0)
            {
              in=1;
              in_convo=0;
              char response[]="trebuie username";
              response[16]='\0';

              printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

              if (write (tdL.cl, response, sizeof(response)) <= 0)
              {
                  printf("[Thread %d] ",tdL.idThread);
                  perror ("[Thread]Eroare la write() catre client.\n");
              }

              char msg1[101];
              if (read (tdL.cl, msg1, sizeof(msg1)) <= 0)
              {
                  printf("[Thread %d]\n",tdL.idThread);
                  perror ("Eroare la read() de la client.\n");
                      
              }

              strcpy(current_user, msg1);
              
              printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, msg1);	

              char response1[]="trebuie password";
              response1[16]='\0';

              printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response1);

              if (write (tdL.cl, response1, sizeof(response1)) <= 0)
              {
                  printf("[Thread %d] ",tdL.idThread);
                  perror ("[Thread]Eroare la write() catre client.\n");
              }
              
              char msg2[101];
              if (read (tdL.cl, msg2, sizeof(msg2)) <= 0)
              {
                  printf("[Thread %d]\n",tdL.idThread);
                  perror ("Eroare la read() de la client.\n");
                      
              }
              
              printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, msg2);	

              sqlite3* db = open_database("my_database.db");
              int exists = check_account(db, msg1, msg2);
              close_database(db);

              if(exists == 1)
              {
                  char response2[]="client logged in!";
                  response2[17]='\0';

                  printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response2);

                  if (write (tdL.cl, response2, sizeof(response2)) <= 0)
                  {
                      printf("[Thread %d] ",tdL.idThread);
                      perror ("[Thread]Eroare la write() catre client.\n");
                  }
              }
              else
              if(exists == 0)
              {
                  char response2[]="username sau parola incorecta!";
                  response2[30]='\0';

                  printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response2);

                  if (write (tdL.cl, response2, sizeof(response2)) <= 0)
                  {
                      printf("[Thread %d] ",tdL.idThread);
                      perror ("[Thread]Eroare la write() catre client.\n");
                  }
              }

            }

            if(strcmp("conversatii", msg)==0)
            {
              if(in==0)
              {
                char response[]="clientul nu e logged in!";
                response[24]='\0';

                printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                if (write (tdL.cl, response, sizeof(response)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }
              }

              if(in==1)
              {
                load=0;
                memset(a_user, 0, sizeof a_user);

                char conversations[256][256];
                int count;
                sqlite3* db = open_database("my_database.db");
                show_conversations(db, current_user,conversations, &count);
                close_database(db);

                int i;
                for(i=0; i<count ; i++)
                {
                  printf("%s\n", conversations[i]);
                }

                in_convo=0;

                printf("[Thread %d]Trimitem mesajul inapoi...%d\n",tdL.idThread, count);

                if (write (tdL.cl, &count, sizeof(int)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }

                ssize_t bytes_written = write_wrapper(tdL.cl, conversations, sizeof(conversations));

                if (bytes_written > 0) {
                    printf("Wrote %zd bytes to the socket.\n", bytes_written);
                } 
                else { printf("Error at bytes_written.\n");}
              }
            }

            if(strcmp("trimite_mesaj", msg)==0)
            {
              if(in==0)
              {
                char response[]="clientul nu e logged in!";
                response[24]='\0';

                printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                if (write (tdL.cl, response, sizeof(response)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }
              }

              if(in==1)
              {
                in_convo=0;
                load=0;
                memset(a_user, 0, sizeof a_user);
                char response[]="trebuie nume";
                response[12]='\0';

                printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                if (write (tdL.cl, response, sizeof(response)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }

                char msg1[101];
                if (read (tdL.cl, msg1, sizeof(msg1)) <= 0)
                {
                    printf("[Thread %d]\n",tdL.idThread);
                    perror ("Eroare la read() de la client.\n");
                        
                }
                
                printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, msg1);	

                char response1[]="mesajul fiind";
                response1[13]='\0';

                printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response1);

                if (write (tdL.cl, response1, sizeof(response1)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }

                char msg2[101];
                if (read (tdL.cl, msg2, sizeof(msg2)) <= 0)
                {
                    printf("[Thread %d]\n",tdL.idThread);
                    perror ("Eroare la read() de la client.\n");
                        
                }
                
                printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, msg2);	

                sqlite3* db = open_database("my_database.db");
                insert_into_messages(db, current_user, msg1, msg2);
                close_database(db);

                char response2[]="mesaj trimis!";
                response2[13]='\0';

                printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response2);

                if (write (tdL.cl, response2, sizeof(response2)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }
              }
            }

            if(strcmp("istoric_mesaje", msg)==0)
            {
              if(in==0)
              {
                char response[]="clientul nu e logged in!";
                response[24]='\0';

                printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                if (write (tdL.cl, response, sizeof(response)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }
              }

              if(in==1)
              {
                in_convo=1;
                load=0;
                memset(a_user, 0, sizeof a_user);
                char response[]="trebuie nume";
                response[12]='\0';

                printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                if (write (tdL.cl, response, sizeof(response)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }

                char receiver[101];
                if (read (tdL.cl, receiver, sizeof(receiver)) <= 0)
                {
                    printf("[Thread %d]\n",tdL.idThread);
                    perror ("Eroare la read() de la client.\n");
                        
                }
                
                printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, receiver);	

                char messages[256][256];
                memset(messages, 0, sizeof messages);
                load=5;
                strcpy(a_user, receiver);
                //printf('a_user: %s, receiver: %s', a_user, receiver ); ?????
                sqlite3* db1 = open_database("my_database.db");
                show_messages(db1, current_user, receiver, messages, load);
                close_database(db1);

                int i;
                for(i=0; i<load ; i++)
                {
                  printf("%s\n", messages[i]);
                }

                printf("[Thread %d]Trimitem mesajul inapoi...%d\n",tdL.idThread, load);

                if (write (tdL.cl, &load, sizeof(int)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }

                ssize_t bytes_written = write_wrapper(tdL.cl, messages, sizeof(messages));

                if (bytes_written > 0) {
                    printf("Wrote %zd bytes to the socket.\n", bytes_written);
                } 
                else { printf("Error at bytes_written.\n");}

              }
            }

            if(strcmp("load_more", msg)==0)
            {
              if(in==0)
              {
                char response[]="clientul nu e logged in!";
                response[24]='\0';

                printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                if (write (tdL.cl, response, sizeof(response)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }
              }

              if(in==1)
              {
                if(in_convo==0)
                {
                  char response[]="intra intr-o conversatie!";
                  response[25]='\0';

                  printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                  if (write (tdL.cl, response, sizeof(response)) <= 0)
                  {
                      printf("[Thread %d] ",tdL.idThread);
                      perror ("[Thread]Eroare la write() catre client.\n");
                  }
                }

                if(in_convo==1)
                {
                    char response[]="tot ok!";
                    response[7]='\0';

                    printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                    if (write (tdL.cl, response, sizeof(response)) <= 0)
                    {
                        printf("[Thread %d] ",tdL.idThread);
                        perror ("[Thread]Eroare la write() catre client.\n");
                    }

                    load=load+5;

                    char messages[256][256];
                    memset(messages, 0, sizeof messages);
                    sqlite3* db = open_database("my_database.db");
                    show_messages(db, current_user, a_user, messages, load);
                    close_database(db);

                    int i;
                    for(i=0; i<load ; i++)
                    {
                      printf("%s\n", messages[i]);
                    }

                    printf("[Thread %d]Trimitem mesajul inapoi...%d\n",tdL.idThread, load);

                    if (write (tdL.cl, &load, sizeof(int)) <= 0)
                    {
                        printf("[Thread %d] ",tdL.idThread);
                        perror ("[Thread]Eroare la write() catre client.\n");
                    }
           
                    ssize_t bytes_written = write_wrapper(tdL.cl, messages, sizeof(messages));

                    if (bytes_written > 0) {
                        printf("Wrote %zd bytes to the socket.\n", bytes_written);
                    } 
                    else { printf("Error at bytes_written.\n");}
                    }                
              }

            }

            if(strcmp("reply", msg)==0)
            {
              if(in==0)
              {
                char response[]="clientul nu e logged in!";
                response[24]='\0';

                printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                if (write (tdL.cl, response, sizeof(response)) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write() catre client.\n");
                }
              }

              if(in==1)
              {
                if(in_convo==0)
                {
                  char response[]="intra intr-o conversatie!";
                  response[25]='\0';

                  printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                  if (write (tdL.cl, response, sizeof(response)) <= 0)
                  {
                      printf("[Thread %d] ",tdL.idThread);
                      perror ("[Thread]Eroare la write() catre client.\n");
                  }
                }

                if(in_convo==1)
                {
                  char response[]="alege mesaj";
                  response[11]='\0';

                  printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

                  if (write (tdL.cl, response, sizeof(response)) <= 0)
                  {
                      printf("[Thread %d] ",tdL.idThread);
                      perror ("[Thread]Eroare la write() catre client.\n");
                  }

                  int id;
                  if (read (tdL.cl, &id, sizeof(int)) <= 0)
                  {
                      printf("[Thread %d]\n",tdL.idThread);
                      perror ("Eroare la read() de la client.\n");
                          
                  }
                  
                  printf ("[Thread %d]Mesajul a fost receptionat...%d\n",tdL.idThread, id);	

                  char response1[]="reply cu";
                  response1[8]='\0';

                  printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response1);

                  if (write (tdL.cl, response1, sizeof(response1)) <= 0)
                  {
                      printf("[Thread %d] ",tdL.idThread);
                      perror ("[Thread]Eroare la write() catre client.\n");
                  }

                  char msg2[101];
                  if (read (tdL.cl, msg2, sizeof(msg2)) <= 0)
                  {
                      printf("[Thread %d]\n",tdL.idThread);
                      perror ("Eroare la read() de la client.\n");
                          
                  }
                  
                  printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread, msg2);	

                  sqlite3* db=open_database("my_database.db");
                  insert_reply_messages(db, current_user, a_user, msg2, id);
                  close_database(db);

                  char response2[]="reply cu succes!";
                  response2[16]='\0';

                  printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response2);

                  if (write (tdL.cl, response2, sizeof(response2)) <= 0)
                  {
                      printf("[Thread %d] ",tdL.idThread);
                      perror ("[Thread]Eroare la write() catre client.\n");
                  }
                }
              }
            }

            if(strcmp("logout", msg)==0 && in==1)
            {
              in=0;
              load=0;
              memset(a_user, 0, sizeof a_user);

              char response[]="client logged out!";
              response[18]='\0';

              printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, response);

              if (write (tdL.cl, response, sizeof(response)) <= 0)
              {
                  printf("[Thread %d] ",tdL.idThread);
                  perror ("[Thread]Eroare la write() catre client.\n");
              }
            }

        }
        else
        {
            //in=0;
            nr=0;
            printf("[Thread %d]Trimitem mesajul inapoi...%d\n",tdL.idThread, nr);

            if (write (tdL.cl, &nr, sizeof(int)) <= 0)
            {
                printf("[Thread %d] ",tdL.idThread);
                perror ("[Thread]Eroare la write() catre client.\n");
            }
            else
                printf ("[Thread %d]Mesajul a fost trasmis cu succes.\n",tdL.idThread);	

            continua=0;

            close((int)arg);
            return(NULL);	
        }
    }

}