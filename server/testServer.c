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

#define MYPORT "3490"
#define BACKLOG 20

int main(int argc, char const *argv[])
{
	//addrinfo is the address struct
	struct addrinfo hints, *res;
	//sockfd is the socket file descriptor
	int sockfd, new_fd;

	//Others trying to connect
	struct sockaddr_storage their_addr;
	socklen_t addr_size;


	//Loads up address struct
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(NULL, MYPORT, &hints, &res)==-1) {
		printf("Error on getaddrinfo():Getting address info error\n");
	}

	//Create a socket
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if(sockfd==-1) {
		printf("Error on socket():Creating socket error\n");
	}

	//Bind the socket to a local port (not always necessary)
	//Kernel (Network OS will bind to a local port if needed)
	if(bind(sockfd, res->ai_addr, res->ai_addrlen)==-1) {
		printf("Error on bind():Binding error\n");
	}

	//Connect to an IP adress on a port
	// if(connect(sockfd,res->ai_addr,res->ai_addrlen)==-1) {
	// 	printf("Error on connect(): Connecting error\n");
	// }

	//Start listening
	listen(sockfd,BACKLOG);


	//Start accepting connections from others
	addr_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr_storage*)&their_addr,&addr_size);

	if(new_fd==-1) {
		printf("Error on accept():Accepting error\n");
	}


	//Allows reusing/rebinding of the socket port
	int yes = 1;

	// if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
	// 	perror("setsockopt");
	// 	exit(1);
	// } 





	return 0;
}