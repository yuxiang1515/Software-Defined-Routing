#include <map>
#include "../include/global.h"

using namespace std;

map<uint8_t, int> socks;

bool contains_sock(uint8_t trans_id) {

	return socks.count(trans_id);

}

void add_sock(uint8_t trans_id, int sock_index) {

	printf("Add DATA SOCK");

	socks[trans_id] = sock_index;

}

int get_sock(uint8_t trans_id) {
	return socks[trans_id];
}

void remove_sock(uint8_t trans_id) {

	socks.erase(trans_id);
}


