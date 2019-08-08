#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>


int main()
{
	int sock;
	int connid;
	struct sockaddr_in ad;

	sock = socket(PF_INET, SOCK_DGRAM,0);

	printf("socket = %d\n",sock);

	memset(&ad,0,sizeof(ad));
	ad.sin_family = AF_INET;
	ad.sin_port = htons(8000);

	ad.sin_addr.s_addr = inet_addr("142.133.113.28");

//ad.sin_addr.s_addr = inet_addr("127.0.0.1");

	printf("addr = %x\n", ad.sin_addr.s_addr);

	connid = connect(sock,(struct sockaddr*)&ad,sizeof(ad));

	printf("connect = %d\n",connid);

	while(1)
	{
		send(sock,"Hello Server",12,0);
		sleep(1);
	}

	return 0;
}



