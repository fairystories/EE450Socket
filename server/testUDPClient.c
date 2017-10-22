#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define MYPORT "21064"
#define AWSPORT 24064
#define BACKLOG 20
#define MAXRECV 100

void *get_in_addr(struct sockaddr*);

int main(int argc, char const *argv[])
{
	//addrinfo is the address struct
	struct sockaddr_in serv_sin;
	int sockfd;
	char buf[MAXRECV];
	int numbytes;

	//Create a socket
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(sockfd==-1) {
		perror("serverA: socket()");
		// continue; //Continue to the next addrinfo
	}

	memset((char *)&serv_sin,0,sizeof(serv_sin));
    serv_sin.sin_family=AF_INET;
    serv_sin.sin_port=AWSPORT;

	printf("serverA: sockfd is %d\n", sockfd);

	while(1) {
		char *msgToSend = "serverA: Sending over UDP";
		if(sendto(sockfd, msgToSend, strlen(msgToSend), 0, (struct sockaddr*)&serv_sin, sizeof(struct sockaddr))==-1) {
			perror("serverA sendto");
			break;
		}

		printf("UDP trying to send '%s' to serverB\n", msgToSend);

		char buf[MAXRECV];
		int serv_sin = sizeof(struct sockaddr);
		if((numbytes = recvfrom(sockfd, buf, MAXRECV-1, 0, (struct sockaddr*)&serv_sin, (socklen_t*)&serv_sin))==-1) {
			perror("serverA recvfrom");
			break;
		}
		buf[numbytes] = '\0';
		printf("UDP got '%s' from serverB\n", buf);
	}

	close(sockfd);

	return 0;
}