#ifndef PRIORITYQ_H
#define PRIORIRYQ_H
/* ADT for priority queue */
struct priority_q;
 
struct priority_q_ele_ops {
    int (*is_larger)(void *lhs, void *rhs);
    void (*assign)(void *lhs, void *rhs);
};
 
struct priority_q *create_priority_q(int max_nr, int ele_size, void *min_ele, struct priority_q_ele_ops *pqe_ops);
void destroy_priority_q(struct priority_q *q);
void priority_q_insert(struct priority_q *q, void *e);
void priority_q_del_min(struct priority_q *q, void *min_ele);
 
/* we use heap to imp priority queue */
 
struct priority_q {
    int capacity;
    int cur_nr;
    int ele_size;
    void *elements;
    struct priority_q_ele_ops *pqe_ops;
} ;
int int_is_larger(void *lhs, void *rhs);
void int_assign(void *lhs, void *rhs);
#endif
