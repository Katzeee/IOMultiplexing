#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("usage:./tcpclient ip port.\n");
		return -1;
	}
	int sockfd;
	struct sockaddr_in servaddr;
	char buffer[1024];

	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket() failed.\n");
		return -1;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);

	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("connect(%s:%s) failed.\n", argv[1], argv[2]);
		close(sockfd);
		return -1;
	}

	printf("connect ok.\n");

	for(int ii=0; ii<10000; ii++)
	{
		memset(buffer, 0, sizeof(buffer));
		printf("please input:");
		scanf("%s", buffer);

		if(strcmp(buffer, "quit") == 0)
		{
			return 0;
		}
		if(write(sockfd, buffer, strlen(buffer)) <= 0)
		{
			printf("write() failed.\n");
			close(sockfd);
			return -1;
		}
		memset(buffer, 0, sizeof(buffer));
		if(read(sockfd, buffer, sizeof(buffer)) <= 0)
		{
			printf("read() failed.\n");
			close(sockfd);
			return -1;
		}
		printf("recv:%s\n", buffer);

	}
}
