#include "server.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_QUEUE 100
typedef struct{
	char word[BUF_SIZE][BUF_SIZE];
	char result[BUF_SIZE][BUF_SIZE];
	char basePath[BUF_SIZE];
	int query_count;
	int line_num;
	int found;
	int state;
	int t_state_number;
}request;

request requestQueue[MAX_QUEUE];
int front = -1, rear = -1;
bool flag = 0; //改良原本只能存放N-1個元素，加上此布林變數即可存放N個元素

int isFull(){
	return (rear % MAX_QUEUE == front);
}

int isEmpty(){
	return front == rear;
}

void Add(request* queue, request item){
	if ((isFull() && flag == 1)|| (rear == MAX_QUEUE - 1 && front == -1)) {
		printf("Circular Queue is full!\n");
		return;
	}
	rear = (rear + 1) % MAX_QUEUE;
	queue[rear] = item;
	if (front == rear) flag = 1;
}

void Delete(request* queue){
	if (isEmpty() && flag == 0){
		printf("Circular Queue is empty!\n");
		return;
	}
	front = (front + 1) % MAX_QUEUE;
	if (front == rear) flag = 0;
}


int t_count=0;

int countStr(char *fpath,int num,void *arg){

	int count=0,len=0,x=0;
	FILE *fp = NULL;
	char *fileContent;
	char *test;
	request *req=(request *)arg;


	fp = fopen(fpath,"r");
	if(fp==NULL){
		printf("error\n");
	}

	fseek(fp,0L,SEEK_END);
	len=ftell(fp);
	rewind(fp);

	fileContent=(char*)malloc((len+1)*sizeof(char));
	memset(fileContent,0,sizeof(char)*(len+1));



	x=fread(fileContent,sizeof(char),len,fp);
	fclose(fp);
	x++;

	
	test = strstr(fileContent, req->word[num]);
	while(test){
		count++;
		test = strstr(test+1, req->word[num]);
	}

	free(fileContent);
	/*while(result[a][0]){a++;};*/
	if (count != 0){

		sprintf(req->result[(req->line_num)++],"File: %s Count: %d\n",fpath,count);
		/*printf("File: %s Count: %d\n",fpath,count);*/
		return 1;
	};
	return 0;
}

void readFileList(char *basePath,int num,void *arg){
    DIR *dir;
    struct dirent *ptr;
    char base[1000];
    char fpath[1000];
	int count=0;
	request *req=(request *)arg;

    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8){    ///file
            memset(fpath,'\0',sizeof(fpath));
            strcpy(fpath,basePath);
            strcat(fpath,"/");
            strcat(fpath,ptr->d_name);
			count+=countStr(fpath,num,req);
		}
        else if(ptr->d_type == 10)    ///link file
            printf("d_name:%s/%s\n",basePath,ptr->d_name);
        else if(ptr->d_type == 4)    ///dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readFileList(base,num,req);
        }
    }
    closedir(dir);
	req->found=count;
}

void *child(void *arg){
	request *req=(request *)arg;

	for(int i=1;i<=req->query_count;i++){

		sprintf(req->result[req->line_num++],"String: \"%s\"\n",req->word[i]);
		readFileList(req->basePath,i,req);
		if(req->found == 0)
			strcpy(req->result[req->line_num++],"Not found\n");
	}
	req->state=2;
	pthread_exit(NULL);
}



/*int main(int argc, char *argv[])*/
int main()
{
	char basePath[BUF_SIZE];
	int query_count=0,x=0,t_num=4;
	pthread_t t[t_num];
	request req;
	int t_state[t_num];


	memset(&t_state,0,sizeof(int));
    //创建套接字
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //将套接字和IP、端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    //进入监听状态，等待用户发起请求
    listen(serv_sock, 20);
    //接收客户端请求

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    x++;


    while(1){
    printf("new socket\n");
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

	memset(&req,0,sizeof(request));
	printf("req memset\n");
	printf("line_num:%d word:%s result[1]:%s\n",req.line_num,req.word[1],req.result[0]);
    x=read(clnt_sock, req.word, sizeof(req.word));

	query_count=0;
	query_count=atoi(req.word[0]);

    // memset(req.result,'\0',sizeof(req.result));
	memset(basePath,'\0',sizeof(basePath));
	strcpy(req.basePath,"./testdir");

	req.query_count=query_count;
	Add(requestQueue,req);

	printf("start query\n");
	if(query_count!=0){
		printf("Query ");
		for(int i=1;i<=query_count;i++){
			printf("\"%s\" ",req.word[i]);
			}
		printf("\n");
		// child(&req);
		requestQueue[rear].state=1;
			
	};

	for(int i=0;i<MAX_QUEUE;i++){
		switch(requestQueue[i].state){
			case 1:
				if (t_count<t_num){
					requestQueue[i].state=0;
					for(int j=0;j<t_num;j++){
						if(t_state[j]==0){
							printf("thread create\n");
							requestQueue[i].t_state_number=j;
							pthread_create(&t[j], NULL, child, (void*) &requestQueue[i]);
							t_state[j]=1;
							t_count++;
							break;

						}
					}
				}

				break;
			case 2:
				printf("write\n");
				requestQueue[i].state=0;
				Delete(requestQueue);
				x=write(clnt_sock, requestQueue[i].result, sizeof(requestQueue[i].result));
				t_state[requestQueue[i].t_state_number]=0;
				t_count--;

				break;
		};
	}


	printf("close\n");
    
    close(clnt_sock);
    // memset(word, 0,sizeof(req.word));

    }
    //关闭套接字
    close(serv_sock);
    return 0;

}
