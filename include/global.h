#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <vector>
#define INF 65535

#define ERROR(err_msg) {perror(err_msg); exit(EXIT_FAILURE);}

/* https://scaryreasoner.wordpress.com/2009/02/28/checking-sizeof-at-compile-time/ */
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)])) // Interesting stuff to read if you are interested to know how this works

extern uint16_t CONTROL_PORT, ROUTER_PORT, DATA_PORT;
extern int control_socket, router_socket, data_socket;

void set_init(uint16_t routers_num, uint16_t update_interval);

uint16_t get_routers_num();

uint16_t get_update_interval();

#endif