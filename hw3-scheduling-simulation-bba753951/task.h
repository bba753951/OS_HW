#ifndef TASK_H
#define TASK_H

#include "typedefine.h"
#include "config.h"

status_type activate_task(task_type id);
status_type terminate_task(void);
extern struct priority_q *q;
extern task_state_type task_state[];
extern task_type resource_state[];

#endif /* TASK_H */