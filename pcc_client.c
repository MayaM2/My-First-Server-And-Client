#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>

unsigned int convertui(char *st) { //convert str to an unsigned int. taken from https://stackoverflow.com/questions/34206446/how-to-convert-string-into-unsigned-int-c
  char *x;
  for (x = st ; *x ; x++) {
    if (!isdigit(*x))
      return 0L;
  }
  return (strtoul(st, 0L, 10));
}

int main(int argc, char *argv[]){
	int sockfd = -1;
	int rc;
	unsigned int i=0;
	char random_char[1];
	int random_data;
	unsigned int length = convertui(argv[3]);
	unsigned int result;
	struct sockaddr_in serv_addr; // where we Want to get to
	struct sockaddr_in my_addr;   // where we actually connected through
	struct sockaddr_in peer_addr; // where we actually connected to

	socklen_t addrsize = sizeof(struct sockaddr_in );

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror(strerror(errno));
		exit(1);
	}
	getsockname(sockfd,(struct sockaddr*) &my_addr, &addrsize);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	if(connect(sockfd,(struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror(strerror(errno));
		exit(1);
	}
	getsockname(sockfd, (struct sockaddr*) &my_addr,   &addrsize);
	getpeername(sockfd, (struct sockaddr*) &peer_addr, &addrsize);

	// open /dev/urandom for reading
	random_data = open("/dev/urandom", O_RDONLY);
	if(random_data<0){
		perror(strerror(errno));
		close(sockfd);
		exit(1);
	}

	//transfer read data to server
	for(i=0;i<length;i++){
		rc = read(random_data, random_char, sizeof(char));
		if(rc<0){
			perror(strerror(errno));
			close(sockfd);
			exit(1);
		}
		rc = write(sockfd,random_char,sizeof(char));
		if(rc!=1){
			perror(strerror(errno));
			close(sockfd);
			exit(1);
		}
	}
	// finished writing to server
	rc = shutdown(sockfd,SHUT_WR);
	if(rc<0){
		perror(strerror(errno));
		close(sockfd);
		exit(1);
	}
	// read number of printable from server
	rc = read(sockfd,&result,sizeof(unsigned int));
	if(rc<0){
		perror(strerror(errno));
		close(sockfd);
		exit(1);
	}
	close(sockfd);
	printf("# of printable characters: %u\n", result);
	exit(0);
}

