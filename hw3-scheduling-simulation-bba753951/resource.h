#ifndef RESOURCE_H
#define RESOURCE_H

#include "typedefine.h"
#include "config.h"

extern struct priority_q *q;
extern task_type resource_state[];
extern task_state_type task_state[];
extern task_priority_type o_priority[];

status_type get_resource(resource_type id);
status_type release_resource(resource_type id);

#endif /* RESOURCE_H */