
#include "../include/global.h"
#include <map>

using namespace std;

class Router_Info{
	public:
		uint16_t id;
		uint16_t router_port;
		uint16_t data_port;
		uint16_t cost;
		uint32_t ip_addr;
		bool is_neighbor;
		uint16_t next_hop_id; 
};

map<uint16_t, Router_Info> router_info_map;
map<uint16_t, map<uint16_t, uint16_t> > router_table_map;

uint16_t self_id;

void init_router(uint16_t id, uint16_t router_port, uint16_t data_port, uint16_t cost, uint32_t ip_addr) {
	
	
	Router_Info router_info;
	router_info.id = id;
	router_info.router_port = router_port;
	router_info.data_port = data_port;
	router_info.cost = cost;

	if (cost == 0) {
		ROUTER_PORT = router_port;
		DATA_PORT = data_port;
		self_id = id;
	}

	router_info.ip_addr = ip_addr;
	router_info.is_neighbor = (cost != INF && cost != 0);
	

	router_info_map[id] = router_info;

}

vector<uint16_t> get_all_routers() {
	vector<uint16_t> res;

	map<uint16_t, Router_Info>:: iterator p;
	for (p = router_info_map.begin(); p != router_info_map.end(); p++) {
		res.push_back(p->first);
	}
	return res;
}

vector<uint16_t> get_neighbors() {
	vector<uint16_t> res;

	map<uint16_t, Router_Info>:: iterator p;
	for (p = router_info_map.begin(); p != router_info_map.end(); p++) {
		if (p->second.is_neighbor) res.push_back(p->first);
	}

	return res;
}

uint16_t get_self_id(){
	return self_id;
}

bool is_self(uint16_t id) {
	return id == self_id;
}

uint32_t get_ip_addr(uint16_t id) {
	return router_info_map[id].ip_addr;
}

uint16_t get_data_port(uint16_t id) {
	return router_info_map[id].data_port;
}

uint16_t get_router_port(uint16_t id) {
	return router_info_map[id].router_port;
}

uint16_t get_cost(uint16_t id) {
	return router_info_map[id].cost;
}

uint16_t get_total_cost(uint16_t id) {
	return router_table_map[self_id][id];
}

uint16_t find_id_by_ip(uint32_t ip_addr) {

	map<uint16_t, Router_Info>:: iterator p;
	for (p = router_info_map.begin(); p != router_info_map.end(); p++) {
		
		if (p->second.ip_addr == ip_addr) return p->first;
	}

	return -1;
}

void init_table() {

	vector<uint16_t> id_set = get_all_routers();
	

	for (int a = 0; a < id_set.size(); a++) {
		for (int b = 0; b < id_set.size(); b++) {

			int i = id_set[a];
			int j = id_set[b];

			if (i == self_id) {

				router_table_map[i][j] = router_info_map[j].cost;

				if (j == self_id || router_info_map[j].is_neighbor) {
					router_info_map[j].next_hop_id = j;
				} else {
					router_info_map[j].next_hop_id = INF;
					
				}

			}
			else {
				router_table_map[i][j] = INF;
			}
		}
	}

	
}

void set_cost(uint16_t from_id, uint16_t to_id, uint16_t cost) {

	router_table_map[from_id][to_id] = cost;

}

void set_nei_cost(uint16_t id, uint16_t cost) {
	router_info_map[id].cost = cost;
}

void calculate_table() {


	vector<uint16_t> id_set = get_all_routers();

	for (int a = 0; a < id_set.size(); a++) {

		int j = id_set[a];

		router_table_map[self_id][j] = router_info_map[j].cost;

		if (j == self_id || router_info_map[j].is_neighbor) {

				router_info_map[j].next_hop_id = j;

		} else {
		
			router_info_map[j].next_hop_id = INF;
					
		}

	}

	for (int a = 0; a < id_set.size(); a++) {

		for (int b = 0; b < id_set.size(); b++) {

			uint16_t i = id_set[a];
			uint16_t j = id_set[b];

			if (!router_info_map[j].is_neighbor || router_info_map[j].cost == INF || router_table_map[j][i] == INF) continue;


			if (router_info_map[j].cost+router_table_map[j][i] < router_table_map[self_id][i]) {

				router_table_map[self_id][i] = router_table_map[j][i]+router_info_map[j].cost;
				router_info_map[i].next_hop_id = j;

			}

		}

	}

}



void remove_from_net(uint16_t rm_id) {

	router_info_map[rm_id].is_neighbor = false;
	router_info_map[rm_id].cost = INF;

	router_table_map[self_id][rm_id] = INF;
	router_info_map[rm_id].next_hop_id = INF;

}

uint16_t get_next_hop(uint16_t id){
	return router_info_map[id].next_hop_id;
}
