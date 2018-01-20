#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include "../include/global.h"
#include "../include/connection_manager.h"
#include "../include/sendfile_lib.h"
#include "../include/router_manager.h"
#include "../include/packet_save.h"
#include "../include/network_util.h"
#include "../include/file_stats.h"
#include "../include/write_manager.h"
#include "../include/route_sock_manager.h"

using namespace std;

vector<int> data_recv_sock_vec;

int create_data_socket(int data_port) {
    
    printf("Create data socket\n");
	struct sockaddr_in myaddr;
	socklen_t addr_len;

	data_socket = socket(PF_INET, SOCK_STREAM, 0);

	bzero(&myaddr, sizeof myaddr);

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(data_port);
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(data_socket, (struct sockaddr*) &myaddr, sizeof myaddr) < 0) ERROR("Bind Failed");

	if (listen(data_socket, 5) < 0) ERROR("Listen Failed");

	add_socket(data_socket);

	return data_socket;
 
}

int create_data_recv_socket(int data_socket) {

	struct sockaddr_in recv_addr;
	socklen_t addr_len;

	addr_len = sizeof recv_addr;

	int recv_sock = accept(data_socket, (struct sockaddr*)&recv_addr, &addr_len);

	if (recv_sock < 0) ERROR("Accept Data Socket Failed");

	data_recv_sock_vec.push_back(recv_sock);

	return recv_sock;

}

void remove_data_recv_socket(int sock_index) {

	for (int i = data_recv_sock_vec.size()-1; i >= 0; i--) {

		if (data_recv_sock_vec[i] == sock_index) data_recv_sock_vec.erase(data_recv_sock_vec.begin()+i);

	}

}

bool isData(int recv_sock) {

	for (int i = 0; i < data_recv_sock_vec.size(); i++) {

		if (data_recv_sock_vec[i] == recv_sock) return true;
	}
	return false;
}

bool recvfile(int sock_index) {

	// printf("Recv File\n");

	uint32_t des_ip_addr;
	uint8_t trans_id, TTL;
	uint16_t seq_num;
	uint32_t FIN_num;
	bool is_last;
	

	char* file_pak_header_chars = (char*)malloc(sizeof(char)*FILE_PAK_HEADER_SIZE);

	if (recvALL(sock_index, file_pak_header_chars, FILE_PAK_HEADER_SIZE) < 0) {
		free(file_pak_header_chars);
		remove_data_recv_socket(sock_index);

		return false;
	}

	char* file_pak_data = (char*)malloc(sizeof(char)*FILE_PAK_SIZE);
	if (recvALL(sock_index, file_pak_data, FILE_PAK_SIZE) < 0) {

		free(file_pak_data);
		remove_data_recv_socket(sock_index);
		return false;

	}

	struct FILE_PAK_HEADER* file_pak_header = (struct FILE_PAK_HEADER*)file_pak_header_chars;

	des_ip_addr = ntohl(file_pak_header->des_ip_addr);
	trans_id = file_pak_header->trans_id;
	TTL = file_pak_header->TTL;
	seq_num = ntohs(file_pak_header->seq_num);
	FIN_num = ntohl(file_pak_header->padding);
	is_last = (FIN_num == FIN_PADDING_ONE);

	TTL--;

	// printf("des_ip %u trans_id %u TTL %u seq_num %u FIN_num %u\n" ,des_ip_addr, trans_id, TTL, seq_num, FIN_num);
	// if (is_last) printf("*********LAST PACKET*******\n");
	// printf("%.*s\n", FILE_PAK_SIZE, file_pak_data);

	if (TTL == 0) return true;

	file_pak_header->TTL = TTL;

	add_file_info(trans_id, TTL, seq_num);
	add_packet(file_pak_header_chars, file_pak_data);

	if (!is_self(find_id_by_ip(des_ip_addr))) {

		if (!contains_sock(trans_id)) {
			add_sock(trans_id, create_send_socket(des_ip_addr));
		}

		int send_socket = get_sock(trans_id);

		

		sendALL(send_socket, file_pak_header_chars, FILE_PAK_HEADER_SIZE);
		sendALL(send_socket, file_pak_data, FILE_PAK_SIZE);

		if (is_last) {
			
			remove_sock(trans_id);

			close(send_socket);
		}

	} else {

		write_into_disk(trans_id, file_pak_data, is_last);
	}

	// printf("Free Data Header \n");
		
	free(file_pak_header_chars);


	// printf("Free Data Pak\n");

	free(file_pak_data);

	// printf("Free End\n");

	return true;

}