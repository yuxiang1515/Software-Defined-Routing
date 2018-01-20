#ifndef _ROUTER_MANAGER_H_
#define _ROUTER_MANAGER_H_

void init_router(uint16_t id, uint16_t router_port, uint16_t data_port, uint16_t cost, uint32_t ip_addr);

std::vector<uint16_t> get_neighbors();

std::vector<uint16_t> get_all_routers();

uint16_t get_self_id();

bool is_self(uint16_t id);

uint16_t get_data_port(uint16_t id);

uint16_t get_router_port(uint16_t id);

uint32_t get_ip_addr(uint16_t id);

uint16_t get_cost(uint16_t id);

uint16_t get_total_cost(uint16_t id);

uint16_t find_id_by_ip(uint32_t ip_addr);

void set_cost(uint16_t from_id, uint16_t to_id, uint16_t cost);

void set_nei_cost(uint16_t id, uint16_t cost);

void init_table();

void calculate_table();

void remove_from_net(uint16_t rm_id);

uint16_t get_next_hop(uint16_t id);

#endif