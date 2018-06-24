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
    char username[31];
    int reservation_start_time, reservation_end_time, people, reservation_id;
} db[100];

int db_head;

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


    //Send some messages to the client
    int i=1;
    while(1)
    {
      // Send Menu
      strcpy(message,"Greetings! Welcome to E-Sim Hotel\nI am your booking handler\n");
      strcat(message,"Enter 1. to make a booking\n");
      strcat(message,"Enter 2. to cancel existing booking\n");
      strcat(message,"Enter 3. to quit\n");
      strcat(message,"Choice: ");
      write(sock , message , strlen(message)+1);

      // Response for booking/cancellation
      read(sock , client_message , 2000);
      printf("client %dth message: '%s'\n", i++,client_message);
      int res=atoi(client_message);
      // printf("%d",res);
      if(res==1)
      {
        strcpy(message, "You have opted to make a booking");
        strcat(message, "\nEnter your Name: ");
        write(sock, message, strlen(message) + 1);

        // Read name
        read(sock , client_message , 2000);
        printf("client name: '%s'\n", client_message);
        printf("1\n");
        struct booking new_booking;
        strcpy(new_booking.username,client_message);


        // Send and get table details
        strcpy(message, "Number of Seats: ");
        write(sock, message, strlen(message) + 1);
        read_size = read(sock , client_message , 2000);
        client_message[read_size]=0;
        printf("client name: '%s'\n", client_message);
        printf("12\n" );
        new_booking.people = atoi(client_message);

        // Send and get start time details
        strcpy(message, "Reservation Start Time(24hours) e.g 1200: ");
        write(sock, message, strlen(message) + 1);
        read_size = read(sock , client_message , 2000);
        client_message[read_size]=0;
        printf("client name: '%s'\n", client_message);
        new_booking.reservation_start_time = atoi(client_message);

        //Send and get end time
        strcpy(message, "Reservation End Time(24hours) e.g 1200: ");
        write(sock, message, strlen(message) + 1);
        read_size = read(sock , client_message , 2000);
        client_message[read_size]=0;
        printf("client name: '%s'\n", client_message);
        new_booking.reservation_end_time = atoi(client_message);

        // Save to DB
        new_booking.reservation_id = db_head;
        db[db_head] = new_booking;
        db_head++;

        // Send booking confirmation
        strcpy(message, "Reservation made Booking Id: ");
        char reservation_id[5];
        sprintf(reservation_id, "%d", new_booking.reservation_id);
        strcat(message, reservation_id);
        // printf("%s\n",message);
        strcat(message, "\nReturning to Main Menu...\n");
        write(sock, message, strlen(message));
        // read(sock, client_message, 2000);
      //
      }
      else if(res==2)
      {
        strcpy(message, "Recieved");
        write(sock, message, strlen(message) + 1);
      }
      else if (res == 3)
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
