#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void delay(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}


int main(int argc, char* argv[])
{
  // create socket
  int sockfd, portno, n, epollfd, nfds, read_size;
  struct sockaddr_in serv_addr;
  struct epoll_event ev;

  char buffer[2000];
  if (argc < 3) {
    fprintf(stderr,"Usage %s [HOSTNAME] [PORT]\n", argv[0]);
    exit(0);
  }
  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");


  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  // bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port = htons(portno);
  printf("Want to connect to %s. Hit any key to continue \nor Ctrl+C to exit",argv[1]);
  getchar();
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    error("ERROR connecting");
  //

  char response[2000];
  while(1)
  {
    // Get menu
    if( read(sockfd , buffer , 2000) < 0)
    {
      puts("recv failed");
      return 0;
    }
    printf("%s",buffer);

    // Check for client close
    if(strcmp(buffer,"SOCKTERM")==0)
    {
      printf("Thank You for visiting eSim Hotel\n");
      close(sockfd);
      return 0;

    }

    // Send menu reponse
    scanf("%[^\n]%*c", response );
    // printf("%s",response);
    int res=atoi(response);
    send(sockfd , response , strlen(response)+1 , 0);
    printf("**Looping**");

    // Get details for booking/cancellation
    // read(sockfd , buffer , 2000);
    // printf("%s", buffer);
    //
    // // send details for booking/cancellation
    // if(res==1)
    // {
    //   // Send name
    //   fgets(response,2000,stdin);
    //   response[strlen(response) - 1] = 0;
    //   send(sockfd , response , strlen(response) , 0);
    //
    //   // Get and Send table details
    //   read(sockfd , buffer , 2000);
    //   printf("%s",buffer);
    //   fgets(response,2000,stdin);
    //   response[strlen(response) - 1] = 0;
    //   send(sockfd , response , strlen(response) , 0);
    //
    //   // Get and Send time
    //   read(sockfd , buffer , 2000);
    //   printf("%s",buffer);
    //   fgets(response,2000,stdin);
    //   response[strlen(response) - 1] = 0;
    //   printf("%s\n",response);
    //   send(sockfd , response , strlen(response) , 0);
    //
    //   // Get booking status
    //   // read(sockfd , buffer , 2000);
    //   // printf("%s",buffer);
    //   // printf("Press Enter to continue..");
    //   // getchar();
    //   // send(sockfd , response , strlen(response) , 0);
    //
    //
    // }
    // // else
    // // {
    // //
    // // }
   }

  close(sockfd);
  return 0;

}
