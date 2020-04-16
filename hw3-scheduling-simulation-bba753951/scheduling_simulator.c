#include "resource.h"
#include "task.h"
#include "priorityq.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <string.h>
#include "task_set.h"
#include "typedefine.h"
#include "scheduling_simulator.h"
#include <unistd.h>


struct priority_q *q=NULL;
struct priority_q *auto_q=NULL;
task_state_type task_state[TASKS_COUNT];
task_type resource_state[RESOURCES_COUNT]= {0};
task_priority_type o_priority[TASKS_COUNT]= {0};


#define handle_error(msg) \
	do { perror(msg); exit(1); } while (0)

int findindex(task_type id)
{
    for(int i=0; i<TASKS_COUNT; i++)
    {
        if(task_const[i].id==id)
        {
            return i;
        }
    }
    return -1;
};

int main()
{

    int i = 0;
    int id=0;



    for (i = 0; i < TASKS_COUNT ; i++)
    {
        task_state[i]=SUSPENDED;
        o_priority[i]=task_const[i].static_priority;
    }


    struct priority_q_ele_ops int_ops =
    {
        .is_larger = int_is_larger,
        .assign = int_assign
    };

    task_const_type min =
    {
        TASK_idle_task,
        idle_task,
        100,
    };
    // printf("create\n");
    q = create_priority_q(100, sizeof(task_const_type), (void *)&min, &int_ops);
    // auto_q = create_priority_q(100, sizeof(task_const_type), (void *)&min, &int_ops);

    if (!q)
    {
        fprintf(stderr, "create q fail.\n");
    }
    // if (!auto_q) {
    //     fprintf(stderr, "create auto_q fail.\n");
    // }
	printf("\n");


    for(i=0; i<AUTO_START_TASKS_COUNT; i++)
    {
        // printf("%d--%u\n",i,auto_start_tasks_list[i]);
        id=findindex(auto_start_tasks_list[i]);
        // printf("find id:%d\n",id);
        priority_q_insert(q, (void *)&task_const[id]);
        task_state[id]=READY;

    }


    task_type t=0;
//    task_const_type val={
    //     TASK_idle_task,
    //     idle_task,
    //     0,
    // };




    // int a = auto_q->cur_nr;
    // for (int j = 0; j < a ; j++){
    // 	printf("remain:%d\n",auto_q->cur_nr);

//    	priority_q_del_min(auto_q, &val);
//    	printf("terminate id:%u\n",val.id);

//    }



    t=priority_q_front(q);

    while(1)
    {
        t=priority_q_front(q);
		 printf("q remain:%d\n",q->cur_nr);
        // printf("---------------------start %u---------------------\n",t);
        activate_task(t);
    }





    //     t=priority_q_Nth(q,1);

	 printf("---------------------continue ---------------------\n");

    // 		activate_task(t);


    // printf("---------------------main ---------------------\n");

    // for(i=0;i<TASKS_COUNT;i++){
    //   printf("task state:%u\n",task_state[i]);
    // }


    //         printf("q remain:%d\n",q->cur_nr);
    //     t=priority_q_Nth(q,1);

    // printf("---------------------continue %u---------------------\n",t);
    //             activate_task(t);



    // printf("---------------------main2 ---------------------\n");

    // for(i=0;i<TASKS_COUNT;i++){
    //   printf("task state:%u\n",task_state[i]);
    // }
    //         printf("q remain:%d\n",q->cur_nr);




    //                   t=priority_q_Nth(q,1);

    //   printf("---------------------continue %u---------------------\n",t);
    //               activate_task(t);


    //           printf("q remain:%d\n",q->cur_nr);





    // activate_task(task_const[1].id);


// printf("------------------------------------------0\n");
//     priority_q_insert(q, (void *)&task_const[0]);
// printf("------------------------------------------1\n");
//     priority_q_insert(q, (void *)&task_const[1]);
//     ((task_const_type *)(q->elements + sizeof(task_const_type)))->static_priority=resources_priority[0];
// printf("------------------------------------------2\n");

//     priority_q_insert(q, (void *)&task_const[2]);
// printf("------------------------------------------3\n");
//     priority_q_insert(q, (void *)&task_const[3]);
// printf("------------------------------------------4\n");
//     priority_q_insert(q, (void *)&task_const[4]);
// printf("------------------------------------------5\n");
//     priority_q_insert(q, (void *)&task_const[5]);



// t=priority_q_Nth(q,1);
// printf("1:%u\n",t);
// t=priority_q_Nth(q,2);
// printf("2:%u\n",t);
// t=priority_q_Nth(q,3);
// printf("3:%u\n",t);
// t=priority_q_Nth(q,4);
// printf("4:%u\n",t);
// t=priority_q_Nth(q,5);
// printf("5:%u\n",t);
// t=priority_q_Nth(q,6);
// printf("6:%u\n",t);



// int a = q->cur_nr;

//     for (int j = 0; j < a ; j++){
// 		printf("remain:%d\n",q->cur_nr);

//     	priority_q_del_min(q, &val);
//     	printf("terminate id:%u\n",val.id);

//     }





    // activate_task(task_const[1].id);




    //   while(q->cur_nr>0){
    //   	printf("remain:%d\n",q->cur_nr);
    //   	 	t=priority_q_front(q);
    // printf("id:%u\n",t);
    // printf("------------------------------------------\n");
    //   	activate_task(t);
    //   }
    //   printf("\n");

    // printf("del done\n");





    return 0;
}
