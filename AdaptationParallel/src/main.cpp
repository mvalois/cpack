/**
 * @file main.cpp
 * @brief statical analysis of a database of passwords
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 */



#include <fstream>
#include <unistd.h>
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
			if (string(argv[i]) == "--hiderare" || string(argv[i]) == "-hr") {
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

			else if (string(argv[i]) == "--regex" || string(argv[i]) == "-r") {
				if (argc == i+1) {
					wcerr << "Missing argument --top [value]" << endl;
					return -1;
				}
				test.setRegex(argv[i+1]);
				i++;
			}

			else if (string(argv[i]) == "--limitsimplemasks" || string(argv[i]) == "-lsm") {
				if (argc == i+1) {
					wcerr << "Missing argument --limitsimplemasks [value]" << endl;
					return -1;
				}
				test.setLimitSimplemask(atoi(argv[i+1]));
				i++;
			}

			else if (string(argv[i]) == "--limitadvancedmasks" || string(argv[i]) == "-lam") {
				if (argc == i+1) {
					wcerr << "Missing argument --limitadvancedmasks [value]" << endl;
					return -1;
				}
				test.setLimitAdvancedmask(atoi(argv[i+1]));
				i++;
			}

			else if (string(argv[i]) == "--help" || string(argv[i]) == "-h") {
				test.showHelp();
				return 0;
			}

			else if (string(argv[i]) == "--withcount" || string(argv[i]) == "-w") {
				test.setWithcount(true);
			}

			else if (string(argv[i]) == "--parallel" || string(argv[i]) == "-p") {
				if (argc == i+1) {
					wcerr << "Missing argument --parallel [value]" << endl;
					return -1;
				}
				test.setNbThread(atoi(argv[i+1]));
				i++;
			}

			else {
				filename = argv[i];
			}
		}
	}



	if (filename == "") {
		wcerr << "Missing filename" << endl;
		return -1;
	}




	if (test.generate_stats(filename)) {
		test.print_stats();
	}

	return 0;
}
