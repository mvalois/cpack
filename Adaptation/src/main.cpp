#include <iostream>
#include <fstream>
#include "statsgen.h"
using namespace std;



int main(int argc,char* argv[]) {
	if(argc < 2){
		wcerr << "Missing arguments" << endl;
		return -1;
	}

	locale::global(locale(""));

	Statsgen test;

	string filename = argv[1];

	if (test.generate_stats(filename)) {
		test.print_stats();
	}
	
	
	return 0;
}
