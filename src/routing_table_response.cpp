
using namespace std;

#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/global.h"
#include "../include/control_header_lib.h"
#include "../include/router_manager.h"
#include "../include/network_util.h"

#define TABLE_ENTRI_SIZE 8


struct __attribute__((__packed__)) table_entri
{
	uint16_t id;
	uint16_t padding;
	uint16_t next_hop_id;
	uint16_t cost;
};

char* create_table_entri(uint16_t id) {

	char* buffer = (char *)malloc(sizeof(char)*TABLE_ENTRI_SIZE);

	struct table_entri* entri = (struct table_entri*)buffer;

	uint16_t next_hop = get_next_hop(id);
	uint16_t cost = get_total_cost(id);

	printf("TABLE id: %u, next_hop: %u, cost: %u\n", id, next_hop, cost);

	entri->id = htons(id);
	entri->next_hop_id = htons(get_next_hop(id));
	entri->cost = htons(get_total_cost(id));

	return buffer;
}

void routing_table_response(int sock_index) {
	uint8_t control_code = 0x02;
	uint8_t response_code = 0x00;
	int routers_num = get_routers_num();

	char* response_header = create_response_header(sock_index, control_code, response_code, routers_num*TABLE_ENTRI_SIZE);

	sendALL(sock_index, response_header, CNTRL_RESP_HEADER_SIZE);
	free(response_header);

	vector<uint16_t> ids = get_all_routers();

	for (int i = 0; i < ids.size(); i++) {

		char* entri = create_table_entri(ids[i]);

		sendALL(sock_index, entri, TABLE_ENTRI_SIZE);

		free(entri);
	}
}

