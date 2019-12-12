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
#include <getopt.h>
#include "statsgen.h"

using namespace std;

#define MISSING_FILENAME "Missing filename"

static struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"withcount", no_argument, NULL, 'w'},
	{"hiderare", no_argument, NULL, 'H'},
	{"top", required_argument, NULL, 't'},
	{"regex", required_argument, NULL, 'r'},
	{"out", required_argument, NULL, 'o'},
	{"debug", no_argument, NULL, 'd'},
	{"limit-advanced-masks", required_argument, NULL, 'A'},
	{"limit-simple-masks", required_argument, NULL, 'S'},
	{"parallel", required_argument, NULL, 'p'},
	{"security", no_argument, NULL, 's'},
	{NULL, 0, NULL, 0}
};


void showHelp() {
	static const char* USAGE[] = {
		"Usage: stats FILENAME [OPTIONS]",
		"To be sure the database's format can be read, please use this command before:",
		"	iconv -f ISO-8859-1 -t UTF-8 databaseInput.txt -o databaseOutput.txt",
		"Use '-' for FILENAME to read from stdin",
		"Options:",
		"	--help, -h          Show this help message",
		"	--withcount, -w     Mendatory if the input database has the following format : [number of occurence] [password]",
		"	--hiderare, -H      Hide all statistics below 1%",
		"	--top, -t [value]   Show only [value] first results",
		"	--regex, -r [value] Show result for password, using the regular expression [value]",
		"	--out, -o [value]   Writes masks into [value] file",
		"	--debug, -d         Enable debug printing",
		"",
		"Optimisation options to reduce the execution time : ",
		"	--limit-advanced-masks, -A [value]  Limit the size of the advanced masks at [value], if size>[value]: othermasks",
		"	--limit-simple-masks, -S [value]    Limit the size of the simple masks at [value], if size>[value]: othermasks",
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
	locale::global(locale(""));
	
	string filename;
	if(argc > 1){
		filename = argv[optind];
	}
	else {
		showHelp();
	}

	Statsgen statsgen(filename);

	int opt;
	while ((opt = getopt_long(argc, argv, "hwHt:r:o:dA:S:p:s", long_options, NULL)) != -1){
		switch(opt){
			case 'h':
				showHelp(); break;
			case 'w':
				statsgen.setWithcount(true); break;
			case 'H':
				statsgen.setHiderare(1); break;
			case 't':
				statsgen.setTop(atoi(optarg)); break;
			case 'r':
				statsgen.setRegex(optarg); break;
			case 'o':
				statsgen.setOutfile(optarg); break;
			case 'd':
				statsgen.enableDebug(); break;
			case 'A':
				statsgen.setLimitAdvancedmask(atoi(optarg)); break;
			case 'S':
				statsgen.setLimitSimplemask(atoi(optarg)); break;
			case 'p':
				statsgen.setNbThread(atoi(optarg)); break;
			case 's':
				statsgen.setSecurityRules(); break;
			default:
				showHelp(); break;
		}

	}
	
	if (statsgen.generate_stats()) {
		statsgen.print_stats();
	}

	return 0;
}
