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

#define MYPORT "25064"
#define MAXDATASIZE 100 // max number of bytes we can get at once 

void *get_in_addr(struct sockaddr*);

int main(int argc, char const *argv[])
{
	//addrinfo is the address struct
	struct addrinfo hints, *servinfo, *p;
	//sockfd is the socket file descriptor
	int sockfd, new_fd, rv, numbytes;
	char buf[MAXDATASIZE];

	//For reusing port
	int yes = 1;

	//Others trying to connect
	struct sockaddr_storage their_addr;
	socklen_t their_size;
	char conn_addr[INET_ADDRSTRLEN];



	//Loads up address struct
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if((rv=getaddrinfo(argv[1], MYPORT, &hints, &servinfo))!=0) {
		perror("server getaddrinfo()");
		return 1;
	}

	for(p=servinfo;p!=NULL;p=p->ai_next) {
		//Create a socket
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if(sockfd==-1) {
			perror("server socket()");
			continue; //Continue to the next addrinfo
		}

		//Connect to an IP adress on a port
		if(connect(sockfd,p->ai_addr,p->ai_addrlen)==-1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break; //if reached, it means successful in creating a socket and binding
	}


	if(p==NULL) {	//p==NULL means that for loop ended with no successful creating and/or binding
		printf("Unsuccessful creating and/or binding\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),conn_addr,sizeof conn_addr);
	printf("client: connecting to %s\n", conn_addr);

	freeaddrinfo(servinfo);
	

	if ((numbytes=recv(sockfd,buf,MAXDATASIZE-1,0))==-1) {
		perror("recv");
		exit(1);
	}
	
	buf[numbytes] = '\0';
	printf("client: received '%s'\n",buf);
	close(sockfd);


	//Start accepting connections from others
/*	addr_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr_storage*)&their_addr,&addr_size);

	if(new_fd==-1) {
		perror("accept");
	}*/


	 





	return 0;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}    
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}