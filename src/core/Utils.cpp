#include "Utils.h"

uint64_t nbline_file(const std::string& filename) {
	std::ifstream readfile(filename);
	std::string line;
	uint64_t nb = 0;

	while(std::getline(readfile, line)){
		++nb;
	}
	// we have not read the whole file
	if (readfile.fail() && !readfile.eof()){
		std::cerr << "[ERROR] There was an error reading the file at line " << nb << std::endl;
		return 0;
	}
	return nb;
}