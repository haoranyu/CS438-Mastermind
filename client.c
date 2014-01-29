/*
** client.c -- a stream socket client demo
*/

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

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	char *port = "80";
	char *host = "127.0.0.1";
	int sockfd;  
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];


	if (argc != 3){
        printf("Use format: ./client host port\n");
        exit(1);
    }
    else{
    	host = argv[1];
    	port = argv[2];
    }

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("Connected to %s. Please guess a 4 digit number\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	while(1){
		char respond[200], guess[5];
		int numbytes;

		scanf("%s", &guess);

		if (send(sockfd, guess, 4, 0) == -1)
			perror("send");

		if ((numbytes = recv(sockfd, respond, 105, 0)) == -1) {
		    perror("recv");
		    exit(1);
		}

		respond[numbytes] = '\0';

		int correct = (int)respond[0] - 48;
		printf("%s\n", respond+1);

		if(correct == 0){
			printf("you lose\n");
			break;
		}
		else if(correct == 1){
			printf("you win\n");
			break;
		}
	}

	close(sockfd);

	return 0;
}
