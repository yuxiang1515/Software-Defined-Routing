#ifndef _PACKET_SAVE_H_
#define _PACKET_SAVE_H_

void add_packet(char* file_pak_header, char* file_pak_data);

char* get_last_packet();

char* get_penultimate_packet();

#endif 