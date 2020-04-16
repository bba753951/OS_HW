#include "client.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


/*int main(int argc, char *argv[])*/
int main(int argc, char *argv[])
{

    char key[3]= {0};
    char host[50]= {0};
    int port=0;
    char val[10][100];
    int j=0,k=0,arg_count=0;
    memset(val,0,sizeof(val));
    for(int i=1; i<argc; i++)
    {
        char *arg = argv[i];

        switch(arg[0])
        {
        case '-':
            key[j++]=arg[1];
            arg_count++;
            break;
        default:
            strcpy(val[k++],arg);
        }
    }
    /*check the number whether euqal or not*/
    /*if (sizeof(val)/sizeof(val[0])== sizeof(key)/sizeof(char))*/
    for(int i=0; i<arg_count; i++)
    {

        switch(key[i])
        {
        case 'h':
            strcpy(host,val[i]);
            break;
        case 'p':
            port=atoi(val[i]);
            break;
        default:
            printf("error\n");
        }

    }



    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(host);  //具体的IP地址
    serv_addr.sin_port = htons(port);  //端口



    char buffer_recv[BUF_SIZE][BUF_SIZE];
    char query[BUF_SIZE][BUF_SIZE];
    int query_count=0,format_error=0,start=0;
    char c[4]= {0};
    int x=0,m_count=0;
    char m_count_str[5]= {0};
    /*char c2[1]={0};*/
    /*char c3[1]={0};*/


    while(1)
    {
        //创建套接字
        query_count=1;
        format_error=0;
        start=0;
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

        memset(query,'\0',sizeof(query));
        x=scanf("Query");
        x++;        //for no mean
        /*printf("start\n");*/
        // if(x ==0){
        // 	printf("The strings format is not corret\n");
        // 	format_error=1;
        // }

        while(1)
        {
            memset(query[query_count],0,sizeof(query[query_count]));
            x=scanf("%c",c);
            if(c[0]=='\n') break;
            if(c[0] !=' ')
            {
                format_error=1;
                break;
            }
            x=scanf("%c",(c+1));
            if(c[1] !='"')
            {
                format_error=1;
                break;
            }
            x=scanf("%[^\"\n]",query[query_count]);
            if(query[query_count][0]== 0)
            {
                format_error=1;
                break;
            }
            // printf("qqq:%s\n",query[query_count]);
            x=scanf("%c",(c+2));
            /*scanf("%c%[^\"]%[\"]%c",(c+1),query[query_count],(c+2),c);*/
            /*if(query[query_count][0]!='"' || query[query_count][len-1] != '"'){ */
            if(c[2]!='"')
            {
                format_error=1;
                break;
            };
            // printf(" ");
            query_count++;
        }
        // printf("aaa");




        sprintf(query[0],"%d",query_count-1);

        if(format_error ==1 )
        {

            /*clear stdin*/
            /*scanf("%*[^\n]%*c");*/
            sprintf(query[0],"%d",-1);
            printf("The strings format is not corret\n");
            x=write(sock, query, sizeof(query));
            fflush(stdin);
            memset(query,0,sizeof(query));
            continue;
        }
        /*use query[0] to record num*/
        m_count=0;
        x=write(sock, query, sizeof(query));
        memset(m_count_str,0,sizeof(m_count_str));
        x=read(sock,m_count_str,sizeof(m_count_str));
        // printf("recv count:%s\n", m_count_str);
        m_count=atoi(m_count_str);

        while(m_count--)
        {

            memset(buffer_recv,'\0',sizeof(buffer_recv));
            x=read(sock,buffer_recv,sizeof(buffer_recv));
            start=0;
            // printf("\n");


            while(buffer_recv[start][0])
            {
                printf("%s",buffer_recv[start]);
                start++;
            };

        }





        //关闭套接字
        close(sock);
    }
    return 0;

}
