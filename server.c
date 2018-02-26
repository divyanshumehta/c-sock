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

struct booking {
    char username[31], id[11];
    int reservation_time, people;
};

void error(const char *msg)
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
      // Send Menu
      strcpy(message,"Greetings! Welcome to E-Sim Hotel\nI am your booking handler\n");
      strcat(message,"Enter 1. to make a booking\n");
      strcat(message,"Enter 2. to cancel existing booking\n");
      strcat(message,"Enter 3. to quit\n");
      strcat(message,"Choice: ");
      write(sock , message , strlen(message));

      // Response for booking/cancellation
      read_size = read(sock , client_message , 2000);
      printf("client message: '%s'\n", client_message);
      int res=atoi(client_message);
      if(res==1)
      {
        strcpy(message, "You have opted to make a booking");
        strcat(message, "\nEnter your Name: ");
        write(sock, message, strlen(message) + 1);

        // Read name
        read_size =  read(sock , client_message , 2000);
        printf("client message: '%s'\n", client_message);

        struct booking new_booking;
        strcpy(new_booking.username,client_message);


        // Send and get table details
        strcpy(message, "Number of Seats: ");
        write(sock, message, strlen(message) + 1);
        read_size = read(sock , client_message , 2000);
        client_message[read_size]=0;
        printf("client message: '%s'\n", client_message);

        // Send and get time details
        strcpy(message, "Reservation Time(24hours) e.g 1200: ");
        write(sock, message, strlen(message) + 1);
        read_size = read(sock , client_message , 2000);
        client_message[read_size]=0;
        printf("client message: '%s'\n", client_message);
        // printf("%s\n", "Hey");

        // Save to DB

        // Send booking confirmation
        strcpy(new_booking.id,"ACsr3");
        strcpy(message, "Reservation made Booking Id: ");
        strcat(message, new_booking.id);
        printf("%s\n",message);
        write(sock, message, strlen(message));

      }
      else if(res==2)
      {
        strcpy(message, "Recieved");
        write(sock, message, strlen(message) + 1);
      }
      else
      {
        strcpy(message, "SOCKTERM");
        write(sock, message, strlen(message) + 1);

      }
      printf("%s\n", "****LOOP*****");
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
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);

      int *new_sock = (int *)malloc(sizeof(int *));
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
