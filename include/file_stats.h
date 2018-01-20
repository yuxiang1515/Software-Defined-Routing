#ifndef _FILE_STATS_H_
#define _FILE_STATS_H_

void add_file_info(uint8_t trans_id, uint8_t TTL, uint16_t seq_num);

uint8_t get_TTL(uint8_t trans_id);

std::vector<uint16_t> get_seq_nums(uint8_t trans_id);

#endif