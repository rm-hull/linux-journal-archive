#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

int main()
{
	int sock;
	int connid;
	int i;
	struct sockaddr_in ad;

	sock = socket(PF_INET, SOCK_DGRAM,0);

	memset(&ad,0,sizeof(ad));
	ad.sin_family = PF_INET;
	ad.sin_port = htons(8000);
	ad.sin_addr.s_addr = inet_addr("142.133.113.21");

	connid = connect(sock,(struct sockaddr *)&ad,sizeof(ad));

	for(i=0;i<500000;i++)
	{
		send(sock,"Hello Server",12,0);
	}

	return 0;
}



