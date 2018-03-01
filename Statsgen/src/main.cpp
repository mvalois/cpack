/**
 * @file main.cpp
 * @brief statistical analysis of a database of passwords
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 * Copyright (c) 2018 Jorand Jean-baptiste Bass Yannick
 * All rights reserved.
 *
 * Please see the attached LICENSE file for additional licensing information.
 */



#include <fstream>
#include <unistd.h>
#include "statsgen.h"

using namespace std;


void showHelp() {
	static const char* USAGE[] = {
		"Usage: stats database.txt [OPTIONS]",
		"To be sure the database's format can be read, please use this command before:",
		"	iconv -f ISO-8859-1 -t UTF-8 databaseInput.txt -o databaseOutput.txt",
		"Options:",
		"	--help, -h          Show this help message",
		"	--withcount, -w     Mendatory if the input database has the following format : [number of occurence] [password]",
		"	--hiderare, -hr     Hide all statistics below 1%",
		"	--top, -t [value]   Show only [value] first results",
		"	--regex, -r [value] Show result for password, using the regular expression [value]",
		"",
		"Optimisation options to reduce the execution time : ",
		"	--limitadvancedmasks, -lam [value]  Limit the size of the advanced masks at [value], if size>[value]: othermasks",
		"	--limitsimplemasks, -lsm [value]    Limit the size of the simple masks at [value], if size>[value]: othermasks",
		"	--parallel, -p [value]              Number of usable threads",
		"",
		"",
		"Security rules: ",
		"	--security, -s		Define the security rules",
		"",
        NULL
    };
    for(int line = 0; USAGE[line] != NULL; line++){
            cerr << USAGE[line] << endl;
    }
    exit(EXIT_FAILURE);
}


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
				showHelp();
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

			else if (string(argv[i]) == "--security" || string(argv[i]) == "-s") {
				test.setSecurityRules();
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


	test.setFilename(filename);
	if (test.generate_stats()) {
		test.print_stats();
	}

	return 0;
}
