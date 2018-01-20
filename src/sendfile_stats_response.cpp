#include <netinet/in.h>
#include <cstring>
#include "../include/global.h"
#include "../include/file_stats.h"
#include "../include/control_header_lib.h"
#include "../include/network_util.h"

#define CNTRL_STATS_SIZE 1
#define RESP_STATS_HEADER_SIZE 4
#define RESP_STATS_SEQNUM_SIZE 2

using namespace std;

struct __attribute__((__packed__)) RESP_STATS_HEADER
{
	uint8_t trans_id;
	uint8_t TTL;
	uint16_t padding;
};

char* create_response_stats_header(uint8_t trans_id, uint8_t TTL) {

	char* response_stats_header_chars = (char *) malloc(sizeof(char)*RESP_STATS_HEADER_SIZE);
	bzero(response_stats_header_chars, RESP_STATS_HEADER_SIZE);

	struct RESP_STATS_HEADER* resp_stats_header = (struct RESP_STATS_HEADER*)response_stats_header_chars;
	resp_stats_header->trans_id = trans_id;
	resp_stats_header->TTL = TTL;

	return response_stats_header_chars;

}

char* create_response_stats_seqnums(vector<uint16_t> seq_nums) {

	char* response_stats_seqnums = (char*)malloc(sizeof(char)*seq_nums.size()*RESP_STATS_SEQNUM_SIZE);
	for (int i = 0; i < seq_nums.size(); i++) {

		uint16_t seq_num = htons(seq_nums[i]);
		memcpy(response_stats_seqnums+i*RESP_STATS_SEQNUM_SIZE, &seq_num, RESP_STATS_SEQNUM_SIZE);

	}
	return response_stats_seqnums;

}

void sendfile_stats_response(int sock_index, char* cntrl_payload) {

	uint8_t trans_id, response_code, control_code;
	uint16_t payload_len;

	memcpy(&trans_id, cntrl_payload, CNTRL_STATS_SIZE);

	uint8_t TTL = get_TTL(trans_id);

	vector<uint16_t> seq_nums = get_seq_nums(trans_id);

	response_code = 0x00;
	control_code = 0x06;
	payload_len = RESP_STATS_HEADER_SIZE+seq_nums.size()*RESP_STATS_SEQNUM_SIZE;
	char* response_header = create_response_header(sock_index, control_code, response_code, payload_len);

	char* response_stats_header = create_response_stats_header(trans_id, TTL);

	printf("seqnum  %d\n", seq_nums.size());

	char* resp_stats_seqnums = create_response_stats_seqnums(seq_nums);

	sendALL(sock_index, response_header, CNTRL_RESP_HEADER_SIZE);
	sendALL(sock_index, response_stats_header, RESP_STATS_HEADER_SIZE);
	sendALL(sock_index, resp_stats_seqnums, seq_nums.size()*RESP_STATS_SEQNUM_SIZE);

	free(resp_stats_seqnums);
	free(response_stats_header);
	free(response_header);

}