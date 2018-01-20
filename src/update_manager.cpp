#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include "../include/global.h"
#include "../include/router_manager.h"
#include "../include/timer_manager.h"
#include "../include/connection_manager.h"
#include "../include/network_util.h"

using namespace std;

#define ROUTER_UPDATE_HEADER_SIZE 8
#define ROUTER_UPDATE_ROUTER_SIZE 12

struct __attribute__((__packed__)) router_update_header{
	uint16_t update_fields_num;
	uint16_t source_router_port;
	uint32_t source_ip_addr;
};

struct __attribute__((__packed__)) router_update_router{
	uint32_t ip_addr;
	uint16_t router_port;
	uint16_t padding;
	uint16_t id;
	uint16_t cost;
};

char* create_update_header() {

	struct router_update_header *header;
	char* header_to_send = (char*)malloc(sizeof(char)*ROUTER_UPDATE_HEADER_SIZE);

	uint16_t num = get_routers_num();
	uint32_t ip_addr = get_ip_addr(get_self_id());

	header = (struct router_update_header*)header_to_send;
	header->update_fields_num = htons(get_routers_num());
	header->source_router_port =  htons(ROUTER_PORT);
	header->source_ip_addr = htonl(get_ip_addr(get_self_id()));

	return header_to_send;
}

char* create_update_router() {

	uint16_t routers_num = get_routers_num();
	char* update_router = (char*)malloc(sizeof(char)*ROUTER_UPDATE_ROUTER_SIZE*routers_num);

	vector<uint16_t> routers_id = get_all_routers();

	for (int i = 0; i < routers_id.size(); i++) {

		struct router_update_router* router = (struct router_update_router*)malloc(ROUTER_UPDATE_ROUTER_SIZE);
		router->ip_addr = htonl(get_ip_addr(routers_id[i]));
		router->router_port = htons(get_router_port(routers_id[i]));
		router->id = htons(routers_id[i]);
		router->cost = htons(get_total_cost(routers_id[i]));


		memcpy(update_router+i*ROUTER_UPDATE_ROUTER_SIZE, router, ROUTER_UPDATE_ROUTER_SIZE);
		free(router);

	}


	return update_router;

}

void init_router_socket() {
	
	struct sockaddr_in my_addr;
	router_socket = socket(PF_INET, SOCK_DGRAM, 0);

	bzero(&my_addr, sizeof my_addr);

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(ROUTER_PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(router_socket, (struct sockaddr*)&my_addr, sizeof my_addr);
	add_socket(router_socket);

	start_timer(get_self_id());

}

void send_update(int sock_index) {


	uint16_t routers_num = get_routers_num();
	struct sockaddr_in nei_addr;
	socklen_t to_len = sizeof(nei_addr);

	char* update_header = create_update_header();
	char* update_router =  create_update_router();

	char* update_packet = (char *)malloc(sizeof(char)*(ROUTER_UPDATE_HEADER_SIZE+routers_num*ROUTER_UPDATE_ROUTER_SIZE));

	memcpy(update_packet, update_header, ROUTER_UPDATE_HEADER_SIZE);
	memcpy(update_packet+ROUTER_UPDATE_HEADER_SIZE, update_router, routers_num*ROUTER_UPDATE_ROUTER_SIZE);

	vector<uint16_t> neighbors = get_neighbors();

	for (int i = 0; i < neighbors.size(); i++) {

		uint16_t nei_id = neighbors[i];
		bzero(&nei_addr, to_len);
		nei_addr.sin_family = AF_INET;
		nei_addr.sin_port = htons(get_router_port(nei_id));
		nei_addr.sin_addr.s_addr = htonl(get_ip_addr(nei_id));

		sendto(sock_index, update_packet, ROUTER_UPDATE_HEADER_SIZE+routers_num*ROUTER_UPDATE_ROUTER_SIZE, 0, (struct sockaddr*) &nei_addr, to_len);
		
	}

	free(update_header);
	free(update_router);
	free(update_packet);

	start_timer(get_self_id());

}

void receive_update(int sock_index) {


	char* update_header_chars;
	char* update_packet_chars;
	struct sockaddr_in from;
	socklen_t from_len = sizeof(from);
	uint16_t from_id, update_fields_num;

	update_fields_num = get_routers_num();

	update_packet_chars = (char *)malloc(sizeof(char)*(ROUTER_UPDATE_HEADER_SIZE+update_fields_num*ROUTER_UPDATE_ROUTER_SIZE));

	if (recvfrom(sock_index, update_packet_chars, ROUTER_UPDATE_HEADER_SIZE+update_fields_num*ROUTER_UPDATE_ROUTER_SIZE, 0, (struct sockaddr*)&from, &from_len) == -1) {
		ERROR("Recv router failed!");
	}

	update_header_chars = (char *)malloc(sizeof(char)*ROUTER_UPDATE_HEADER_SIZE);
	bzero(update_header_chars, ROUTER_UPDATE_HEADER_SIZE);
	memcpy(update_header_chars, update_packet_chars, ROUTER_UPDATE_HEADER_SIZE);

	struct router_update_header* update_header = (struct router_update_header*)update_header_chars;
	from_id = find_id_by_ip(ntohl(update_header->source_ip_addr));

	free(update_header_chars);


	for (int i = 0; i < update_fields_num; i++) {

		char* update_router_chars = (char *)malloc(sizeof(char)*ROUTER_UPDATE_ROUTER_SIZE);
		bzero(update_router_chars, ROUTER_UPDATE_ROUTER_SIZE);
		memcpy(update_router_chars, update_packet_chars+ROUTER_UPDATE_HEADER_SIZE+i*ROUTER_UPDATE_ROUTER_SIZE, ROUTER_UPDATE_ROUTER_SIZE);

		struct router_update_router* update_router = (struct router_update_router*)update_router_chars;
		uint16_t goal_id = ntohs(update_router->id);
		uint16_t goal_total_cost = ntohs(update_router->cost);

		set_cost(from_id, goal_id, goal_total_cost);

		free(update_router_chars);

	}

	free(update_packet_chars);

	calculate_table();

	remove_timer(from_id);

	start_timer(from_id);

}

