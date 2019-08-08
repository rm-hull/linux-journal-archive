#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

int main()
{
	int sock,sockresp;
	int connid;
	int err;
	int i;
	char buff[20];
	struct sockaddr_in ad, adresp;

	sock = socket(PF_INET, SOCK_DGRAM,0);

	sockresp = socket(PF_INET, SOCK_DGRAM,0);

	memset(&ad,0,sizeof(ad));
	ad.sin_family = PF_INET;
	ad.sin_port = htons(8000);

	ad.sin_addr.s_addr = inet_addr("142.133.113.28");

	connid = connect(sock,(struct sockaddr *)&ad,sizeof(ad));

	memset(&adresp,0,sizeof(adresp));
	adresp.sin_family = PF_INET;
	adresp.sin_port = htons(8001);

	adresp.sin_addr.s_addr = inet_addr("142.133.113.67");

	err = bind(sockresp,(struct sockaddr *)&adresp,sizeof(adresp));

	for(i=0;i<100000;i++) {
		err = send(sock,"Hello Server",12,0);
		if(err != 12) {
			printf("send error = %d\n",err);
			break;
		}
 
		err = recv(sockresp,buff,2,0x100);

		if(err <= 0){
			printf("recv err = %d\n",err);
			break;
		}
		buff[1] = 0;
	}

	return(0);
}



