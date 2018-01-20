#ifndef _TIMER_MANAGER_H_
#define _TIMER_MANAGER_H_

bool has_timer();

void start_timer(uint16_t id);

int get_next_stop_time();

uint16_t get_stop_id_and_pop();

void remove_timer(uint16_t id);

#endif