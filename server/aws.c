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
#define BACKLOG 20

void *get_in_addr(struct sockaddr*);

int main(int argc, char const *argv[])
{
	//addrinfo is the address struct
	struct addrinfo hints, *servinfo, *p;
	//sockfd is the socket file descriptor
	int sockfd, new_fd;

	//For reusing port
	int yes = 1;

	//Others trying to connect
	struct sockaddr_storage their_addr;
	socklen_t their_size;
	char conn_src[INET_ADDRSTRLEN];



	//Loads up address struct
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(NULL, MYPORT, &hints, &servinfo)!=0) {
		perror("server getaddrinfo()");
	}

	for(p=servinfo;p!=NULL;p=p->ai_next) {
		//Create a socket
		sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

		if(sockfd==-1) {
			perror("server socket()");
			continue; //Continue to the next addrinfo
		}

		//Bind the socket to a local port (not always necessary)
		//Kernel (Network OS will bind to a local port if needed)
		if(bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)==-1) {
			perror("server bind()");
			continue;
		}

		//Allows reusing/rebinding of the socket port
		if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
			perror("setsockopt");
			exit(1);
		}

		break; //if reached, it means successful in creating a socket and binding
	}

	freeaddrinfo(servinfo);

	if(p==NULL) {	//p==NULL means that for loop ended with no successful creating and/or binding
		printf("Unsuccessful creating and/or binding\n");
	}

	//Connect to an IP adress on a port
	// if(connect(sockfd,res->ai_addr,res->ai_addrlen)==-1) {
	// 	printf("Error on connect(): Connecting error\n");
	// }

	//Start listening
	listen(sockfd,BACKLOG);

	printf("Server: waiting for connections...\n");

	while(1) {
		//Start accepting connections from others
		their_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr*)&their_addr,&their_size);

		if(new_fd==-1) {
			perror("accept");
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),conn_src,sizeof conn_src);
		printf("server: connection from %s\n", conn_src);
	}


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