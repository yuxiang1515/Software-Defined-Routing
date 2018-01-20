#ifndef _ROUTE_SOCK_MANAGER_
#define _ROUTE_SOCK_MANAGER_

bool contains_sock(uint8_t trans_id);

void add_sock(uint8_t trans_id, int sock_index);

int get_sock(uint8_t trans_id);

void remove_sock(uint8_t trans_id);

#endif