#ifndef NETWORK_UTIL_H_
#define NETWORK_UTIL_H_

ssize_t recvALL(int sock_index, char *buffer, ssize_t nbytes);
ssize_t sendALL(int sock_index, char *buffer, ssize_t nbytes);

int recvfromAll(int sock_index, char *buffer, int len, struct sockaddr_in* from, socklen_t from_len);
int sendtoAll(int sock_index, char *buffer, int len, struct sockaddr_in* to, socklen_t to_len);

#endif