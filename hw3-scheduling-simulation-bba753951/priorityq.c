#include "priorityq.h"
#include <stdio.h>
#include <stdlib.h>

struct priority_q *create_priority_q(int max_nr, int ele_size, void *min_ele, struct priority_q_ele_ops *pqe_ops)
{
    struct priority_q *q = NULL;

    q = malloc(sizeof(struct priority_q));
    if (!q)
    {
        fprintf(stderr, "in %s: malloc heap failed.\n", __func__);
        goto malloc_heap_fail;
    }

    q->capacity = max_nr;
    q->cur_nr = 0;

    q->ele_size = ele_size;
    // use +1 bcz node 0 is not used;
    q->elements = malloc(ele_size*(max_nr+1));
    if (!q->elements)
    {
        fprintf(stderr, "in %s: malloc elements failed.\n", __func__);
        goto malloc_ele_fail;
    }

    q->pqe_ops = pqe_ops;

    /* set elements[0] as min_ele's value for later efficient testing */
    q->pqe_ops->assign(q->elements, min_ele);

    return q;

malloc_ele_fail:
    free(q);
malloc_heap_fail:
    return NULL;
}

void destroy_priority_q(struct priority_q *q)
{
    free(q->elements);
    free(q);
}

static inline int pq_is_larger(struct priority_q *q, void *lhs, void *rhs)
{
    return q->pqe_ops->is_larger(lhs, rhs);
}

//在pop的時候要使用大於等於才能確保相同的後來在後面
static inline int larger_equal(struct priority_q *q, void *lhs, void *rhs)
{
    task_const_type *L = lhs;
    task_const_type *R = rhs;
    return L->static_priority <= R->static_priority;
}
static inline int is_equal(struct priority_q *q, void *lhs, void *rhs)
{
    task_const_type *L = lhs;
    task_const_type *R = rhs;
    return L->static_priority == R->static_priority;
}
static inline void *pq_ith_ele(struct priority_q *q, int i)
{
    return (q->elements + (q->ele_size*i));
}

static inline void pq_assign(struct priority_q *q, void *lhs, void *rhs)
{
    q->pqe_ops->assign(lhs, rhs);
}

void priority_q_insert(struct priority_q *q, void *e)
{
    int i = 0;

    q->cur_nr += 1;
    // 並沒有真正的做swap，而是用比較的，空一個位置，由下往上比較
    for (i = q->cur_nr ; pq_is_larger(q, pq_ith_ele(q, i/2), e) ; i /= 2)
    {
        if(i>1 && i%2==1 && is_equal(q,pq_ith_ele(q,i/2),pq_ith_ele(q,i-1)))
        {
            pq_assign(q, pq_ith_ele(q, i), pq_ith_ele(q, i-1));
            pq_assign(q, pq_ith_ele(q, i-1), pq_ith_ele(q, i/2));

        }
        else
        {
            pq_assign(q, pq_ith_ele(q, i), pq_ith_ele(q, i/2));
        }
    }
    // 最後再把值放入找到的位置
    pq_assign(q, pq_ith_ele(q, i), e);
}

void priority_q_del_min(struct priority_q *q, void *min_ele)
{
    int i = 0;
    int child = 0;
    void *last_ele = NULL;
    int btn=0;

    //let node 1(minumun node) assign to mun_ele to return
    pq_assign(q, min_ele, pq_ith_ele(q, 1));
    // use -- 是因為把最後一個元素放到第一個，最後一個可以去掉了
    last_ele = pq_ith_ele(q, q->cur_nr--);
    // 利用parent-child性質
    if(q->cur_nr==0)
    {
        printf("empty\n");
        return;
    }
    if(q->cur_nr < 0)
    {
        printf("priority_q already empty\n");
        return;
    }

    // 從上往下比較
    for (i = 1 ; (i*2) <= q->cur_nr; i = child)
    {
        child = i*2;

        // 右邊node大於左邊node就++，代表右邊比較小，比較右邊就好
        if (child != q->cur_nr && pq_is_larger(q, pq_ith_ele(q, child), pq_ith_ele(q, child+1)))
        {
            child++;
            btn=1;
        }
        // 從上到下和最後一個node做比較(不真的把最後一個node和第一個node做swap)
        if (larger_equal(q, last_ele, pq_ith_ele(q, child)))
        {
            pq_assign(q, pq_ith_ele(q, i), pq_ith_ele(q, child));
            if(btn==0)
            {
                pq_assign(q, pq_ith_ele(q, child), pq_ith_ele(q, child+1));
                child++;


            }
        }
        else
        {

            break;
        }
    }
    // 最後在把最後一個node插入
    pq_assign(q, pq_ith_ele(q, i), last_ele);
}

task_type priority_q_front(struct priority_q *q)
{
    return ((task_const_type *)(q->elements + sizeof(task_const_type)))->id;
}
task_type priority_q_second(struct priority_q *q)
{
    return ((task_const_type *)(q->elements + sizeof(task_const_type)*2))->id;
}
task_type priority_q_Nth(struct priority_q *q,int n)
{
    return ((task_const_type *)(q->elements + sizeof(task_const_type)*n))->id;
}


task_priority_type priority_q_front_priority(struct priority_q *q)
{
    return ((task_const_type *)(q->elements + sizeof(task_const_type)))->static_priority;
}


/* test code, use int as element */
int int_is_larger(void *lhs, void *rhs)
{
    task_const_type *L = lhs;
    task_const_type *R = rhs;
    // printf("L:%d\n",L->static_priority);
    // printf("R:%d\n",R->static_priority);
    // printf("a:%d\n",L->static_priority < R->static_priority);

    return L->static_priority < R->static_priority;
}


void int_assign(void *lhs, void *rhs)
{
    task_const_type *L = lhs;
    task_const_type *R = rhs;

    *L = *R;
}


