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

#define SERA_PORT 21064
#define AWSPORT 24064
#define BACKLOG 20
#define MAXRECV 100

void convertFloatToString(float number, char* result);

int main(int argc, char const *argv[])
{
	//addrinfo is the address struct
	struct sockaddr_in serv_sin, serA_sin;
	int sockfd_serA;
	char buf[MAXRECV];
	int numbytes;

	//Create a socket
	sockfd_serA = socket(AF_INET, SOCK_DGRAM, 0);

	if(sockfd_serA==-1) {
		perror("serverA: socket()1");
	}

	memset(&serv_sin,0,sizeof(serv_sin));
    serv_sin.sin_family=AF_INET;
    serv_sin.sin_port=htons(AWSPORT);

	memset(&serA_sin,0,sizeof(serA_sin));
    serA_sin.sin_family=AF_INET;
    serA_sin.sin_port=htons(SERA_PORT);

    if(bind(sockfd_serA, (struct sockaddr*)&serA_sin, sizeof(serA_sin))==-1) {
    	perror("sockfd_serA bind");
    }

    printf("The Server A is up and running using UDP on port %d.\n", SERA_PORT);

	while(1) {
		// char *msgToSend = "serverA: Sending over UDP";
		// if(sendto(sockfd_serA, msgToSend, strlen(msgToSend), 0, (struct sockaddr*)&serv_sin, sizeof(struct sockaddr))==-1) {
		// 	perror("serverA sendto");
		// 	break;
		// }
		


	//	printf("UDP trying to send '%s' to serverB\n", msgToSend);

		char inputFromAWS_str[MAXRECV];
		int serv_sin_len = sizeof(struct sockaddr);
		if((numbytes = recvfrom(sockfd_serA, inputFromAWS_str, MAXRECV-1, 0, (struct sockaddr*)&serv_sin, (socklen_t*)&serv_sin_len))==-1) {
			perror("serverA recvfrom");
			break;
		}
		inputFromAWS_str[numbytes] = '\0';

		float inputFromAWS_f = strtof(inputFromAWS_str,NULL);
		printf("The Server A received input < %f >\n", inputFromAWS_f);


		float inputSqr_f = inputFromAWS_f*inputFromAWS_f;

		char sqrResult[50];
		convertFloatToString(inputSqr_f, sqrResult);

		printf("The Server A calculated square: < %s >\n", sqrResult);

		if(sendto(sockfd_serA, sqrResult, strlen(sqrResult), 0, (struct sockaddr*)&serv_sin, sizeof(struct sockaddr))==-1) {
			perror("serverA sendto1");
			break;
		}

		printf("The Server A finished sending the output to AWS\n");


		while(1) {}
	}

	close(sockfd_serA);

	return 0;
}

void convertFloatToString(float number, char* result) {
	int length = snprintf(NULL, 0, "%f", number);
	// char result[length+1];
	sprintf(result, "%f", number);
	result[length] = '\0';

	// return result;
}