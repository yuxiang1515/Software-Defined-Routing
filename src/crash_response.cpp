#include <sys/socket.h>
#include "../include/global.h"
#include "../include/control_header_lib.h"
#include "../include/network_util.h"

void crash_response(int sock_index) {

	uint8_t control_code, response_code;
	uint16_t payload_len;

	control_code = 0x04;
	response_code = 0x00;
	payload_len = 0x00;

	char* response_header = create_response_header(sock_index, control_code, response_code, payload_len);

	sendALL(sock_index, response_header, CNTRL_RESP_HEADER_SIZE);

	free(response_header);

	exit(0);

}