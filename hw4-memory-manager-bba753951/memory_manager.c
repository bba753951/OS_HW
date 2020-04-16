#include<stdio.h>
#include<string.h>
#include <stdlib.h>

typedef struct
{
    int wr;
    int vpi;
} Info;

void fifo(const int M,const int N,int in[],int count)
{
    int input=0,evicted=-1,des=-1,source=-1;
    int pt[M][3],pfi[N],disk[M];       //pt:pfi,present,disk
    char HM[10] = {0};
    int index=0,hit=0,miss=0;       //index 目前用到pfi哪個

    memset(pt,-1,sizeof(pt));
    memset(pfi,-1,sizeof(pfi));
    memset(disk,-1,sizeof(disk));
    for(int i=0; i<M; i++)
    {
        pt[i][1]=0;
    }
    for(int x=0; x<count; x++)
    {
        input=in[x];
        evicted=-1;
        des=-1;
        source=-1;
        if(pt[input][1])
        {
            strcpy(HM,"Hit");
            printf("%s, %d=>%d\n",HM,input,pt[input][0]);
            hit++;
        }
        else
        {
            miss++;
            strcpy(HM,"Miss");
            if(pfi[index]!=-1)
            {
                // page out
                pt[pfi[index]][1]=0;
                evicted=pfi[index];
                for(int k=0; k<M; k++)
                {
                    if(disk[k]==-1)
                    {
                        des=k;
                        disk[k]=pfi[index];
                        pt[pfi[index]][2]=k;
                        break;
                    }
                }
                // page in
                if(pt[input][2]!=-1)
                {
                    disk[pt[input][2]]=-1;
                    source=pt[input][2];

                }

            }
            pfi[index]=input;
            pt[input][0]=index;
            pt[input][1]=1;
            index++;
            if(index == N)
            {
                index=0;
            }
            printf("%s, %d, %d>>%d, %d<<%d\n",HM,pt[input][0],evicted,des,input,source);


        }
    }
    printf("Page Fault Rate: %.3f\n",(float)miss/(miss+hit));


}


//-------------------------ECFA-----------------------------



void ECFA(const int M,const int N,Info in[],int count)
{
    int input=0,evicted=-1,des=-1,source=-1,wr=0,btn=0;
    int pt[N][3],disk[M];
    // char HM[10] = {0};
    int index=0,hit=0,miss=0,done=0;       //index 目前用到pfi哪個

    memset(pt,0,sizeof(pt));
    memset(disk,-1,sizeof(disk));
    for(int i=0; i<N; i++)
    {
        pt[i][0]=-1;
    }

    for(int x=0; x<count; x++)
    {
        input=in[x].vpi;
        wr=in[x].wr;
        evicted=-1;
        des=-1;
        source=-1;
        btn=0;
        done=0;
        // index=0;
        for(int i=0; i<N; i++)
        {
            if(pt[i][0]==input)
            {
                printf("Hit, %d=>%d\n",input,i);
                hit++;
                btn=1;
                if(wr)
                {
                    pt[i][1]=1;
                    pt[i][2]=1;
                }
                else
                {
                    pt[i][1]=1;
                    pt[i][2]=0;

                }
                break;
            }
        }
        if(btn)
            continue;

        miss++;
        while(!done)
        {
            for(int i=0; i<N; i++)
            {
                if(index==N)
                    index=0;
                evicted=pt[index][0];
                if(pt[index][1]==0 && pt[index][2]==0)
                {
                    done=1;
                    if(evicted!=-1)
                    {
                        for(int k=0; k<M; k++)
                        {
                            if(disk[k]==-1)
                            {
                                des=k;
                                disk[k]=evicted;
                                break;
                            }
                        }

                    }
                    for(int k=0; k<M; k++)
                    {
                        if(disk[k]==input)
                        {
                            source=k;
                            disk[k]=-1;
                            break;
                        }
                    }
                    if(wr)
                    {
                        pt[index][1]=1;
                        pt[index][2]=1;
                    }
                    else
                    {
                        pt[index][1]=1;
                        pt[index][2]=0;

                    }
                    pt[index][0]=input;
                    index++;


                    break;
                }
                index++;
            }
            if(done)
                break;
            for(int i=0; i<N; i++)
            {
                if(index==N)
                    index=0;
                if(pt[index][1]==0 && pt[index][2]==1)
                {
                    done=1;
                    evicted=pt[index][0];
                    if(evicted!=-1)
                    {
                        for(int k=0; k<M; k++)
                        {
                            if(disk[k]==-1)
                            {
                                des=k;
                                disk[k]=evicted;
                                break;
                            }
                        }

                    }
                    for(int k=0; k<M; k++)
                    {
                        if(disk[k]==input)
                        {
                            source=k;
                            disk[k]=-1;
                            break;
                        }
                    }
                    if(wr)
                    {
                        pt[index][1]=1;
                        pt[index][2]=1;
                    }
                    else
                    {
                        pt[index][1]=1;
                        pt[index][2]=0;
                    }
                    pt[index][0]=input;
                    index++;

                    break;
                }
                else
                {
                    pt[index][1]=0;

                }
                index++;
            }

        }
        printf("Miss, %d, %d>>%d, %d<<%d\n",index-1,evicted,des,input,source);


    }
    printf("Page Fault Rate: %.3f\n",(float)miss/(miss+hit));


}


