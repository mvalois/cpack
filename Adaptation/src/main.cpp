#include <iostream>
#include <fstream>
#include "statsgen.h"
using namespace std;



int main(int argc,char* argv[]) {
	if(argc < 2){
		wcerr << "\nMissing arguments\n\t--help for information\n" << endl;
		return -1;
	}

	locale::global(locale(""));
	Statsgen test;
	string filename;


	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (string(argv[i]) == "--hiderare" || string(argv[i]) == "-r") {
				test.setHiderare(1);
			}

			else if (string(argv[i]) == "--top" || string(argv[i]) == "-t") {
				if (argc == i+1) {
					wcerr << "Missing argument --top [value]" << endl;
					return -1;
				}
				test.setTop(atoi(argv[i+1]));
				i++;
			}

			else if (string(argv[i]) == "--help" || string(argv[i]) == "-h") {
				test.showHelp();
				return 0;
			}

			else {
				filename = argv[i];
			}
		}
	}


	if (filename == "") {
		cerr << "Missing filename" << endl;
		return -1;
	}


	if (test.generate_stats(filename)) {
		test.print_stats();
	}
	
	
	return 0;
}
