
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

ssize_t read_wrapper(int socketdesc, void *buffer, size_t count) {
    ssize_t total = 0;
    ssize_t bytes_read;

    while (total < count) {
        bytes_read = read(socketdesc, buffer + total, count - total);

        if (bytes_read > 0) {
            total += bytes_read;
        } else if (bytes_read == 0) {
            break;
        } else {
            return bytes_read;
        }
    }
    return total;
}

int main (int argc, char *argv[])
{
    int sd;			// descriptorul de socket
    struct sockaddr_in server;	// structura folosita pentru conectare 
  		// mesajul trimis
    char buf[101];

  /* exista toate argumentele in linia de comanda? */
    if (argc != 3)
    {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

  /* stabilim portul */
     port = atoi (argv[2]);

  /* cream socketul */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("Eroare la socket().\n");
        return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
    server.sin_family = AF_INET;
  /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
    server.sin_port = htons (port);
  
  /* ne conectam la server */
    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
        perror ("[client]Eroare la connect().\n");
        return errno;
    }

    /* citirea mesajului */
    printf ("[client]Introduceti comanda: ");
    fflush (stdout);
    //read (0, buf, sizeof(buf));
    scanf("%s",buf);

    printf("[client] Am citit %s\n",buf);

    while(strcmp(buf, "quit")!=0)
    {
        if(strcmp(buf, "register")==0)
        {
            /* trimiterea mesajului la server */
          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);

          printf ("[client]Seteaza username: ");
          fflush (stdout);
          //read (0, buf, sizeof(buf));
          bzero(buf, sizeof(buf));
          scanf("%s", buf);

          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);

          char warning[]="username-ul este deja existent";
          warning[30]='\0'; 

          while(strcmp(buf, warning)==0)
          {
              printf ("[client]Seteaza alt username: ");
              fflush (stdout);
              bzero(buf, sizeof(buf));
              scanf("%s", buf);

              if(write (sd, buf, sizeof(buf)) <= 0)
              { 
                perror ("[client]Eroare la write() spre server.\n");
                return errno;           
              }

              if (read (sd, buf, sizeof(buf)) < 0)
              {
                  perror ("[client]Eroare la read() de la server.\n");
                  return errno;
              }

              printf ("[client]Mesajul primit este: %s\n", buf);
          }

          printf ("[client]Password: ");
          fflush (stdout);
          //read (0, buf, sizeof(buf));
          bzero(buf, sizeof(buf));
          scanf("%s", buf);

          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);
        }

        if(strcmp("login", buf)==0)
        {
          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);

          printf ("[client]Username: ");
          fflush (stdout);
          //read (0, buf, sizeof(buf));
          bzero(buf, sizeof(buf));
          scanf("%s", buf);

          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);

          printf ("[client]Password: ");
          fflush (stdout);
          //read (0, buf, sizeof(buf));
          bzero(buf, sizeof(buf));
          scanf("%s", buf);

          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);
        }

        if(strcmp("conversatii", buf)==0)
        {
          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          int count;
          if (read (sd, &count, sizeof(int)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %d\n", count);

          if(count!=-1)
          {
            char conversations[256][256];
            ssize_t bytes_read = read_wrapper(sd, conversations, sizeof(conversations));

            if (bytes_read> 0) 
            {
                printf("Read %zd bytes to the socket.\n", bytes_read);
            }
            else printf("Error at bytes_read.\n");

            printf ("[client]Mesajul primit este: \n");

            int i;
            for(i=0; i<count ; i++)
            {
                printf("%s\n", conversations[i]);
            }
          }
          else printf("Clientul nu este logged in!\n");
        }

        if(strcmp("trimite_mesaj", buf)==0)
        {
          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);

          if(strcmp(buf, "clientul nu e logged in!")!=0)
          {
            printf ("[client]Catre: ");
            fflush (stdout);
            //read (0, buf, sizeof(buf));
            bzero(buf, sizeof(buf));
            scanf("%s", buf);

            if(write (sd, buf, sizeof(buf)) <= 0)
            { 
              perror ("[client]Eroare la write() spre server.\n");
              return errno;           
            }

            if (read (sd, buf, sizeof(buf)) < 0)
            {
                perror ("[client]Eroare la read() de la server.\n");
                return errno;
            }

            printf ("[client]Mesajul primit este: %s\n", buf);

            printf ("[client]Mesaj: ");
            fflush (stdout);
            bzero(buf, sizeof(buf));
            read (0, buf, sizeof(buf));
            //fgets(buf, 101, stdin);
            buf[strlen(buf)-1]='\0';

            if(write (sd, buf, sizeof(buf)) <= 0)
            { 
              perror ("[client]Eroare la write() spre server.\n");
              return errno;           
            }

            if (read (sd, buf, sizeof(buf)) < 0)
            {
                perror ("[client]Eroare la read() de la server.\n");
                return errno;
            }

            printf ("[client]Mesajul primit este: %s\n", buf);
          }
        }

        if(strcmp("istoric_mesaje", buf)==0)
        {
          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);

          if(strcmp(buf, "clientul nu e logged in!")!=0)
          {
            printf ("[client]Cu: ");
            fflush (stdout);
            //read (0, buf, sizeof(buf));
            bzero(buf, sizeof(buf));
            scanf("%s", buf);

            if(write (sd, buf, sizeof(buf)) <= 0)
            { 
              perror ("[client]Eroare la write() spre server.\n");
              return errno;           
            }

            int count;
            if (read (sd, &count, sizeof(int)) < 0)
            {
                perror ("[client]Eroare la read() de la server.\n");
                return errno;
            }

            printf ("[client]Mesajul primit este: %d\n", count);

            char messages[256][256];
            ssize_t bytes_read = read_wrapper(sd, messages, sizeof(messages));

            if (bytes_read> 0) 
            {
                printf("Read %zd bytes to the socket.\n", bytes_read);
            }
            else printf("Error at bytes_read.\n");

            printf ("[client]Mesajul primit este: \n");

            int i;
            for(i=0; i<count ; i++)
            {
                printf("%s\n", messages[i]);
            }
          }
        }

        if(strcmp("load_more", buf)==0)
        {
          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);

          if(strcmp(buf, "clientul nu e logged in!")!=0 && strcmp(buf, "intra intr-o conversatie!")!=0)
          {
            int count;
            if (read (sd, &count, sizeof(int)) < 0)
            {
                perror ("[client]Eroare la read() de la server.\n");
                return errno;
            }

            printf ("[client]Mesajul primit este: %d\n", count);

            char messages[256][256];
            ssize_t bytes_read = read_wrapper(sd, messages, sizeof(messages));

            if (bytes_read> 0) 
            {
                printf("Read %zd bytes to the socket.\n", bytes_read);
            }
            else printf("Error at bytes_read.\n");

            printf ("[client]Mesajul primit este: \n");

            int i;
            for(i=0; i<count ; i++)
            {
                printf("%s\n", messages[i]);
            }
          }

        }

        if(strcmp("reply", buf)==0)
        {
          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);

          if(strcmp(buf, "clientul nu e logged in!")!=0 && strcmp(buf, "intra intr-o conversatie!")!=0)
          {
            printf ("[client]Mesajul: ");
            fflush (stdout);
            int id;
            scanf("%d", &id);

            if(write (sd, &id, sizeof(int)) <= 0)
            { 
              perror ("[client]Eroare la write() spre server.\n");
              return errno;           
            }

            if (read (sd, buf, sizeof(buf)) < 0)
            {
                perror ("[client]Eroare la read() de la server.\n");
                return errno;
            }

            printf ("[client]Mesajul primit este: %s\n", buf);

            printf ("[client]Reply cu: ");
            fflush (stdout);
            bzero(buf, sizeof(buf));
            read (0, buf, sizeof(buf));
            buf[strlen(buf)-1]='\0';
            //scanf("%s", buf);

            if(write (sd, buf, sizeof(buf)) <= 0)
            { 
              perror ("[client]Eroare la write() spre server.\n");
              return errno;           
            }

            if (read (sd, buf, sizeof(buf)) < 0)
            {
                perror ("[client]Eroare la read() de la server.\n");
                return errno;
            }

            printf ("[client]Mesajul primit este: %s\n", buf);
          }
        }

        if(strcmp("logout", buf)==0)
        {
          if(write (sd, buf, sizeof(buf)) <= 0)
          { 
            perror ("[client]Eroare la write() spre server.\n");
            return errno;           
          }

          if (read (sd, buf, sizeof(buf)) < 0)
          {
              perror ("[client]Eroare la read() de la server.\n");
              return errno;
          }

          printf ("[client]Mesajul primit este: %s\n", buf);
        }

        /* citirea mesajului */
        printf ("[client]Introduceti comanda: ");
        fflush (stdout);
        //read (0, buf, sizeof(buf));
        bzero(buf, sizeof(buf));
        scanf("%s", buf);

        
        printf("[client] Am citit %s\n",buf);
    }

    /* trimiterea mesajului la server */
        if(write (sd, buf, sizeof(buf)) <= 0)
        {
             perror ("[client]Eroare la write() spre server.\n");
             return errno;           
        }

        /* citirea raspunsului dat de server 
        (apel blocant pina cind serverul raspunde) */
        int nr;
        if (read (sd, &nr, sizeof(int)) < 0)
            {
            perror ("[client]Eroare la read() de la server.\n");
            return errno;
        }

        printf ("[client]Mesajul primit este: %d\n", nr);

    /* inchidem conexiunea, am terminat */
     close (sd);
}