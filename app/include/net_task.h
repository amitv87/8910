#ifndef _NET_TASK_H_
#define _NET_TASK_H_

#define single_sim_card 0

int  launch_tcp_connection();
void tcp_connection();
void registered_network();
void net_task(void *param);
void netrecv_task(void *param);
void parameter_check();
void update_version();
#endif /* _NET_TASK_H_ */