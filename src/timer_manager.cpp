
#include <sys/time.h>
#include "../include/global.h"
#include "../include/router_manager.h"

using namespace std;

class Next_Time {
	public:
		uint16_t id;
		int stop_time;
};

vector<Next_Time> wait_list;

bool has_timer() {
	return !wait_list.empty();
}

void start_timer(uint16_t id) {

	uint16_t interval = get_update_interval();
	Next_Time next_time;
	next_time.id = id;
	struct timeval currenttime;
	gettimeofday(&currenttime, NULL);

	if (is_self(id)) {
		next_time.stop_time = currenttime.tv_sec+interval;
	} else {
		next_time.stop_time = currenttime.tv_sec+3*interval;
	}
	wait_list.push_back(next_time);

}

int get_next_stop_time() {

	Next_Time nt = wait_list[0];

	for (int i = 1; i < wait_list.size(); i++) {

		if (wait_list[i].stop_time < nt.stop_time) nt = wait_list[i];

	}

	struct timeval currenttime;
	gettimeofday(&currenttime, NULL);

	return nt.stop_time-currenttime.tv_sec;
}

uint16_t get_stop_id_and_pop() {

	int st = 0;

	for (int i = 1; i < wait_list.size(); i++) {

		if (wait_list[i].stop_time < wait_list[st].stop_time) st = i;

	}

	uint16_t id = wait_list[st].id;
	wait_list.erase(wait_list.begin()+st);

	printf("id: %d stop\n", id);

	return id;
}

void remove_timer(uint16_t id) {
	for (int i = 0; i < wait_list.size(); i++) {
		if (wait_list[i].id == id) {
			wait_list.erase(wait_list.begin()+i);
			break;
		}
	}
}
