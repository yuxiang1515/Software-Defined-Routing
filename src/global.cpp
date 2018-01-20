#include "../include/global.h"

uint16_t CONTROL_PORT = 0, ROUTER_PORT = 0, DATA_PORT = 0;

int control_socket = -1, router_socket = -1, data_socket = -1;

uint16_t routers_num;

uint16_t update_interval;

void set_init(uint16_t a, uint16_t b) {
	routers_num = a;
	update_interval = b;
}

uint16_t get_routers_num() {
	return routers_num;
}

uint16_t get_update_interval() {
	return update_interval;
}
