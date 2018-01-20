#include <map>
#include "../include/global.h"

using namespace std;

class File_Info {
	private:
		uint8_t TTL;
		vector<uint16_t> seq_nums;

	public:
		File_Info() {
			seq_nums.reserve(10240);
		}

		void set_TTL(uint8_t t) {
			TTL = t;
		}

		uint8_t get_TTL(){
			return TTL;
		}

		void add_seq_num(uint16_t seq_num) {
			seq_nums.push_back(seq_num);
		}

		vector<uint16_t> get_seq_nums() {
			return seq_nums;
		}
};

map<uint8_t, File_Info> file_info_map;

void add_file_info(uint8_t trans_id, uint8_t TTL, uint16_t seq_num) {

	file_info_map[trans_id].set_TTL(TTL);
	file_info_map[trans_id].add_seq_num(seq_num);

}

uint8_t get_TTL(uint8_t trans_id) {

	uint8_t zero = 0;

	if (!file_info_map.count(trans_id)) return zero;

	return file_info_map[trans_id].get_TTL();

}

vector<uint16_t> get_seq_nums(uint8_t trans_id) {
	return file_info_map[trans_id].get_seq_nums();
}