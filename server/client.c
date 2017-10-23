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

#define AWSPORT 25064
#define MAXDATASIZE 100 // max number of bytes we can get at once 

void *get_in_addr(struct sockaddr*);
void subtractFromOne(char* input, char* result);

int main(int argc, char const *argv[])
{
	//addrinfo is the address struct
	struct addrinfo hints, *servinfo, *p;
	//sockfd is the socket file descriptor
	int sockfd, rv, numbytes;
	char buf[MAXDATASIZE];

	//Address of connection
	char conn_addr[INET_ADDRSTRLEN];



	//Loads up address struct
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int htons_AWSPORT = htons(AWSPORT);
	int length = snprintf(NULL, 0, "%d", htons_AWSPORT);

	char htons_AWSPORT_str[length+1];
	sprintf(htons_AWSPORT_str, "%d", htons_AWSPORT);
	htons_AWSPORT_str[length] = '\0';


	if((rv=getaddrinfo("0.0.0.0", htons_AWSPORT_str, &hints, &servinfo))!=0) {
		perror("server getaddrinfo()");
		return 1;
	}

	//Connect to first possible result
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

		break; //if reached, it means successful in creating a socket and connecting
	}


	if(p==NULL) {	//p==NULL means that for loop ended with no successful creating and/or connecting
		printf("Unsuccessful creating and/or binding\n");
		return 2;
	}

	printf("The client is up and running.\n");

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),conn_addr,sizeof conn_addr);
	// printf("client: connecting to %s\n", conn_addr);

	freeaddrinfo(servinfo);
	
	while (1) {
		// if ((numbytes=recv(sockfd,buf,MAXDATASIZE-1,0))==-1) {
		// 	perror("recv");
		// 	exit(1);
		// }
		
		// buf[numbytes] = '\0';
		// printf("client: received '%s'\n",buf);

		char bufCat[strlen(argv[1])+strlen(argv[2])+2];
		snprintf(bufCat, sizeof(bufCat), "%s %s", argv[1], argv[2]);

		// printf("after concat get %s\n", bufCat);

		char *input = (char*)argv[2];
		char subInput[50];
		subtractFromOne((char*)argv[2], subInput);
		int bytes_sent;

		if((bytes_sent = send(sockfd, input, strlen(input), 0))<0) {
			perror("sendInput");
		}
		if ((numbytes=recv(sockfd,buf,MAXDATASIZE-1,0))==-1) {
			perror("recv");
			exit(1);
		}

		
		buf[numbytes] = '\0';
		printf("Got ack: %s\n", buf);

		char *funcMsg = (char*)argv[1];
		
		if((bytes_sent = send(sockfd, funcMsg, strlen(funcMsg), 0))<0) {
			perror("sendFunction");
		}
		if ((numbytes=recv(sockfd,buf,MAXDATASIZE-1,0))==-1) {
			perror("recv");
			exit(1);
		}

		buf[numbytes] = '\0';
		printf("Got ack: %s\n", buf);
		printf("The client sent < %s > and %s to AWS\n", subInput, funcMsg);
		// printf("client: trying to send '%s' to server\n", bufCat);


		if ((numbytes=recv(sockfd,buf,MAXDATASIZE-1,0))==-1) {
			perror("recv");
			exit(1);
		}

		buf[numbytes] = '\0';
		printf("According to AWS, %s on < %s >: < %s >\n", funcMsg, input, buf);

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

void subtractFromOne(char* input, char* result) {
	float in = strtof(input,NULL);

	in = (float)(1.0-in);

	int length = snprintf(NULL, 0, "%f", in);
	sprintf(result, "%f", in);
	result[length] = '\0';
}