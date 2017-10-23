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

#define AWS_TCP_PORT 25064
#define AWS_UDP_PORT 24064
#define ServerA_PORT 21064
#define BACKLOG 20
#define MAXRECV 100

void *get_in_addr(struct sockaddr*);

int main(int argc, char const *argv[])
{
	//addrinfo is the address struct
	struct addrinfo hints, *servinfo, *p;
	//sockfd is the socket file descriptor
	int sockfd_client, new_fd;

	//For reusing port
	int yes = 1;

	//Others trying to connect
	struct sockaddr_storage client_addr;
	socklen_t client_size;
	char conn_src[INET_ADDRSTRLEN];
	// char buf[MAXRECV];
	int numbytes;

	struct sockaddr_in aws_sin, serA_sin, serB_sin, serC_sin;
	int sockfd_serA, sockfd_serB, sockfd_serC;


	//Loads up address struct
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int htons_AWS_TCP_PORT = htons(AWS_TCP_PORT);
	int length = snprintf(NULL, 0, "%d", htons_AWS_TCP_PORT);

	char htons_AWS_TCP_PORT_str[length+1];
	sprintf(htons_AWS_TCP_PORT_str, "%d", htons_AWS_TCP_PORT);
	htons_AWS_TCP_PORT_str[length] = '\0';

	if(getaddrinfo("localhost", htons_AWS_TCP_PORT_str, &hints, &servinfo)!=0) {
		perror("server getaddrinfo()");
		return 1;
	}

	//Bind to first possible result
	for(p=servinfo;p!=NULL;p=p->ai_next) {
		//Create a socket
		sockfd_client = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if(sockfd_client==-1) {
			perror("server socket()");
			continue; //Continue to the next addrinfo
		}

		//Allows reusing/rebinding of the socket port
		if (setsockopt(sockfd_client,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
			perror("setsockopt");
			exit(1);
		}

		//Bind the socket to a local port (not always necessary)
		//Kernel (Network OS will bind to a local port if needed)
		if(bind(sockfd_client, p->ai_addr, p->ai_addrlen)==-1) {
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

	printf("server: sockfd for TCP (aws, client) is %d\n", sockfd_client);

	//Start listening
	listen(sockfd_client,BACKLOG);

	printf("The AWS is up and running\n");

	/*---------------SERVERA SOCKET SETUP!!!----------------*/

	//Create a socket
	sockfd_serA = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(sockfd_serA==-1) {
		perror("aws serA socket()");
	}

	memset(&serA_sin,0,sizeof(serA_sin));
    serA_sin.sin_family=AF_INET;
    serA_sin.sin_port=htons(ServerA_PORT);

	memset(&aws_sin, 0, sizeof(aws_sin));
    aws_sin.sin_family=AF_INET;
    aws_sin.sin_port=htons(AWS_UDP_PORT);

    if(bind(sockfd_serA, (struct sockaddr*)&aws_sin, sizeof(aws_sin))==-1) {
    	perror("sockfd_serA bind");
    }

	printf("aws udp for serA: sockfd is %d\n", sockfd_serA);

	/*--------------------SERVERA SOCKET SETUP DONE-------------------------*/

	while(1) {


		//-------------------Wait to recieve from server A-------------------------
		// char serA_conn[MAXRECV];
		// int serA_sin_len = sizeof(serA_sin);
		// if((numbytes = recvfrom(sockfd_serA, serA_conn, MAXRECV-1, 0, (struct sockaddr*)&serA_sin, &serA_sin_len))==-1) {
		// 	perror("serA recvfrom");
		// 	break;
		// }
		// serA_conn[numbytes] = '\0';
		// printf("UDP got '%s' from serverA\n", serA_conn);


		//Send input to server A, B, C
		// char *initMsgToA = "Hi A I am AWS";
		// if(sendto(sockfd_serA, inputToSerA, strlen(inputToSerA), 0, (struct sockaddr*)&serA_sin, sizeof(struct sockaddr))==-1) {
		// 	perror("aws sendto serA");
		// 	break;
		// }

		// printf("AWS trying to send '%s' to serverA and sockfd = %d\n", initMsgToA, sockfd_serA);
		//--------------------------------------------------------------------------


		//Start accepting connections from client----------------------------------------------------
		client_size = sizeof client_addr;
		new_fd = accept(sockfd_client, (struct sockaddr*)&client_addr,&client_size);

		if(new_fd==-1) {
			perror("accept");
		}

		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr),conn_src,sizeof conn_src);
		printf("server: connection from %s\n", conn_src);

		// char *servMsg = "Server is sending this message to client";
		int bytes_sent;

		// if((bytes_sent = send(new_fd, servMsg, strlen(servMsg), 0))<0) {
		// 	perror("send");
		// }
		// printf("server: trying to send '%s' to client\n", servMsg);
		// printf("sent %d bytes successfully\n", bytes_sent);

		// printf("waiting to recieve from Client\n");
		char input[MAXRECV];
		if ((numbytes=recv(new_fd,input,MAXRECV-1,0))==-1) {
			perror("recvFunction");
			exit(1);
		}
		input[numbytes] = '\0';
		printf("The AWS received input < %s > and ",input);

		char *ackFunc = "Got function";
		if((bytes_sent = send(new_fd, ackFunc, strlen(ackFunc), 0))<0) {
			perror("sendAckInput");
		}

		char func[MAXRECV];
		if ((numbytes=recv(new_fd,func,MAXRECV-1,0))==-1) {
			perror("recvInput");
			exit(1);
		}
		func[numbytes] = '\0';
		printf("function=%s from the client using TCP over port %d\n",func, AWS_TCP_PORT);

		char *ackInput = "Got input";
		if((bytes_sent = send(new_fd, ackInput, strlen(ackInput), 0))<0) {
			perror("sendAckFunction");
		}

		//AWS GOT BOTH FUNCTION AND INPUT FROM THE CLIENT----------------------

		//Send input to server A, B, C
		char *inputToSerA = input;
		if(sendto(sockfd_serA, inputToSerA, strlen(inputToSerA), 0, (struct sockaddr*)&serA_sin, sizeof(struct sockaddr))==-1) {
			perror("aws sendto serA");
			break;
		}

		printf("The AWS sent < %s > to Backend-Server A\n", inputToSerA);

		char sqrResultFromA[MAXRECV];
		int serA_sin_len = sizeof(struct sockaddr);
		if((numbytes = recvfrom(sockfd_serA, sqrResultFromA, sizeof(sqrResultFromA), 0, (struct sockaddr*)&serA_sin, &serA_sin_len))==-1) {
			perror("serA recvfrom1");
			break;
		}

		printf("The AWS received < %s > Backend-Server <A> using UDP over port < %d >\n", sqrResultFromA, AWS_UDP_PORT);

	}


	close(sockfd_client);
	close(sockfd_serA);
	close(sockfd_serB);
	close(sockfd_serC);

	return 0;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}    
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}