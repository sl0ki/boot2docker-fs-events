#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int sockfd;
struct sockaddr_in servaddr,cliaddr;

void udp_init(char *host, int port) {

  sockfd = socket(AF_INET,SOCK_DGRAM,0);
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(host);
  servaddr.sin_port = htons(port);
}

void udp_send(char *data, int data_len) {

  sendto(sockfd, data, data_len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
}
