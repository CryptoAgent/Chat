#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

int clients_fd[100];
int cnt = 0;

void * client_h(void *);

int main(){
	
	int serv_sock = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in serv_addr;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(8080);
	serv_addr.sin_family = AF_INET;

	bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	listen(serv_sock,100);

	while(cnt<100){
	
	int client_sock = accept(serv_sock,NULL,NULL);
	clients_fd[cnt]=client_sock;
	pthread_t id;
	int ret = pthread_create(&id,NULL,&client_h,NULL);
	if(ret!=0){
		printf("Can't create new thread\r\n");
	}

	cnt++;
	}

	return 0;
}

void * client_h(void * arg){
	int our_id = cnt-1;
	int this_client_sock = clients_fd[our_id];
	char buff[1024];
	while(1){
		memset(buff,'\0',1024);
		int stat = recv(this_client_sock,&buff,1024,0);
		if(stat==0||stat==-1){
			clients_fd[our_id]=0;
			pthread_exit(NULL);
		}
		for(int i = 0;i<cnt;i++){
			if(clients_fd[i]==this_client_sock){
				continue;
			}else{
				if(clients_fd[i]!=0){
					send(clients_fd[i],&buff,1024,0);
				}
			}
		}
	}

}