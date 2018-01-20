#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/global.h"
#include "../include/router_manager.h"
#include "../include/control_header_lib.h"
#include "../include/network_util.h"

struct __attribute__((__packed__)) UPDATE_INFO{
	uint16_t nei_id;
	uint16_t cost;
};

void update_response(int sock_index, char* payload) {

	struct UPDATE_INFO* update_info = (struct UPDATE_INFO*)payload;

	uint8_t control_code, response_code;

	uint16_t payload_len;

	set_nei_cost(ntohs(update_info->nei_id), ntohs(update_info->cost));

	calculate_table();

	control_code = 0x03;
	response_code = 0x00;
	payload_len = 0;

	char* response_header = create_response_header(sock_index, control_code, response_code, payload_len);

	sendALL(sock_index, response_header, CNTRL_RESP_HEADER_SIZE);

	free(response_header);

}