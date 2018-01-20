#ifndef _RECVFILE_MANAGER_H_
#define _RECVFILE_MANAGER_H_

int create_data_socket(int data_port);

int create_data_recv_socket(int data_socket);

bool isData(int recv_sock);

bool recvfile(int sock_index);

#endif