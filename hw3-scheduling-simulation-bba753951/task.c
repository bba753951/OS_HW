#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include "scheduling_simulator.h"
#include "priorityq.h"
#include <ucontext.h>
#include "config.h"
#include "task_set.h"




#define handle_error(msg) \
	do { perror(msg); exit(1); } while (0)


static ucontext_t uctx_main[TASKS_COUNT];

static ucontext_t uctx_func[TASKS_COUNT];
char func2_stack[TASKS_COUNT][1000];

task_const_type val=
{
    TASK_idle_task,
    idle_task,
    0,
};
status_type activate_task(task_type id)
{
    status_type result = STATUS_OK;
    task_type t=0,t2=0;






    for (int i = 0; i < TASKS_COUNT ; i++)
    {
        if(task_const[i].id == id)
        {
            if(task_state[i]!=SUSPENDED)
            {
                // printf("small errrrrrrrrrrrrrrrrrrrrr\n");

                result = STATUS_ERROR;
                // break;
            }
            if(task_state[i]==SUSPENDED)
            {
                priority_q_insert(q, (void *)&task_const[i]);
            }
            t=priority_q_front(q);

            if(task_state[i]==RUNNING && id ==t)
            {
                // printf("errrrrrrrrrrrrrrrrrrrrr\n");
                return STATUS_ERROR;
            }
            // printf("front:%d\n",t);
            // printf("front priority: %u\n",((task_const_type *)(q->elements + sizeof(task_const_type)))->static_priority);
            if(task_const[i].id==t)
            {
                // printf("activate\n");
                // printf("task_state:%u\n",task_state[i]);

                if (getcontext(&uctx_func[i]) == -1)
                    handle_error("getcontext");

                // 预分配的栈空间
                uctx_func[i].uc_stack.ss_sp = func2_stack[i];
                uctx_func[i].uc_stack.ss_size = sizeof(func2_stack[i]);

                uctx_func[i].uc_link = &uctx_main[i];

                // uctx_func的上下文环境：func2
                makecontext(&uctx_func[i], task_const[i].entry, 0);


                task_state[i]=RUNNING;
                if(q->cur_nr > 1)
                {
                    t2=priority_q_second(q);
                    for (int i = 0; i < TASKS_COUNT ; i++)
                    {
                        if(task_const[i].id==t2)
                        {
                            task_state[i]=READY;
                        };
                    };
                };



                // // swap 跳转uctx_func2
                // printf("amain: swapcontext(&uctx_main[%d], &uctx_func[%d])\n",i,i);
                if (getcontext(&uctx_main[i]) == -1)
                    handle_error("maincontext");
                if (swapcontext(&uctx_main[i], &uctx_func[i]) == -1)
                    handle_error("swapcontext");

                // printf("amain: return\n");


            }
            else
            {
                task_state[i]=READY;
                // printf("task_state:%u\n",task_state[i]);
            }

            // printf("---------------------activate %u---------------------\n",id);

            for(i=0; i<TASKS_COUNT; i++)
            {
                // printf("task state:%u\n",task_state[i]);
            }


            break;
        };
    }
    // printf("activate result:%u\n",result);

    return result;
}

status_type terminate_task(void)
{
    task_type t=0;
    t=priority_q_front(q);
    // printf("terminate front:%d\n",t);


    status_type result = STATUS_OK;

    for (int i = 0; i < RESOURCES_COUNT ; i++)
    {
        if(resource_state[i] == t)
        {
            return STATUS_ERROR;
        }
    }




    // printf("count1: %d\n",q->cur_nr);
    priority_q_del_min(q, &val);
    // printf("pop done\n");


    // if(q->cur_nr > 0){

    //     t=priority_q_front(q);
    //     for (int i = 0; i < TASKS_COUNT ; i++) {
    //         if(task_const[i].id==t){
    //             task_state[i]=RUNNING;
    //         };
    //     };
    // }



    for (int i = 0; i < TASKS_COUNT ; i++)
    {
        // printf("1terminate:%u\n",val.id);
        // printf("2terminate:%u\n",task_const[i].id);
        // printf("1state:%u\n",RUNNING);
        // printf("2state:%u\n",task_state[i]);



        if(task_const[i].id == val.id && task_state[i]==RUNNING)
        {
            // printf("terminate:%d\n",i);
            task_state[i]=SUSPENDED;
            break;

        }
    }
    // printf("count2: %d\n",q->cur_nr);
    // printf("terminate id:%u\n",val.id);





    // printf("terminate result:%u\n",result);

    return result;
}