// -----------------------------SLRU-------------------------------
struct CacheListNode
{
    int vpi;
    int ref;
    int pfi;
    struct CacheListNode* next;
    struct CacheListNode* prev;
};
struct CacheList
{
    int maxsize;
    int size;
    struct CacheListNode* head;
    struct CacheListNode* tail;
};


void lruCacheInit(struct CacheList* list,int capacity)
{
    list->tail=list->head=NULL;
    list->maxsize=capacity;
    list->size=0;

}


void add(struct CacheList* list,int vpi,int ref,int pfi)
{
    if (list->size+1 > list->maxsize)
    {
        printf("list full\n");
        return;
    }

    list->size++;
    struct CacheListNode* temp=(struct CacheListNode*)malloc(sizeof(struct CacheListNode));
    temp->vpi=vpi;
    temp->ref=ref;
    temp->pfi=pfi;
    temp->next=NULL;
    temp->prev=NULL;
    if(list->head == NULL)
    {
        list->head=temp;
        list->tail=temp;
    }
    else
    {
        temp->next=list->head;
        list->head->prev=temp;
        list->head=temp;
    }


    return;
}

void deletenode(struct CacheList* list,struct CacheListNode* temp,int free_not)
{
    if (!list->size)
    {
        printf("list already empty\n");
        return;
    }
    list->size--;
    if(temp==list->tail)
    {
        list->tail=temp->prev;
        if(list->tail)
        {
            list->tail->next=NULL;
        }
        else
        {
            list->head=NULL;
        }
    }
    else if(temp==list->head)
    {
        list->head=temp->next;
        if(list->head)
        {
            list->head->prev=NULL;
        }
        else
        {
            list->tail=NULL;
        }

    }
    else
    {
        temp->prev->next=temp->next;
        temp->next->prev=temp->prev;
    }
    if(free_not)
    {
        free (temp);
    }


    return;
}

struct CacheListNode* find(struct CacheList* list,int vpi)
{
    struct CacheListNode* temp=list->head;

    while(temp)
    {
        if(temp->vpi==vpi)
        {
            return temp;
        }

        temp=temp->next;
    }
    return temp;
}

struct CacheListNode* find_tail_zero(struct CacheList* list)
{
    struct CacheListNode* temp=list->tail;
    int btn=0;

