#ifndef _SENDFILE_LIB_H_
#define _SENDFILE_LIB_H_

#define FILE_PAK_HEADER_SIZE 12
#define FILE_PAK_SIZE 1024
#define FIN_PADDING_ONE 2147483648

struct __attribute__((__packed__)) FILE_PAK_HEADER
{
	uint32_t des_ip_addr;
	uint8_t trans_id;
	uint8_t TTL;
	uint16_t seq_num;
	uint32_t padding;
};

int create_send_socket(uint32_t des_ip_addr);

char* create_file_pak_header(uint32_t des_ip_addr, uint8_t trans_id, uint8_t TTL, uint16_t seq_num, bool is_last);

#endif