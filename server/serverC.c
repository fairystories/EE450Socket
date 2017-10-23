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

#define SERB_PORT 23064
#define AWSPORT 24064
#define BACKLOG 20
#define MAXRECV 100

void convertFloatToString(float number, char* result);
float calculateFifthPower(float in);

int main(int argc, char const *argv[])
{
	//addrinfo is the address struct
	struct sockaddr_in serv_sin, serC_sin;
	int sockfd_serC;
	char buf[MAXRECV];
	int numbytes;

	//Create a socket
	sockfd_serC = socket(AF_INET, SOCK_DGRAM, 0);

	if(sockfd_serC==-1) {
		perror("serverC: socket()1");
	}

	memset(&serv_sin,0,sizeof(serv_sin));
    serv_sin.sin_family=AF_INET;
    serv_sin.sin_port=htons(AWSPORT);

	memset(&serC_sin,0,sizeof(serC_sin));
    serC_sin.sin_family=AF_INET;
    serC_sin.sin_port=htons(SERB_PORT);

    if(bind(sockfd_serC, (struct sockaddr*)&serC_sin, sizeof(serC_sin))==-1) {
    	perror("sockfd_serC bind");
    }

    printf("The serverC is up and running using UDP on port %d.\n", SERB_PORT);

	while(1) {
		// char *msgToSend = "serverC: Sending over UDP";
		// if(sendto(sockfd_serC, msgToSend, strlen(msgToSend), 0, (struct sockaddr*)&serv_sin, sizeof(struct sockaddr))==-1) {
		// 	perror("serverC sendto");
		// 	break;
		// }
		


	//	printf("UDP trying to send '%s' to serverB\n", msgToSend);

		char inputFromAWS_str[MAXRECV];
		int serv_sin_len = sizeof(struct sockaddr);
		if((numbytes = recvfrom(sockfd_serC, inputFromAWS_str, MAXRECV-1, 0, (struct sockaddr*)&serv_sin, (socklen_t*)&serv_sin_len))==-1) {
			perror("serverC recvfrom");
			break;
		}
		inputFromAWS_str[numbytes] = '\0';

		float inputFromAWS_f = strtof(inputFromAWS_str,NULL);
		printf("The serverC received input < %f >\n", inputFromAWS_f);

		float inputFifth_f = calculateFifthPower(inputFromAWS_f);

		char fifthResult[50];
		convertFloatToString(inputFifth_f, fifthResult);

		printf("The serverC calculated square: < %s >\n", fifthResult);

		if(sendto(sockfd_serC, fifthResult, strlen(fifthResult), 0, (struct sockaddr*)&serv_sin, sizeof(struct sockaddr))==-1) {
			perror("serverC sendto1");
			break;
		}

		printf("The serverC finished sending the output to AWS\n");

	}

	close(sockfd_serC);

	return 0;
}

void convertFloatToString(float number, char* result) {
	int length = snprintf(NULL, 0, "%f", number);
	sprintf(result, "%f", number);
	result[length] = '\0';
}

float calculateFifthPower(float in) {
	return in*in*in*in*in;
}