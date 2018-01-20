#include <map>
#include <cstring>
#include <string>
#include <sstream>
#include "../include/global.h"
#include "../include/sendfile_lib.h"

#define BUFFER_SIZE 10485760

using namespace std;

struct Write_Info {
	uint8_t id;
	int count;
	char* buffer;
};

vector<struct Write_Info> infos;

void write_file(uint8_t trans_id, int loc) {

	// printf("%s\n", "WRITE FILE");

	FILE* file_point;

	int t_id = trans_id;
	string fileheader = "file-";
		
	stringstream ss;
	ss << fileheader << t_id;
	string filename = ss.str();

	printf("filename: %.*s \n", filename.length(), filename.c_str());

	file_point = fopen(filename.c_str(), "w");

	if (file_point == NULL) ERROR("OPEN FILE FAILED");

	fprintf(file_point, "%.*s", FILE_PAK_SIZE*infos[loc].count, infos[loc].buffer);

	fclose(file_point);
	free(infos[loc].buffer);
	infos.erase(infos.begin()+loc);

}

void write_into_disk(uint8_t trans_id, char* file_pak_data, bool is_last) {

	// printf("%s\n", "WRITE INTO DISK");

	int i;
	for (i = 0; i < infos.size(); i++) {
		if (infos[i].id == trans_id) {
			break;
		}
	}

	if (i == infos.size()) {

		struct Write_Info info;
		info.id = trans_id;
		info.count = 0;
		info.buffer = (char *) malloc (sizeof(char)*BUFFER_SIZE);
		if (info.buffer == NULL) ERROR("CREATE BUFFER ERROR");
		infos.push_back(info);

	}

	memcpy(infos[i].buffer+infos[i].count*FILE_PAK_SIZE, file_pak_data, FILE_PAK_SIZE);
	infos[i].count++;

	if (is_last) {

		printf("%s\n", "is_last");

		write_file(trans_id, i);

		
	}

}



