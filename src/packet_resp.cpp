#include <netinet/in.h>
#include <cstring>
#include "../include/global.h"
#include "../include/sendfile_lib.h"
#include "../include/packet_save.h"
#include "../include/network_util.h"
#include "../include/control_header_lib.h"

void last_pak_response(int sock_index) {

	uint8_t control_code, response_code;
	uint16_t response_payload;

	control_code = 0x07;
	response_code = 0x00;
	response_payload = FILE_PAK_HEADER_SIZE+FILE_PAK_SIZE;

	char* response_header = create_response_header(sock_index, control_code, response_code, response_payload);

	char* response_packet = get_last_packet();

	char* resp_pak_header = (char*)malloc(sizeof(char)*FILE_PAK_HEADER_SIZE);
	memcpy(resp_pak_header, response_packet, FILE_PAK_HEADER_SIZE);

	struct FILE_PAK_HEADER* header = (FILE_PAK_HEADER*)resp_pak_header;

	

	printf("LAST PACKET\n");

	printf("des %u trans_id %u TTL %u seq_num %u padding %u\n", ntohl(header->des_ip_addr), header->trans_id, header->TTL, ntohs(header->seq_num), header->padding);

	printf("%.*s\n", FILE_PAK_SIZE, response_packet+FILE_PAK_HEADER_SIZE);

	sendALL(sock_index, response_header, CNTRL_RESP_HEADER_SIZE);
	sendALL(sock_index, response_packet, response_payload);

	free(resp_pak_header);

	free(response_packet);
	free(response_header);

}

void penultimate_pak_response(int sock_index) {

	uint8_t control_code, response_code;
	uint16_t response_payload;

	control_code = 0x08;
	response_code = 0x00;
	response_payload = FILE_PAK_HEADER_SIZE+FILE_PAK_SIZE;

	char* response_header = create_response_header(sock_index, control_code, response_code, response_payload);

	char* response_packet = get_penultimate_packet();

	char* resp_pak_header = (char*)malloc(sizeof(char)*FILE_PAK_HEADER_SIZE);
	memcpy(resp_pak_header, response_packet, FILE_PAK_HEADER_SIZE);

	struct FILE_PAK_HEADER* header = (FILE_PAK_HEADER*)resp_pak_header;

	printf("PENULTIMATE PACKET\n");

	printf("des %u trans_id %u TTL %u seq_num %u padding %u\n", ntohl(header->des_ip_addr), header->trans_id, header->TTL, ntohs(header->seq_num), header->padding);

	printf("%.*s\n", FILE_PAK_SIZE, response_packet+FILE_PAK_HEADER_SIZE);

	sendALL(sock_index, response_header, CNTRL_RESP_HEADER_SIZE);
	sendALL(sock_index, response_packet, response_payload);

	free(response_packet);
	free(response_header);

}

