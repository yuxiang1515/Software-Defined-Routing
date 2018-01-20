#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include "../include/global.h"
#include "../include/router_manager.h"
#include "../include/sendfile_lib.h"
#include "../include/network_util.h"
#include "../include/file_stats.h"
#include "../include/packet_save.h"
#include "../include/control_header_lib.h"

#define CNTRL_SEND_HEADER_SIZE 8

struct __attribute__((__packed__)) CNTRL_SEND_HEADER
{
	uint32_t des_ip_addr;
	uint8_t TTL;
	uint8_t tran_id;
	uint16_t seq_num;
};


void sendfile_response(int sock_index, char* payload, uint16_t payload_len) {

	uint32_t des_ip_addr;
	uint8_t TTL;
	uint8_t trans_id;
	uint16_t seq_num;
	char* filename;
	
	FILE* file_point;
	int send_socket, count, pak_num;
	bool has_next;
	long file_size;

	// printf("SendFile\n");

	struct CNTRL_SEND_HEADER* cntrl_send_header = (struct CNTRL_SEND_HEADER*)malloc(CNTRL_SEND_HEADER_SIZE);
	memcpy(cntrl_send_header, payload, CNTRL_SEND_HEADER_SIZE);

	des_ip_addr = ntohl(cntrl_send_header->des_ip_addr);
	TTL = cntrl_send_header->TTL;
	trans_id = cntrl_send_header->tran_id;
	seq_num = ntohs(cntrl_send_header->seq_num);

	free(cntrl_send_header);

	if (TTL == 0) return;

	filename = (char *) malloc(sizeof(char)*(payload_len-CNTRL_SEND_HEADER_SIZE+1));
	bzero(filename, payload_len-CNTRL_SEND_HEADER_SIZE+1);
	memcpy(filename, payload+CNTRL_SEND_HEADER_SIZE, payload_len-CNTRL_SEND_HEADER_SIZE);

	printf("filename: %.*s\n", payload_len-CNTRL_SEND_HEADER_SIZE, filename);

	send_socket = create_send_socket(des_ip_addr);

	file_point = fopen(filename, "r");

	if (file_point == NULL) ERROR("Open File Failed");

	fseek(file_point, 0L, SEEK_END);
	file_size = ftell(file_point);
	pak_num = file_size/FILE_PAK_SIZE;

	printf("file_size: %ld pak_num %d\n", file_size, pak_num);

	fseek(file_point, 0L, SEEK_SET);

	char* file_content = (char *)malloc(sizeof(char)*(file_size+1));
	memset(file_content, '\0', file_size+1);

	fread(file_content, 1, file_size, file_point);

	count = 0;

	for (count = 0; count < pak_num; count++) {

		char* file_pak_header = create_file_pak_header(des_ip_addr, trans_id, TTL, seq_num+count, count == pak_num-1);

		// printf("des_ip: %u TTL %u tran_id %u seq_num %u\n", des_ip_addr, TTL, trans_id, seq_num+count);
		// printf("%.*s\n", FILE_PAK_SIZE, file_content+count*FILE_PAK_SIZE);

		sendALL(send_socket, file_pak_header, FILE_PAK_HEADER_SIZE);
		sendALL(send_socket, file_content+count*FILE_PAK_SIZE, FILE_PAK_SIZE);

		add_packet(file_pak_header, file_content+count*FILE_PAK_SIZE);
		
		add_file_info(trans_id, TTL, seq_num+count);
		free(file_pak_header);
		
	}

	

	fclose(file_point);
	free(filename);
	free(file_content);

	close(send_socket);

	char* sendfile_response_header = create_response_header(sock_index, 0x05, 0x00, 0);

	sendALL(sock_index, sendfile_response_header, CNTRL_RESP_HEADER_SIZE);

	free(sendfile_response_header);

}


