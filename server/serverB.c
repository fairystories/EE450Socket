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

#define AWSPORT 24064
#define BACKLOG 20
#define MAXRECV 1000

int main(int argc, char const *argv[])
{
	//addrinfo is the address struct
	struct sockaddr_in my_sin, other_sin;
	int sockfd;
	char buf[MAXRECV];
	int numbytes;

	//Create a socket
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(sockfd==-1) {
		perror("serverB: socket()");
	}

	memset(&my_sin, 0, sizeof(my_sin));
    my_sin.sin_family=AF_INET;
    my_sin.sin_port=htons(AWSPORT);

    int yes = 1;
    //Allows reusing/rebinding of the socket port
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
		perror("serverB setsockopt");
		exit(1);
	}

    if(bind(sockfd, (struct sockaddr*)&my_sin, sizeof(my_sin))==-1) {
    	perror("serverB bind");
    }

	printf("serverB: sockfd is %d\n", sockfd);

	while(1) {
		int other_sin_len = sizeof(other_sin);
		if((numbytes = recvfrom(sockfd, buf, MAXRECV-1, 0, (struct sockaddr*)&other_sin, &other_sin_len))==-1) {
			perror("serverB recvfrom");
			break;
		}
		buf[numbytes] = '\0';
		printf("UDP got '%s' from serverA\n", buf);
	}

	close(sockfd);

	return 0;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}    
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}