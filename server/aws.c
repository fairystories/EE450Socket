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
#define ServerB_PORT 22064
#define ServerC_PORT 23064
#define BACKLOG 20
#define MAXRECV 100

void *get_in_addr(struct sockaddr*);
float calculateLog(float x1, float x2, float x3, float x4, float x5, float x6);
float calculateDiv(float x1, float x2, float x3, float x4, float x5, float x6);
float convertStrToFloat(char *in);
void convertFloatToString(float number, char* result);

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
	int numbytes;

	struct sockaddr_in aws_sin, serA_sin, serB_sin, serC_sin;
	int sockfd_serA, sockfd_serB, sockfd_serC;

	//TCP socket code snippet from Beej's
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


	memset(&aws_sin, 0, sizeof(aws_sin));
    aws_sin.sin_family=AF_INET;
    aws_sin.sin_port=htons(AWS_UDP_PORT);

	/*---------------SERVERA SOCKET SETUP!!!----------------*/

	//Create a socket
	sockfd_serA = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(sockfd_serA==-1) {
		perror("aws serA socket()");
	}

	memset(&serA_sin,0,sizeof(serA_sin));
    serA_sin.sin_family=AF_INET;
    serA_sin.sin_port=htons(ServerA_PORT);

	/*--------------------SERVERA SOCKET SETUP DONE-------------------------*/

	/*---------------SERVERB SOCKET SETUP!!!----------------*/

	//Create a socket
	sockfd_serB = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(sockfd_serB==-1) {
		perror("aws serA socket()");
	}

	memset(&serB_sin,0,sizeof(serB_sin));
    serB_sin.sin_family=AF_INET;
    serB_sin.sin_port=htons(ServerB_PORT);

	/*--------------------SERVERB SOCKET SETUP DONE-------------------------*/

	/*---------------SERVERC SOCKET SETUP!!!----------------*/

	//Create a socket
	sockfd_serC = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(sockfd_serC==-1) {
		perror("aws serC socket()");
	}

	memset(&serC_sin,0,sizeof(serC_sin));
    serC_sin.sin_family=AF_INET;
    serC_sin.sin_port=htons(ServerC_PORT);

	/*--------------------SERVERC SOCKET SETUP DONE-------------------------*/

	while(1) {

		//Start accepting connections from client----------------------------------------------------
		client_size = sizeof client_addr;
		new_fd = accept(sockfd_client, (struct sockaddr*)&client_addr,&client_size);

		if(new_fd==-1) {
			perror("accept");
		}

		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr),conn_src,sizeof conn_src);

		int bytes_sent;

		//Receveing Function and Input
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

		//Send input to server A
		char *inputToSerA = input;
		if(sendto(sockfd_serA, inputToSerA, strlen(inputToSerA), 0, (struct sockaddr*)&serA_sin, sizeof(struct sockaddr))==-1) {
			perror("aws sendto serA");
			break;
		}
		printf("The AWS sent < %s > to Backend-Server A\n", inputToSerA);

		//Send input to server B
		char *inputToSerB = input;
		if(sendto(sockfd_serB, inputToSerB, strlen(inputToSerB), 0, (struct sockaddr*)&serB_sin, sizeof(struct sockaddr))==-1) {
			perror("aws sendto serB");
			break;
		}
		printf("The AWS sent < %s > to Backend-Server B\n", inputToSerB);

		//Send input to server C
		char *inputToSerC = input;
		if(sendto(sockfd_serC, inputToSerC, strlen(inputToSerC), 0, (struct sockaddr*)&serC_sin, sizeof(struct sockaddr))==-1) {
			perror("aws sendto serC");
			break;
		}
		printf("The AWS sent < %s > to Backend-Server C\n", inputToSerC);


		//Recieve result from A
		char resultFromA[MAXRECV];
		int serA_sin_len = sizeof(struct sockaddr);
		if((numbytes = recvfrom(sockfd_serA, resultFromA, sizeof(resultFromA), 0, (struct sockaddr*)&serA_sin, &serA_sin_len))==-1) {
			perror("serA recvfrom1");
			break;
		}
		resultFromA[numbytes] = '\0';
		printf("The AWS received < %s > Backend-Server <A> using UDP over port < %d >\n", resultFromA, AWS_UDP_PORT);

		//Recieve result from B
		char resultFromB[MAXRECV];
		int serB_sin_len = sizeof(struct sockaddr);
		if((numbytes = recvfrom(sockfd_serB, resultFromB, sizeof(resultFromB), 0, (struct sockaddr*)&serB_sin, &serB_sin_len))==-1) {
			perror("serB recvfrom1");
			break;
		}
		resultFromB[numbytes] = '\0';
		printf("The AWS received < %s > Backend-Server <B> using UDP over port < %d >\n", resultFromB, AWS_UDP_PORT);

		//Recieve result from C
		char resultFromC[MAXRECV];
		int serC_sin_len = sizeof(struct sockaddr);
		if((numbytes = recvfrom(sockfd_serC, resultFromC, sizeof(resultFromC), 0, (struct sockaddr*)&serC_sin, &serC_sin_len))==-1) {
			perror("serC recvfrom1");
			break;
		}
		resultFromC[numbytes] = '\0';
		printf("The AWS received < %s > Backend-Server <C> using UDP over port < %d >\n", resultFromC, AWS_UDP_PORT);

		//Send ResultA to A
		char *resAToA = resultFromA;
		if(sendto(sockfd_serA, resAToA, strlen(resAToA), 0, (struct sockaddr*)&serA_sin, sizeof(struct sockaddr))==-1) {
			perror("aws resA sendto serA");
			break;
		}
		printf("The AWS sent < %s > to Backend-Server A\n", resAToA);

		//Send ResultB to B
		char *resAToB = resultFromA;
		if(sendto(sockfd_serB, resAToB, strlen(resAToB), 0, (struct sockaddr*)&serB_sin, sizeof(struct sockaddr))==-1) {
			perror("aws resA sendto serB");
			break;
		}
		printf("The AWS sent < %s > to Backend-Server B\n", resAToB);


		//Recieve sec result from A
		char secResultFromA[MAXRECV];
		if((numbytes = recvfrom(sockfd_serA, secResultFromA, sizeof(secResultFromA), 0, (struct sockaddr*)&serA_sin, &serA_sin_len))==-1) {
			perror("serA recvfrom1");
			break;
		}
		secResultFromA[numbytes] = '\0';
		printf("The AWS received < %s > Backend-Server <A> using UDP over port < %d >\n", secResultFromA, AWS_UDP_PORT);

		//Recieve sec result from B
		char secResultFromB[MAXRECV];
		if((numbytes = recvfrom(sockfd_serB, secResultFromB, sizeof(secResultFromB), 0, (struct sockaddr*)&serB_sin, &serB_sin_len))==-1) {
			perror("serB recvfrom1");
			break;
		}
		secResultFromB[numbytes] = '\0';
		printf("The AWS received < %s > Backend-Server <B> using UDP over port < %d >\n", secResultFromB, AWS_UDP_PORT);

		
		printf("Values of powers received by AWS: < %s, %s, %s, %s, %s, %s >\n", input, resultFromA, resultFromB, secResultFromA, resultFromC, secResultFromB);

		//Converting the calculated results (powers) from string to float
		float x1 = convertStrToFloat(input);
		float x2 = convertStrToFloat(resultFromA);
		float x3 = convertStrToFloat(resultFromB);
		float x4 = convertStrToFloat(secResultFromA);
		float x5 = convertStrToFloat(resultFromC);
		float x6 = convertStrToFloat(secResultFromB);

		char finalResult[50];

		//determining whether the function is log or div
		int isLog = 1;
		char* logFunc = "LOG";
		if(strcmp(func,logFunc)!=0) {
			isLog = 0;
		}

		if(isLog) {
			float LogResult = calculateLog(x1,x2,x3,x4,x5,x6);
			printf("AWS calculated LOG on < %s >: < %f >\n", input, LogResult);
			convertFloatToString(LogResult, finalResult);
		}
		else {
			float DivResult = calculateDiv(x1,x2,x3,x4,x5,x6);
			printf("AWS calculated DIV on < %s >: < %f >\n", input, DivResult);
			convertFloatToString(DivResult, finalResult);
		}


		if((bytes_sent = send(new_fd, finalResult, strlen(finalResult), 0))<0) {
			perror("sendFinalResult");
		}
		printf("The AWS sent < %s > to client\n", finalResult);

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

//Log function
float calculateLog(float x1, float x2, float x3, float x4, float x5, float x6) {
	return (float)((-1.0)*x1-(x2/2.0)-(x3/3.0)-(x4/4.0)-(x5/5.0)-(x6/6.0));
}

//Div function
float calculateDiv(float x1, float x2, float x3, float x4, float x5, float x6) {
	return (float)(1.0+x1+x2+x3+x4+x5+x6);
}

//Convert string to float
float convertStrToFloat(char *in) {
	return strtof(in,NULL);
}

//Convert float to string
void convertFloatToString(float number, char* result) {
	int length = snprintf(NULL, 0, "%f", number);
	sprintf(result, "%f", number);
	result[length] = '\0';
}