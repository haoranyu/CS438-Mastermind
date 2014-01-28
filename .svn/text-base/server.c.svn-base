/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 100 // max number of bytes we can get at once 
void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int existArr(int c, int *arr){
	int i;
	for (i = 0; i < 4; ++i) {
		if(arr[i] == (c - 48))
			return 1;
	}
	return 0;
}
void insertArr(int c, int *arr){
	int i;
	for (i = 0; i < 4; ++i) {
		if(arr[i] == -1){
			arr[i] = (c - 48);
			break;
		}
	}
}
void evaluate(char *ans, char *guess, char *respond, int counter){
	int i, j;
	int match = 0;
	int exist = 0;
	int match_array[4] = {-1,-1,-1,-1};
	int exist_array[4] = {-1,-1,-1,-1};

	for(i = 0; i < 4; i++){
		if(ans[i] == guess[i]){
			match++;
			insertArr(guess[i], match_array);
		}
	}
	for(i = 0; i < 4; i++){
		if(ans[i] != guess[i]){
			for(j = 0; j < 4; j++){
				if(!(existArr(guess[i],match_array) || existArr(guess[i],exist_array)) && ans[j] == guess[i]){
					exist++;
					insertArr(guess[i], exist_array);
				}
			}
		}
	}
	//printf("%d%d%d%d\n",match_array[0],match_array[1],match_array[2],match_array[3]);
	//printf("%d%d%d%d\n",exist_array[0],exist_array[1],exist_array[2],exist_array[3]);

	if(match == 4)
		sprintf(respond, "1%d correct color+slot, %d correct colors", match, exist);
	else if(counter == 8)
		sprintf(respond, "0%d correct color+slot, %d correct colors", match, exist);
	else
		sprintf(respond, "2%d correct color+slot, %d correct colors", match, exist);
}
void getRandom(char *aim){
	char *set = "012345";
	int i;
	char str[4];
	srand((unsigned int)time((time_t *)NULL));
	for(i = 0; i < 4; i++){
		str[i] = set[(rand()%6)];
	}
	str[4] = '\0';
	strcpy(aim, str);
}
int main(int argc, char* argv[])
{

	char ans[7];
	char *port = "80";
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	if (argc != 2){
        printf("Use format: ./server port\n");
        exit(1);
    }
    else{
    	port = argv[1];
    }

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	scanf("%s", &ans);
	if(!strcmp(ans,"random")){
		getRandom(ans);
	}

	//printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		//printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			int counter = 0;

			while(counter < 8){
				counter++;
				char guess[5];
				char respond[50];
				char toclient[50];
				if ((recv(new_fd, guess, 4, 0)) == -1) {
				    perror("recv");
				    exit(1);
				}
				guess[4] = '\0';
				evaluate(ans, guess, respond, counter);

				if (send(new_fd, respond, 100, 0) == -1)
					perror("send");
			}
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}
