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

#define SERB_PORT 22064
#define AWSPORT 24064
#define BACKLOG 20
#define MAXRECV 100

void convertFloatToString(float number, char* result);
float calculateCube(float in);

int main(int argc, char const *argv[])
{
	//addrinfo is the address struct
	struct sockaddr_in serv_sin, serB_sin;
	int sockfd_serB;
	char buf[MAXRECV];
	int numbytes;

	//Create a socket
	sockfd_serB = socket(AF_INET, SOCK_DGRAM, 0);

	if(sockfd_serB==-1) {
		perror("serverB: socket()1");
	}

	memset(&serv_sin,0,sizeof(serv_sin));
    serv_sin.sin_family=AF_INET;
    serv_sin.sin_port=htons(AWSPORT);

	memset(&serB_sin,0,sizeof(serB_sin));
    serB_sin.sin_family=AF_INET;
    serB_sin.sin_port=htons(SERB_PORT);

    if(bind(sockfd_serB, (struct sockaddr*)&serB_sin, sizeof(serB_sin))==-1) {
    	perror("sockfd_serB bind");
    }

    printf("The Server B is up and running using UDP on port %d.\n", SERB_PORT);

	while(1) {
		//Receving input from AWS
		char inputFromAWS_str[MAXRECV];
		int serv_sin_len = sizeof(struct sockaddr);
		if((numbytes = recvfrom(sockfd_serB, inputFromAWS_str, MAXRECV-1, 0, (struct sockaddr*)&serv_sin, (socklen_t*)&serv_sin_len))==-1) {
			perror("serverB recvfrom");
			break;
		}
		inputFromAWS_str[numbytes] = '\0';

		//Converting string input to float
		float inputFromAWS_f = strtof(inputFromAWS_str,NULL);
		printf("The Server B received input < %f >\n", inputFromAWS_f);

		//Calculate the third power of the input
		float inputCube_f = calculateCube(inputFromAWS_f);

		char cubeResult[50];
		convertFloatToString(inputCube_f, cubeResult);

		printf("The Server B calculated square: < %s >\n", cubeResult);

		//Send result back to AWS
		if(sendto(sockfd_serB, cubeResult, strlen(cubeResult), 0, (struct sockaddr*)&serv_sin, sizeof(struct sockaddr))==-1) {
			perror("serverB sendto1");
			break;
		}

		printf("The Server B finished sending the output to AWS\n");

	}

	//Close socket when reaching out of while loop (which is not possible in this code)
	//Closing can be done manually from the terminal
	close(sockfd_serB);

	return 0;
}

//Convert float to string
void convertFloatToString(float number, char* result) {
	int length = snprintf(NULL, 0, "%f", number);
	sprintf(result, "%f", number);
	result[length] = '\0';
}

//Calculating cube
float calculateCube(float in) {
	return in*in*in;
}