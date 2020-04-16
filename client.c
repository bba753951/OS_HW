#include "client.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


/*int main(int argc, char *argv[])*/
int main()
{
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口



    char buffer_recv[BUF_SIZE][BUF_SIZE];
	char query[BUF_SIZE][BUF_SIZE];
	int query_count=0,format_error=0,start=0;
	char c[4]={0};
	int x=0;
	/*char c2[1]={0};*/
	/*char c3[1]={0};*/

	
    while(1){
    //创建套接字
	query_count=1;
	format_error=0;
	start=0;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	memset(query,'\0',sizeof(query));
	x=scanf("Query");
	x++;
	/*printf("start\n");*/
	// if(x ==0){ 
	// 	printf("The strings format is not corret\n");
	// 	format_error=1;
	// }

	while(1){
		memset(query[query_count],0,100*sizeof(char));
		x=scanf("%c",c);
		if(c[0]=='\n') break;
		if(c[0] !=' '){ 
			printf("The strings format is not corret\n");
			format_error=1;
			break;
		}
		x=scanf("%c",(c+1));
		if(c[1] !='"'){ 
			printf("The strings format is not corret\n");
			format_error=1;
			break;
		}
		x=scanf("%[^\"\n]",query[query_count]);
		if(query[query_count][0]== 0){ 
			printf("The strings format is not corret\n");
			format_error=1;
			break;
		}
		printf("%s\n",query[query_count]);
		x=scanf("%c",(c+2));
		/*scanf("%c%[^\"]%[\"]%c",(c+1),query[query_count],(c+2),c);*/
		/*if(query[query_count][0]!='"' || query[query_count][len-1] != '"'){ */
		if(c[2]!='"'){ 
			printf("The strings format is not corret\n");
			format_error=1;
			break;
		}
		/*printf("%s\n",query[query_count]);*/
		query_count++;
	}




	sprintf(query[0],"%d",query_count-1);

	if(format_error ==1 ){ 

		/*clear stdin*/
		/*scanf("%*[^\n]%*c");*/
		fflush(stdin);
		memset(query,0,sizeof(query));
		printf("first error\n");
	}
	/*use query[0] to record num*/

	printf("send server\n");
    x=write(sock, query, sizeof(query));


	memset(buffer_recv,'\0',sizeof(buffer_recv));
    x=read(sock,buffer_recv,sizeof(buffer_recv));

	if(format_error ==1 ){ 
		memset(buffer_recv,'\0',sizeof(buffer_recv));
		close(sock);
		continue;
	}

	while(buffer_recv[start][0]){
		printf("%s",buffer_recv[start]);
		start++;
	};

	memset(buffer_recv,'\0',sizeof(buffer_recv));

    //关闭套接字
    close(sock);
    }
    return 0;

}
