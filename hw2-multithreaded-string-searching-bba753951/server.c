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
typedef struct
{
    char word[BUF_SIZE][BUF_SIZE];
    char result[BUF_SIZE][BUF_SIZE];
    char basePath[BUF_SIZE];
    int query_count;
    int line_num;
    int found;
    int state;          // 0 for processing/nothing ,1 for queue ,2 for done
    int t_state_number;
} request;

request requestQueue[MAX_QUEUE];
int front = -1, rear = -1;
bool flag = 0; //改良原本只能存放N-1個元素，加上此布林變數即可存放N個元素

int isFull()
{
    return (rear % MAX_QUEUE == front);
}

int isEmpty()
{
    return front == rear;
}

void Add(request* queue, request item)
{
    if ((isFull() && flag == 1)|| (rear == MAX_QUEUE - 1 && front == -1))
    {
        printf("Circular Queue is full!\n");
        return;
    }
    rear = (rear + 1) % MAX_QUEUE;
    queue[rear] = item;
    if (front == rear) flag = 1;
}

void Delete(request* queue)
{
    if (isEmpty() && flag == 0)
    {
        printf("Circular Queue is empty!\n");
        return;
    }
    front = (front + 1) % MAX_QUEUE;
    if (front == rear) flag = 0;
}


int t_count=0; //presernt thread count

int countStr(char *fpath,int num,void *arg)
{

    int count=0,len=0,x=0;
    FILE *fp = NULL;
    char *fileContent;
    char *test;
    request *req=(request *)arg;


    fp = fopen(fpath,"r");
    if(fp==NULL)
    {
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
    while(test)
    {
        count++;
        test = strstr(test+1, req->word[num]);
    }

    free(fileContent);
    /*while(result[a][0]){a++;};*/
    if (count != 0)
    {

        sprintf(req->result[(req->line_num)++],"File: %s Count: %d\n",fpath,count);
        /*printf("File: %s Count: %d\n",fpath,count);*/
        return 1;
    };
    return 0;
}

void readFileList(char *basePath,int num,void *arg)
{
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
        else if(ptr->d_type == 8)     ///file
        {
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

void *child(void *arg)
{
    request *req=(request *)arg;

    for(int i=1; i<=req->query_count; i++)
    {

        sprintf(req->result[req->line_num++],"String: \"%s\"\n",req->word[i]);
        readFileList(req->basePath,i,req);
        if(req->found == 0)
            strcpy(req->result[req->line_num++],"Not found\n");
    }
    req->state=2;
    pthread_exit(NULL);
}



int main(int argc, char *argv[])
{
    char basePath[BUF_SIZE];
    int query_count=0,x=0,t_num=4;
    pthread_t t[t_num];
    int t_state[t_num];   //thread use or not
    request req;


    char key[3]= {0};
    char root[50]= {0};
    int port=0;
    char val[3][100];
    int j=0,k=0;
    // int start=0;
    int m_count=0;    // send message count
    char m_count_str[5]= {0};

    memset(val,0,sizeof(val));
    for(int i=1; i<argc; i++)
    {
        char *arg = argv[i];

        switch(arg[0])
        {
        case '-':
            key[j++]=arg[1];
            break;
        default:
            strcpy(val[k++],arg);
        }
    }
    /*check the number whether euqal or not*/
    /*if (sizeof(val)/sizeof(val[0])== sizeof(key)/sizeof(char))*/
    for(int i=0; i<3; i++)
    {
        switch(key[i])
        {
        case 'r':
            strcpy(root,val[i]);
            break;
        case 'p':
            port=atoi(val[i]);
            break;
        case 'n':
            t_num=atoi(val[i]);
            break;
        default:
            printf("parameter error");
        }


    }


    memset(t_state,0,sizeof(t_state));
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(port);  //端口
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(serv_sock, 20);

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    x++;


    while(1)
    {
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

        memset(&req,0,sizeof(request));
        x=read(clnt_sock, req.word, sizeof(req.word));

        query_count=0;
        query_count=atoi(req.word[0]);
        // printf("querry count:%d\n",query_count);
        if(query_count==-1)
        {
            close(clnt_sock);
            continue;
        }

        // memset(req.result,'\0',sizeof(req.result));
        memset(basePath,'\0',sizeof(basePath));
        strcpy(req.basePath,root);

        req.query_count=query_count;
        Add(requestQueue,req);

        if(query_count!=0)
        {
            printf("Query ");
            for(int i=1; i<=query_count; i++)
            {
                printf("\"%s\" ",req.word[i]);
            }
            printf("\n");
            requestQueue[rear].state=1;

        };

        for(int i=0; i<MAX_QUEUE; i++)
        {
            if(requestQueue[i].state==1)
            {
                if (t_count<t_num)
                {
                    requestQueue[i].state=0;
                    for(int j=0; j<t_num; j++)
                    {
                        if(t_state[j]==0)
                        {
                            requestQueue[i].t_state_number=j;
                            pthread_create(&t[j], NULL, child, (void*) &requestQueue[i]);
                            t_state[j]=1;
                            t_count++;
                            break;

                        }
                    }
                }
            }
        }
        for(int i=0; i<MAX_QUEUE; i++)
        {
            if(requestQueue[i].state==2)
                m_count++;
        }
        // printf("count:%d\n", m_count);
        memset(m_count_str,0,sizeof(m_count_str));
        sprintf(m_count_str,"%d",m_count);
        x=write(clnt_sock, m_count_str, sizeof(m_count));

        for(int i=0; i<MAX_QUEUE; i++)
        {
            if(requestQueue[i].state==2 && m_count!=0)
            {
                requestQueue[i].state=0;
                // printf("send:\n");
                x=write(clnt_sock, requestQueue[i].result, sizeof(requestQueue[i].result));
                t_state[requestQueue[i].t_state_number]=0;
                // start=0;
                // while(requestQueue[i].result[start][0])
                // {
                //     printf("result:\n");
                //     printf("%s",requestQueue[i].result[start]);
                //     start++;
                // };
                Delete(requestQueue);
                t_count--;
                m_count--;
            };

        };



        close(clnt_sock);

    }
    close(serv_sock);
    return 0;

}
