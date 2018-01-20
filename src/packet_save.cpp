#include <cstdio>
#include <cstring>
#include <string>
#include "../include/global.h"
#include "../include/sendfile_lib.h"

using namespace std;

vector<string> packets;

void add_packet(char* file_pak_header, char* file_pak_data) {

	// printf("ADD PACKET\n");

	string save_packet(file_pak_header, FILE_PAK_HEADER_SIZE);

	string save_packet2(file_pak_data, FILE_PAK_SIZE);

	string final = save_packet+save_packet2;

	// char* save_packet = (char*)malloc(sizeof(char)*(FILE_PAK_HEADER_SIZE+FILE_PAK_SIZE));

	// memcpy(save_packet, file_pak_header, FILE_PAK_HEADER_SIZE);
	// memcpy(save_packet+FILE_PAK_HEADER_SIZE, file_pak_data, FILE_PAK_SIZE);

	// packets.push_back(save_packet);

	// if (packets.size() > 2) {
	// 	char* ch = packets.front();
	// 	free(ch);
	// 	packets.erase(packets.begin());
	// }

	packets.push_back(final);
	if (packets.size() > 2) packets.erase(packets.begin());

}

char* get_last_packet() {

	char* last_packet = (char *)malloc(sizeof(char)*(FILE_PAK_HEADER_SIZE+FILE_PAK_SIZE));
	memcpy(last_packet, packets[packets.size()-1].c_str(), FILE_PAK_HEADER_SIZE+FILE_PAK_SIZE);

	return last_packet;

}

char* get_penultimate_packet() {

	char* penultimate_packet = (char *)malloc(sizeof(char)*(FILE_PAK_HEADER_SIZE+FILE_PAK_SIZE));
	memcpy(penultimate_packet, packets[0].c_str(), FILE_PAK_HEADER_SIZE+FILE_PAK_SIZE);

	return penultimate_packet;

}
