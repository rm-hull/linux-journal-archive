/* sfl.c sendfile example program
   Dragan Stancevic <visitor@xalien.org> 09-09-2002
 header name                 function / variable
-------------------------------------------------- */
#include <stdio.h>          /* printf, perror */
#include <fcntl.h>          /* open */
#include <unistd.h>         /* close */
#include <errno.h>          /* errno */
#include <string.h>         /* memset */
#include <sys/socket.h>     /* socket */
#include <netinet/in.h>     /* sockaddr_in */
#include <sys/sendfile.h>   /* sendfile */
#include <arpa/inet.h>      /* inet_addr */
#define BUFF_SIZE (10*1024) /* size of the tmp buffer */

int main(int argc, char **argv){
	int sd = 0, fd = 0, is_server = 0; /* socket/file descriptor, Tx or Rx */
	int cnt;                           /* bytes transmitted */
	off_t off = 0;                     /* file offset */
	struct sockaddr_in sa;             /* server address */
	char buffer[BUFF_SIZE];

	if(argc < 3){
	   printf("Usage: sfl [send | receive] address\n");
	   exit(1);
	}

	/* are we sending or receiving */
	if(argv[1][0] == 's') is_server++;

	/* open descriptors */
	sd = socket(PF_INET, SOCK_STREAM, 0);
	if(is_server) fd = open("data.bin", O_RDONLY);

	if(sd < 0 || fd < 0){
		perror("socket/open");
		exit(errno);
	}

	/* clear the memory */
	memset(&sa, 0, sizeof(struct sockaddr_in));

	/* initialize structure */
	sa.sin_family = PF_INET;
	sa.sin_port = htons(1033);
	sa.sin_addr.s_addr = inet_addr(argv[2]);
	
	if(is_server){
		int client; /* new client socket */
		printf("Server binding to [%s]\n", argv[2]);
	
		if(bind(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0){
			perror("bind");
			exit(errno);
		}
		if(listen(sd,1) < 0){
			perror("listen");
			exit(errno);
		}
		if((client = accept(sd, NULL, NULL)) < 0){
			perror("accept");
			exit(errno);
		}
		if ((cnt = sendfile(client,fd,&off,BUFF_SIZE)) < 0){
			perror("sendfile");
			exit(errno);
		}
		printf("Server sent %d bytes.\n", cnt);
		close(client);

	} else {
		int total = 0;
		printf("Client connecting to [%s]\n", argv[2]);

		if(connect(sd, (struct sockaddr*)&sa, sizeof(sa)) < 0){
			perror("errno");
			exit(errno);
		}
		while((cnt = read(sd,&buffer[total],(BUFF_SIZE-total)))) total += cnt;
		printf("Client received %d bytes.\n", total);
	}

	/* close descriptors */
	close(sd);
	if(is_server) close(fd);

return 0;
}
