#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
/*
 * Will store the number of times each printable character was detected.
 * There are 95 such characters.
 */
unsigned int pcc_total [95];

void intHandler(int w) { // SIGINT action handler
	int i=32;
	for(i=32;i<=126;i++){
		printf("char '%c': %u times\n",i,pcc_total[i-32]);
	}
	exit(0);
}

void update_pcc_total(int char_acii){ // will update pcc_total
	pcc_total[char_acii-32]++; //update count
	return;
}

void connection_job (int connfd){ // will handle connection
	char just_read[1];
	unsigned int printable_count=0;
	int w = -1;
	// TODO: don't cancel yet
	while(read(connfd,just_read,1)!=0){
		if(just_read[0]>126 || just_read[0]<32){ // case not printable char
			continue;
		}
		// case found printable char
		update_pcc_total(just_read[0]);
		++printable_count;
	}
	//TODO: can cancel from now
	// stopped reading.
	w = write(connfd,&printable_count,sizeof(unsigned int));
	if(w<0){
		perror(strerror(errno));
		return;
	}
	close(connfd);
}

int main(int argc, char *argv[]){
	struct sockaddr_in serv_addr;
	struct sockaddr_in peer_addr;
	socklen_t addrsize = sizeof(struct sockaddr_in );
	unsigned int server_port = atoi(argv[1]);
	int listenfd  = -1;
	int connfd    = -1;
	int rc=0;


	// dealing with SIGINT
	struct sigaction act;
	act.sa_handler = intHandler;
	sigaction(SIGINT, &act, NULL);

	// Initialize pcc_total
	for(rc=0;rc<95;rc++){
		pcc_total[rc]=0;
	}

	// establishing connection
	listenfd = socket( AF_INET, SOCK_STREAM, 0 );
	memset( &serv_addr, 0, addrsize );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(server_port);
	if( 0 != bind( listenfd,(struct sockaddr*) &serv_addr,addrsize)){
		perror(strerror(errno));
		exit(1);
	}
	if( 0 != listen( listenfd, 10)){
		perror(strerror(errno));
		exit(1);
	}

	//connection established
	while(1){ // loop will accept and handle connections until SIGINT
		connfd = accept(listenfd,(struct sockaddr*) &peer_addr, &addrsize);
		if(connfd<0){
			perror(strerror(errno));
			exit(1);
		}
		connection_job(connfd);
	}
	exit(1);
}