    while(temp)
    {
        if(temp->ref)
        {
            // if ref==1,set to 0,move to head
            temp->ref=0;
            deletenode(list,temp,0);
            // add to head
            add(list,temp->vpi,temp->ref,temp->pfi);

        }
        else
        {
            btn=1;
            break;
        }

        temp=temp->prev;
    }
    if(!btn)
    {
        // 代表找了一圈大家reference都是1,return tail
        temp=list->tail;

    }
    return temp;
}
int insert_disk(int * disk,int size,int insert)
{
    int find=-1;
    for(int i=0; i<size; i++)
    {
        if(disk[i]==-1)
        {
            disk[i]=insert;
            find=i;
            break;
        }
    }
    return find;
}
int search_disk(int * disk,int size,int search)
{
    int find=-1;

    for(int i=0; i<size; i++)
    {
        if(disk[i]==search)
        {
            disk[i]=-1;
            find=i;
            break;
        }
    }
    return find;
}
void SLRU(const int M,const int N,int in[],int count)
{
    struct CacheList active;
    struct CacheList inactive;
    // const int M=7,N=5;
    int input_vpi=0,pfi_index=0,temp_vpi=0,temp_pfi=0;
    int disk[M];
    int hit=0,miss=0;
    int evicted=-1,des=-1,source=-1;
    struct CacheListNode* find_i;
    struct CacheListNode* find_a;
    struct CacheListNode* find_a_a;
    struct CacheListNode* find_i_i;
    lruCacheInit(&active,N/2);
    lruCacheInit(&inactive,N-N/2);

    memset(disk,-1,sizeof(disk));


    for(int i=0; i<count; i++)
    {
        input_vpi=in[i];
        find_i=find(&inactive,input_vpi);
        find_a=find(&active,input_vpi);
        evicted=-1;
        des=-1;
        source=-1;
        if(find_i || find_a)
        {
            //page hit
            hit++;
            if(find_i)
            {
                printf("Hit, %d=>%d\n",input_vpi,find_i->pfi);

                if(find_i->ref)
                {
                    if(active.size < active.maxsize)
                    {
                        //active not full,move from inactive to active,and set reference = 0
                        add(&active,find_i->vpi,0,find_i->pfi);
                        deletenode(&inactive,find_i,1);
                    }
                    else
                    {
                        find_a_a=find_tail_zero(&active);
                        temp_vpi=find_i->vpi;
                        temp_pfi=find_i->pfi;
                        if(find_a_a)
                        {
                            deletenode(&inactive,find_i,1);
                            add(&inactive,find_a_a->vpi,0,find_a_a->pfi);
                            deletenode(&active,find_a_a,1);
                            add(&active,temp_vpi,0,temp_pfi);

                        }

                    }

                }
                else
                {
                    find_i->ref=1;
                }


            }
            if(find_a)
            {
                printf("Hit, %d=>%d\n",input_vpi,find_a->pfi);

                if(!find_a->ref)
                {
                    find_a->ref=1;
                }
            }

        }
        else
        {
            miss++;
            //page fault




            //pfi not full,insert directly
            if(pfi_index<N && inactive.size < inactive.maxsize)
            {
                //search disk to page in
                source=search_disk(disk,M,input_vpi);

                // not full ,inactive insert directly
                add(&inactive,input_vpi,1,pfi_index);
                printf("Miss, %d, %d>>%d, %d<<%d\n",pfi_index,evicted,des,input_vpi,source);

                pfi_index++;


            }
            else
            {
                //select evicted page
                find_i=find_tail_zero(&inactive);
                //page out to disk
                evicted=find_i->vpi;
                des=insert_disk(disk,M,find_i->vpi);
                // remove from inactive
                temp_pfi=find_i->pfi;
                deletenode(&inactive,find_i,1);



                //search disk to page in
                source=search_disk(disk,M,input_vpi);
                add(&inactive,input_vpi,1,temp_pfi);
                printf("Miss, %d, %d>>%d, %d<<%d\n",temp_pfi,evicted,des,input_vpi,source);

            }

        }
    }
    printf("Page Fault Rate: %.3f\n",(float)miss/(miss+hit));


}

int main()
{
    int M=0,N=0,vpi=0,count=0;
    // int in[5]={2,0,1,2,2};
    int input[1000]= {0};
    Info info[1000];
    FILE *fp = NULL;
    char policy[10],temp[100];

    // fp = fopen("ESCA_input1.txt", "r");
    fp = stdin;
    fscanf(fp, "Policy: %s\n", policy);
    fscanf(fp, "%[^0-9] %d\n", temp,&M);
    fscanf(fp, "%[^0-9] %d\n", temp,&N);
    fscanf(fp, "%[^\n]\n", temp);
    while(!feof(fp))
    {
        fscanf(fp, "%[^0-9] %d\n", temp,&vpi);
        if(!strcmp(temp,"Write "))
            info[count].wr=1;
        if(!strcmp(temp,"Read "))
            info[count].wr=0;


        info[count].vpi=vpi;
        input[count]=vpi;
        count++;
    }
    fclose(fp);
    if(!strcmp(policy,"FIFO"))
    {
        fifo(M,N,input,count);
    }
    else if(!strcmp(policy,"ESCA"))
    {
        ECFA(M,N,info,count);
    }
    else if(!strcmp(policy,"SLRU"))
    {
        SLRU(M,N,input,count);
    }
    else
    {
        printf("Wrong policy\n");
    }


}