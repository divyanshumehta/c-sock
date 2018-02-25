#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include<pthread.h>
#include <mysql/mysql.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void *connection_handler(void *socket_desc)
{

    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char message[2000] , client_message[2000];

    // Open DB connection
    // MYSQL *con = mysql_init(NULL);
    //
    // if (con == NULL)
    // {
    //     fprintf(stderr, "%s\n", mysql_error(con));
    //     exit(1);
    // }
    //
    // if (mysql_real_connect(con, "localhost", "root", "jharvard",NULL, 0, NULL, 0) == NULL)
    // {
    //     fprintf(stderr, "%s\n", mysql_error(con));
    //     mysql_close(con);
    //     exit(1);
    // }


    //Send some messages to the client
    while(1)
    {
      strcpy(message,"Greetings! Welcome to E-Sim Hotel\nI am your booking handler\n");
      strcat(message,"Enter 1. to make a booking\n");
      strcat(message,"Enter 2. to cancel existing booking\n");
      strcat(message,"Enter 3. to quit\n");
      strcat(message,"Choice: ");
      send(sock , message , strlen(message), 0);

      read_size = recv(sock , client_message , 2000 , 0);
      if(read_size == 0)
      {
          puts("Client disconnected");
          fflush(stdout);
          break;
      }
      else if(read_size == -1)
      {
          perror("recv failed");
          break;
      }
      int res = atoi(client_message);
      if(res==1)
      {
        puts("booking done");
        fflush(stdout);
      }
      else if(res==2)
      {
        puts("booking cancelled");
        fflush(stdout);
      }
      else
      {
        puts("Close");
        fflush(stdout);
      }

    }

    //Free the socket pointer
    free(socket_desc);

    // Close MYSQL connection
    // mysql_close(con);

    pthread_exit(0);
}

int main(int argc, char *argv[])
{

  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  printf("Building up server...\n");
  struct sockaddr_in serv_addr, cli_addr;
  int n;

  // Check for proper execution of program
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  // Create socket descriptor sucessfully
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  // Set Socket peoperties
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(portno);

  // Sucessfully bind the socket to IP and Port
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  printf("Server listening on port %s\n",argv[1]);

  // Make passive socket
  listen(sockfd,10);
  clilen = sizeof(cli_addr);

  pthread_t tid[10];
  int i=0;
  while(1)
  {

    for(i=0;i<10;++i)
    {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

      int *new_sock = malloc(sizeof(int *));
      *new_sock = newsockfd;
      struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&new_sock;
      struct in_addr ipAddr = pV4Addr->sin_addr;
      char str[INET_ADDRSTRLEN];
      inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
      printf("Connection from %s started\n",str);

      pthread_create( &tid[i], NULL, connection_handler, (void*) new_sock);
      printf("Thread %d handling %s\n",i,str);
    }

  }

  close(sockfd);
  return 0;

}
