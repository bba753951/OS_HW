#include "resource.h"
#include <stdio.h>
#include <stdlib.h>
// #include "scheduling_simulator.h"
#include "priorityq.h"
#include <ucontext.h>
#include "config.h"
#include "task_set.h"
#include "task.h"


task_priority_type cur[TASKS_COUNT]= {0};
// int btn[TASKS_COUNT]={0};

task_const_type val1=
{
    TASK_idle_task,
    idle_task,
    0,
};
task_const_type val2=
{
    TASK_idle_task,
    idle_task,
    0,
};

status_type get_resource(resource_type id)
{
    status_type result = STATUS_OK;
    task_type t=0;
    t=priority_q_front(q);


    for (int i = 0; i < RESOURCES_COUNT ; i++)
    {
        // printf("-----get resource: %u t:%u\n",resource_state[i],t);

        if(resources_id[i] == id)
        {
            if(resource_state[i]!=0)
            {
                result = STATUS_ERROR;
                break;
            }

            resource_state[i]=t;
            // printf("-----get resource2: %u t:%u\n",resource_state[i],t);


            cur[i]=((task_const_type *)(q->elements + sizeof(task_const_type)))->static_priority;
            if(cur[i] < resources_priority[i])
            {
                ((task_const_type *)(q->elements + sizeof(task_const_type)))->static_priority=resources_priority[i];
            }
        };
    }

    return result;
}

status_type release_resource(resource_type id)
{
    status_type result = STATUS_OK;
    task_type t=0,t1=0,t2=0;
    task_priority_type tp1=0;
    int j=0;
    t=priority_q_front(q);
    // printf("resource remain:%u\n",q->cur_nr);
    // printf("********************\n");
    int max_index=-1;
//     if(q->cur_nr>4){

// t=priority_q_Nth(q,1);
// printf("1:%u\n",t);
// t=priority_q_Nth(q,2);
// printf("2:%u\n",t);
// t=priority_q_Nth(q,3);
// printf("3:%u\n",t);
// t=priority_q_Nth(q,4);
// printf("4:%u\n",t);
// exit(1);
//     }


// int a = q->cur_nr;

//     for (int j = 0; j < a ; j++){
//      printf("remain:%d\n",q->cur_nr);

//      priority_q_del_min(q, &val1);
//      printf("terminate id:%u\n",val1.id);

//     }
    // printf("---------------------resource1 ---------------------\n");

    // for(int i=0;i<TASKS_COUNT;i++){
    //   printf("task state:%u\n",task_state[i]);
    // }


    for (int i = 0; i < RESOURCES_COUNT ; i++)
    {
        // printf("-------***--------\n");
        // printf("resource state: %u t:%u\n",resource_state[i],t);

        if(resources_id[i] == id)
        {
            if(resource_state[i]!=t)
            {
                // printf("release error\n");
                return STATUS_ERROR;
            }
            resource_state[i]=0;
            for (int k = 0; k < RESOURCES_COUNT ; k++)
            {
                if(resource_state[k]==t && k > max_index)
                {
                    max_index=k;
                }
            }
            // printf("max_index:%d\n",max_index);


            if(max_index==-1)
            {
                for(j=0; j<TASKS_COUNT; j++)
                {
                    if(task_const[j].id==t)
                    {
                        break;
                    }
                }
                ((task_const_type *)(q->elements + sizeof(task_const_type)))->static_priority=o_priority[j];
            }
            else
            {
                ((task_const_type *)(q->elements + sizeof(task_const_type)))->static_priority=resources_priority[max_index];
            }
            // printf("rel p:%u\n",((task_const_type *)(q->elements + sizeof(task_const_type)))->static_priority);
            // release後可能被preempt

            priority_q_del_min(q, &val1);
            t2=val1.id;
            priority_q_insert(q, &val1);
            t1=priority_q_front(q);
            tp1=priority_q_front_priority(q);
            // printf("---------val1.id:%u\n",val1.id);
            // printf("---------t1:%u\n",t1);




            for(j=0; j<TASKS_COUNT; j++)
            {
                if(task_const[j].id==val1.id)
                {
                    break;

                }
            }
            // printf("---------j:%u\n",j);


            while(t2!=t1)
            {
                if(tp1 == val1.static_priority)
                {
                    priority_q_del_min(q, &val2);
                    priority_q_insert(q, &val2);
                    t1=priority_q_front(q);
                    tp1=priority_q_front_priority(q);

                }
                else
                {
                    task_state[j]=READY;
                    // printf("cur:%u next:%u\n",cur[i],t1);
                    activate_task(t1);
                    t1=priority_q_front(q);
                }
            }
            // printf("---------j:%u\n",j);


            // printf("---------------------resource2 ---------------------\n");

            // for(int i=0;i<TASKS_COUNT;i++){
            //     printf("task state:%u\n",task_state[i]);
            // }
            task_state[j]=RUNNING;








        };
    }
    // printf("---------------------resource3 ---------------------\n");

    // for(int i=0;i<TASKS_COUNT;i++){
    // printf("task state:%u\n",task_state[i]);
    // }


    return result;
}