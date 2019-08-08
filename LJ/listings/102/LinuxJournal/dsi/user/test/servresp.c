#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

int main()
{
	int sock,sockresp;
	int err;
	struct sockaddr_in ad,adresp;
	char buff[20];

	printf("Listen side\n");

	sock = socket(PF_INET, SOCK_DGRAM,0);
	printf("listen socket = %d\n",sock);

	memset(&ad,0,sizeof(ad));
	ad.sin_family = AF_INET;
	ad.sin_port = htons(8000);
	ad.sin_addr.s_addr = inet_addr("142.133.113.28");

	err = bind(sock,(struct sockaddr *)&ad,sizeof(ad));
	printf("bind = %d\n",err);

	memset(&adresp,0,sizeof(adresp));
	adresp.sin_port = htons(8001);
	adresp.sin_family = PF_INET;
	adresp.sin_addr.s_addr = inet_addr("142.133.113.67");
	sockresp = socket(PF_INET,SOCK_DGRAM,0);
	printf("socket resp = %d\n",sockresp);

	while(1)
	{
		err = recv(sock,buff,12,MSG_WAITALL);
		if(err <= 0)
			break;

		sendto(sockresp,"y",2,0,(struct sockaddr *)&adresp,sizeof(adresp));
	}

	return 0;
}
