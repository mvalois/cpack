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

#include "Statsgen.h"

using namespace std;

#define MISSING_FILENAME "Missing filename"

inline void notInteger(const char* value){
	cerr << "Error: " << value << " is not an integer" << endl;
}

inline void needArgument(const char* value){
	cerr << "Error: " << value << " needs an argument" << endl;
}

inline void unknownArg(const char* value){
	cerr << "Error: argument unknown: " << value << endl;
}

void showHelp() {
	static const char* USAGE[] = {
		"Usage: stats FILENAME [OPTIONS]",
		"To be sure the database's format can be read, please use this command before:",
		"	iconv -f ISO-8859-1 -t UTF-8 databaseInput.txt -o databaseOutput.txt",
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
			cout << USAGE[line] << endl;
	}
	exit(EXIT_SUCCESS);
}

void askSecurityRules(Statsgen& stats){
	uint length, special, digit, upper, lower;
	cout << "Minimal length of a password:" << endl;
	cin >> length;

	cout << "Minimum of special characters in a password:" << endl;
	cin >> special;

	cout << "Minimum of digits in a password:" << endl;
	cin >> digit;

	cout << "Minimum of lower characters in a password:" << endl;
	cin >> lower;

	cout << "Minimum of upper characters in a password:" << endl;
	cin >> upper;
	stats.setSecurityRules(length, special, digit, upper, lower);
}

int main(int argc,char* argv[]) {
	locale::global(locale(""));
	
	string filename;
	if(argc >= 2){
		filename = argv[1];
	}
	else {
		showHelp();
	}

	Statsgen statsgen(filename);
	string arg;
	for(int i=2; i < argc; ++i){
		arg = string(argv[i]);
		if(arg == "-h" || arg == "--help"){
			showHelp(); return EXIT_SUCCESS;
		}
		if(arg == "-w" || arg == "--withcount"){
			statsgen.setWithcount(true); continue;
		}
		if(arg == "-H" || arg == "--hiderare"){
			statsgen.setHiderare(1); continue;
		}
		if(arg == "-t" || arg == "--top"){
			try{
				statsgen.setTop(stoi(argv[++i])); continue;
			}
			catch(std::invalid_argument){ notInteger(argv[i]); }
			catch(std::logic_error){ needArgument(argv[i-1]); }
			return EXIT_FAILURE;
		}
		if(arg == "-r" || arg == "--regex"){
			statsgen.setRegex(argv[++i]); continue;
		}
		if(arg == "-o" || arg == "--out"){
			statsgen.setOutfile(argv[++i]); continue;
		}
		if(arg == "-d" || arg == "--debug"){
			statsgen.enableDebug(); continue;
		}
		if(arg == "-A" || arg == "--limit-advanced-masks"){
			try{
				statsgen.setLimitAdvancedmask(stoi(argv[++i])); continue;
			}
			catch(std::invalid_argument){ notInteger(argv[i]); }
			catch(std::logic_error){ needArgument(argv[i-1]); }
			return EXIT_FAILURE;
		}
		if(arg == "-S" || arg == "--limit-simple-masks"){
			try{
				statsgen.setLimitSimplemask(stoi(argv[++i])); continue;
			}
			catch(std::invalid_argument){ notInteger(argv[i]); }
			catch(std::logic_error){ needArgument(argv[i-1]); }
			return EXIT_FAILURE;
		}
		if(arg == "-p" || arg == "--parallel"){
			try{
				statsgen.setNbThread(stoi(argv[++i])); continue;
			}
			catch(std::invalid_argument){ notInteger(argv[i]); }
			catch(std::logic_error){ needArgument(argv[i-1]); }
			return EXIT_FAILURE;
		}
		if(arg == "-s" || arg == "--security"){
			askSecurityRules(statsgen); continue;
		}
		else {
			unknownArg(argv[i]);
		}
	}
	
	if (statsgen.generate_stats()) {
		statsgen.print_stats();
	}

	return 0;
}
