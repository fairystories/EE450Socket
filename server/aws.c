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

#define MYPORT 25064
#define BACKLOG 20
#define MAXRECV 100

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
	char buf[MAXRECV];
	int numbytes;



	//Loads up address struct
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int htons_MYPORT = htons(MYPORT);
	int length = snprintf(NULL, 0, "%d", htons_MYPORT);

	char htons_MYPORT_str[length+1];
	sprintf(htons_MYPORT_str, "%d", htons_MYPORT);
	htons_MYPORT_str[length] = '\0';

	if(getaddrinfo("localhost", htons_MYPORT_str, &hints, &servinfo)!=0) {
		perror("server getaddrinfo()");
		return 1;
	}

	//Bind to first possible result
	for(p=servinfo;p!=NULL;p=p->ai_next) {
		//Create a socket
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if(sockfd==-1) {
			perror("server socket()");
			continue; //Continue to the next addrinfo
		}

		//Allows reusing/rebinding of the socket port
		if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
			perror("setsockopt");
			exit(1);
		}

		//Bind the socket to a local port (not always necessary)
		//Kernel (Network OS will bind to a local port if needed)
		if(bind(sockfd, p->ai_addr, p->ai_addrlen)==-1) {
			perror("server bind()");
			continue;
		}


		break; //if reached, it means successful in creating a socket and binding
	}

	freeaddrinfo(servinfo);

	if(p==NULL) {	//p==NULL means that for loop ended with no successful creating and/or binding
		printf("Unsuccessful creating and/or binding\n");
		return 2;
	}

	printf("server: sockfd for TCP (aws, client) is %d\n", sockfd);

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

		char *servMsg = "Server is sending this message to client";
		int bytes_sent;

		if((bytes_sent = send(new_fd, servMsg, strlen(servMsg), 0))<0) {
			perror("send");
		}
		printf("server: trying to send '%s' to client\n", servMsg);
		printf("sent %d bytes successfully\n", bytes_sent);

		printf("waiting to recieve from Client\n");
		if ((numbytes=recv(new_fd,buf,MAXRECV-1,0))==-1) {
			perror("recvFunction");
			exit(1);
		}
		buf[numbytes] = '\0';
		printf("server: received Function=%s and ",buf);
		char *ackFunc = "Got function";
		if((bytes_sent = send(new_fd, ackFunc, strlen(ackFunc), 0))<0) {
			perror("sendAckFunc");
		}

		if ((numbytes=recv(new_fd,buf,MAXRECV-1,0))==-1) {
			perror("recvInput");
			exit(1);
		}
		buf[numbytes] = '\0';
		printf("Input=%s\n",buf);

		char *ackInput = "Got input";
		if((bytes_sent = send(new_fd, ackInput, strlen(ackInput), 0))<0) {
			perror("sendAckInput");
		}

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