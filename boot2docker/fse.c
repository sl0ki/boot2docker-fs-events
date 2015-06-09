#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>

void trigger_change_e (char *filename) {
   // try something like that 

   // FILE *fp;
   // if( access(filename, F_OK) != -1 ) {
   //     fp = fopen(filename, "a+");
   //     fclose(fp);
   // } 
}

int main(int argc, char**argv)
{
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;
   char mesg[1000];

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(49717);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)); 

   for (;;)
   {
      len = sizeof(cliaddr);
      n = recvfrom(sockfd, mesg, sizeof(mesg), 0, (struct sockaddr *)&cliaddr, &len);
      mesg[n] = 0;
      trigger_change_e(mesg);
      printf("file chnaged: %s",mesg);
   }
}