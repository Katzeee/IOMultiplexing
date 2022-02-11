#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/fcntl.h>

int initserver(int port);

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("usage:./tcpselect port\n");
		return -1;
	}
	//initialize listen socket
	int listensock = initserver(atoi(argv[1]));
	printf("listensock=%d\n", listensock);
	if (listensock < 0)
	{
		printf("initserver() failed.\n");
		return -1;
	}
	fd_set readfdset; //bitmap of sockets including listen and client sockets
	int maxfd; //the max number of sockets
	//initialize the bitmap and add listen socket in
	FD_ZERO(&readfdset);
	FD_SET(listensock, &readfdset);
	maxfd = listensock;

	while(1)
	{
		fd_set tmpfdset = readfdset;
		int infds = select(maxfd+1, &tmpfdset, NULL, NULL, NULL);
		if(infds < 0) //select error
		{
			printf("select() failed.\n");
			perror("select()");
			break;
		}

		if(infds == 0) //select timeout
		{
			printf("select() timeout.\n");
			continue;
		}
		for(int eventfd = 0 ; eventfd <= maxfd ; eventfd++)
		{
			if(FD_ISSET(eventfd, &tmpfdset)<=0)
				continue;
			if(eventfd==listensock) //new client connection
			{
				struct sockaddr_in client;
				socklen_t len = sizeof(client);
				int clientsock = accept(listensock, (struct sockaddr*)&client, &len);
				if(clientsock < 0)
				{
					printf("accept() failed.\n");
					continue;
				}
				printf("client(socket=%d) connected ok.\n", clientsock);
				continue;
			}
			else //data from clients or disconnection of clients
			{
				char buffer[1024];
				memset(buffer, 0, sizeof(buffer));
				
				//read the data from clients
				ssize_t isize=read(eventfd, buffer, sizeof(buffer));

				//an error occurs or socket is closed by itself
				if(isize<=0)
				{
					printf("client(eventfd=%d) disconnected.\n", eventfd);
					close(eventfd); //close client's connection
					FD_CLR(eventfd, &readfdset); //remove it from bitmap
					if(eventfd == maxfd) //recaculate the maxfd
					{
						for(int ii=maxfd; ii>0;ii--)
						{
							if(FD_ISSET(ii, &readfdset))
							{
								maxfd=ii;
								break;
							}
						}

						printf("maxfd=%d.\n", maxfd);
					}
					continue;
				}
				printf("recv(eventfd=%d, size=%ld):%s\n", eventfd, isize, buffer);
				write(eventfd, buffer, strlen(buffer)); //send the messege recieved back to the client
				
			}
		}

	}
	return 0;
}

int initserver(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		printf("socket() failed.\n");
		return -1;
	}

	//linux port in use
	//if you run this program twice in a very short interval, the system maybe warn you "port in use"
	//so you can address this by following codes
	int opt = 1;
	unsigned int len = sizeof(opt);
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, len);
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &opt, len);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	
	if(bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("bind() failed.\n");
		close(sock);
		return -1;
	}

	if(listen(sock, 5)!=0)
	{
		printf("listen() failed.\n");
		close(sock);
		return -1;
	}
	return sock;
}
