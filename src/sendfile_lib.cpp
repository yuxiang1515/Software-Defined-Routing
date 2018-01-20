#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstring>
#include "../include/global.h"
#include "../include/router_manager.h"
#include "../include/sendfile_lib.h"

int create_send_socket(uint32_t des_ip_addr) {

	struct sockaddr_in des_addr;
	socklen_t addr_len;

	printf("%s\n", "Create Send Socket");

	uint16_t des_id = find_id_by_ip(des_ip_addr);
	uint16_t next_hop_id = get_next_hop(des_id);
	uint32_t next_hop_ip_addr = get_ip_addr(next_hop_id);
	uint16_t next_hop_data_port = get_data_port(next_hop_id);


	int send_socket = socket(PF_INET, SOCK_STREAM, 0);

	des_addr.sin_family = AF_INET;
	des_addr.sin_port = htons(next_hop_data_port);
	des_addr.sin_addr.s_addr = htonl(next_hop_ip_addr);
	memset(&des_addr.sin_zero, '\0', sizeof des_addr.sin_zero);
	addr_len = sizeof des_addr;

	printf("des_id %u next_hop_id: %u ip_addr %u, data_port %u\n", des_id, next_hop_id, next_hop_ip_addr, next_hop_data_port);

	if (connect(send_socket, (struct sockaddr*)&des_addr, addr_len) < 0) ERROR("Connect Failed");

	return send_socket;
}

char* create_file_pak_header(uint32_t des_ip_addr, uint8_t trans_id, uint8_t TTL, uint16_t seq_num, bool is_last) {

	char* file_packet_header_chars = (char*)malloc(sizeof(char)*FILE_PAK_HEADER_SIZE);
	struct FILE_PAK_HEADER* file_pak_header = (struct FILE_PAK_HEADER*)file_packet_header_chars;
	file_pak_header->des_ip_addr = htonl(des_ip_addr);
	file_pak_header->trans_id = trans_id;
	file_pak_header->TTL = TTL;
	file_pak_header->seq_num = htons(seq_num);

	if (is_last) file_pak_header->padding = htonl(FIN_PADDING_ONE);
	else file_pak_header->padding = htonl(0);

	return file_packet_header_chars;

}