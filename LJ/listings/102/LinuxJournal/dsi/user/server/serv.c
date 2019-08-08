#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main()
{
	int sock;
	int err;
	struct sockaddr_in ad;
	char buff[20];

	printf("Listen side\n");
	sock = socket(PF_INET, SOCK_DGRAM,0);
	printf("listen socket = %d\n",sock);

	memset(&ad,0,sizeof(ad));
	ad.sin_family = AF_INET;
	ad.sin_port = htons(8000);
	//ad.sin_addr.s_addr = inet_addr("127.0.0.1");
	ad.sin_addr.s_addr = inet_addr("142.133.113.67");

	err = bind(sock,(struct sockaddr *)&ad,sizeof(ad));
	printf("bind = %d\n",err);

	while(1)
	{
		err = recv(sock,buff,12,MSG_WAITALL);
		if(err <= 0)
			break;

		buff[12] = 0;
		printf("%s\n",buff);
	}

	return 0;
}


