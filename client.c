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

void error(char *msg)
{
    perror(msg);
    exit(0);
}


int main(int argc, char* argv[])
{
  // create socket
  int sockfd, portno, n, epollfd, nfds;
  struct sockaddr_in serv_addr;
  struct epoll_event ev;

  char buffer[256];
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

  if( recv(sockfd , buffer , 256 , 0) < 0)
  {
    puts("recv failed");
    return 0;
  }

  puts(buffer);

  // epollfd = epoll_create1(0);
  // if (epollfd == -1)
  // {
  //   perror("epoll_create1");
  //   exit(EXIT_FAILURE);
  // }
  // ev.events = EPOLLIN | EPOLLOUT;
  // if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
  // {
  //   perror("epoll_ctl: listen_sock");
  //   exit(EXIT_FAILURE);
  // }


  // while(1)
  // {
    fgets(buffer,255,stdin);

    //Send some data
    if( send(sockfd , buffer , strlen(buffer) , 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    // //Receive a reply from the server
    // if( recv(sockfd , buffer , 256 , 0) < 0)
    // {
    //     puts("recv failed");
    //     break;
    // }
    // puts(buffer);

   // }

  close(sockfd);
  return 0;

}
