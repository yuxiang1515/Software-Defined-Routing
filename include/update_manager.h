#ifndef _UPDATE_MANAGER_H_
#define _UPDATE_MANAGER_H_

void init_router_socket();

void send_update(int sock_index);

void receive_update(int sock_index);

#endif