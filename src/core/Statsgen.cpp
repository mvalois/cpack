/**
 * @file statsgen.cpp
 * @brief Creation of all methods from Statsgen
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 * Copyright (c) 2018 Jorand Jean-baptiste Bass Yannick
 * All rights reserved.
 *
 * Please see the attached LICENSE file for additional licensing information.
 */


#include <fstream>
#include <iomanip>
#include <algorithm>
#include <thread>


#include "Statsgen.h"
#include "Utils.h"
using namespace std;

Statsgen::Statsgen(const std::string& name):filename(name){}


void Statsgen::askSecurityRules() {
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
	setSecurityRules(length, special, digit, upper, lower);
}

void Statsgen::setSecurityRules(const uint& length, const uint& special, const uint& digit, const uint& upper, const uint& lower) {
	_sr = { _sr.nbSecurePassword, length, special, digit, upper, lower };
}

void Statsgen::configureThread(ThreadData& td) const {
	td.filename = filename;
	td.current_regex = current_regex;
	td.use_regex = use_regex;
	td.withcount = withcount;
	td.limitSimplemask = limitSimplemask;
	td.limitAdvancedmask = limitAdvancedmask;
	td.sr = { 0, _sr.minLength, _sr.minSpecial, _sr.minDigit, _sr.minLower, _sr.minUpper };
}

int Statsgen::generate_stats() {
	td = vector<ThreadData>(nbThread);
	results = ThreadData();
	finished = 0;
	started = false;
	uint64_t nbline = 0;
	nbline = nbline_file(filename);
	if (!nbline){ // error reading the file
		cerr << "[ERROR] Empty file or not existing file" << endl;
		return 0;
	}

	vector<pthread_t> threads(nbThread);

	for(uint i = 0; i < nbThread; i++ ) {
		td[i].thread_id = i + 1;
		configureThread(td[i]);

		if(i == 0){
			td[i].lineBegin = 0;
		}
		else{
			td[i].lineBegin =  td[i-1].lineEnd + 1;
		}
		td[i].lineEnd = (i+1)*nbline/nbThread;

		if(debug_enabled){
			cerr << "[DEBUG] " << "Thread " << td[i].thread_id << " analyse : " << td[i].lineBegin << " --> " << td[i].lineEnd << endl;
		}

		int rc = pthread_create(&threads[i], NULL, generate_stats_thread, (void *)&td[i] );

		if (rc) {
			cerr << "[ERROR] unable to create thread," << rc << endl;
			exit(-1);
		}
	}

	started = true;

	void *status;
	for(uint i = 0; i < nbThread; i++ ) {
		int rc = pthread_join(threads[i], &status);
		if (rc) {
			cerr << "[ERROR] unable to join," << rc << endl;
			exit(-1);
		}
		results += td[i];
		finished++;
	}

	if (!results.total_counter) {
		cerr << "[ERROR] Empty file or not existing file" << endl;
		return 0;
	}

	return 1;
}

void Statsgen::print_stats() {
	int count;
	float perc = percentage(results.total_filter, results.total_counter);

	cout << "\n\tSelected " << results.total_filter << " on " << results.total_counter << " passwords\t("
		<< perc << " %)" << endl;


	cout << "\nSecurity rules : " << endl;
	cout << "\tMinimal length of a password: " << results.sr.minLength << endl;
	cout << "\tMinimum of special characters in a password: " << results.sr.minSpecial << endl;
	cout << "\tMinimum of digits in a password: " << results.sr.minDigit << endl;
	cout << "\tMinimum of lower characters in a password: " << results.sr.minLower << endl;
	cout << "\tMinimum of upper characters in a password: " << results.sr.minUpper << endl;

	float perce = percentage(results.sr.nbSecurePassword, results.total_counter);
	cout << "\n\t\t--> " << results.sr.nbSecurePassword << " passwords\t(" << perce << " %) respect the security rules\n" << endl;


	cout << "\nmin - max\n" << endl;
	cout << setw(43) << right << "digit:  "
			<< setw(2) << right << results.minMaxValue.mindigit << " - " << results.minMaxValue.maxdigit << endl;
	cout << setw(43) << right << "lower:  "
			<< setw(2) << right << results.minMaxValue.minlower << " - " << results.minMaxValue.maxlower << endl;
	cout << setw(43) << right << "upper:  "
			<< setw(2) << right << results.minMaxValue.minupper << " - " << results.minMaxValue.maxupper << endl;
	cout << setw(43) << right << "special:  "
			<< setw(2) << right << results.minMaxValue.minspecial << " - " << results.minMaxValue.maxspecial << endl;



	cout << "\nStatistics relative to length: \n" << endl;
	showMap(results.length, top, results.total_counter, hiderare, count);

	cout << "\nStatistics relative to charsets: \n" << endl;
	showMap(results.charactersets, -1, results.total_counter, hiderare, count);


	cout << "\nStatistics relative to simplemasks: \n" << endl;
	showMap(results.simplemasks, top, results.total_counter, hiderare, count);

	if (limitSimplemask > 0) {
		cout << endl;
		readResult(results.simplemasks["othermasks"], "othermasks", count, results.total_counter, hiderare);
	}


	cout << "\nStatistics relative to advancedmask: \n" << endl;
	showMap(results.advancedmasks, top, results.total_counter, hiderare, count);

	if (! outfile_name.empty()){
		locale::global(locale("C"));
		ofstream outfile_stream(outfile_name);
		map<uint64_t, string, greater<uint64_t>> reverse = flip_map(results.advancedmasks);
		for(pair<uint64_t, string> it : reverse){
			if(it.second == "othermasks") continue;
			outfile_stream << it.second << "," << it.first << endl;
		}
		outfile_stream.close();
	}

	if (limitAdvancedmask > 0) {
		cout << endl;
		readResult(results.advancedmasks["othermasks"], "othermasks", count, results.total_counter, hiderare);
	}
}


pair<uint, uint> get_masks(const string& password, PasswordStats& c){
	char last_simplemask = 'a';
	uint sizeSimpleMask = 0;
	uint sizeAdvancedMask = 0;
	for(wchar_t letter: password){
		sizeAdvancedMask++;

		if (letter >= '0' && letter <= '9') {
			c.pol.digit++;
			c.advancedmask_string += "?d";
			if (last_simplemask != 'd') {
				sizeSimpleMask++;
				c.simplemask_string += "digit";
				last_simplemask = 'd';
			}
		}
		else if(letter >= 'a' && letter <= 'z') {
			c.pol.lower++;
			c.advancedmask_string += "?l";
			if (last_simplemask != 'l') {
				sizeSimpleMask++;
				c.simplemask_string += "lower";
				last_simplemask = 'l';
			}
		}
		else if(letter >= 'A' && letter <= 'Z') {
			c.pol.upper++;
			c.advancedmask_string += "?u";
			if (last_simplemask != 'u') {
				sizeSimpleMask++;
				c.simplemask_string += "upper";
				last_simplemask = 'u';
			}
		}
		else {
			c.pol.special++;
			c.advancedmask_string += "?s";

			if (last_simplemask != 's') {
				sizeSimpleMask++;
				c.simplemask_string += "special";
				last_simplemask = 's';
			}
		}
	}
	return make_pair(sizeSimpleMask, sizeAdvancedMask);
}

void handle_password(const string& password, const uint64_t& nbPasswords, ThreadData* my_data){
	my_data->total_counter += nbPasswords;
	if(my_data->use_regex && !regex_match(password,my_data->current_regex)){
		return;
	}
	PasswordStats c;

	pair<uint, uint> masks = get_masks(password, c);

	if(c.pol.satisfies(my_data->sr, password.size())){
		my_data->sr.nbSecurePassword++;
	}

	if (masks.first > my_data->limitSimplemask) {
		c.simplemask_string = "othermasks";
	}
	if (masks.second > my_data->limitAdvancedmask) {
		c.advancedmask_string = "othermasks";
	}

	my_data->total_filter += nbPasswords;
	my_data->length[ password.size() ] += nbPasswords;
	my_data->charactersets[ c.pol ] += nbPasswords;
	my_data->simplemasks[ c.simplemask_string ] += nbPasswords;
	my_data->advancedmasks[ c.advancedmask_string ] += nbPasswords;
	my_data->minMaxValue.updateMinMax(c.pol);
}

void* generate_stats_thread(void* threadarg) {
	ThreadData* my_data = (ThreadData *) threadarg;

	ifstream readfile(my_data->filename);
	uint64_t nbline = 0;
	string line;
	string password;
	uint64_t nbPasswords;

	while(getline(readfile, line)){
		++nbline;
		if (nbline < my_data->lineBegin){ continue; }
		if (nbline > my_data->lineEnd){ break; }
		if (line.size() == 0){ continue; }

		if (my_data->withcount) {
			istringstream is(line);
			is >> nbPasswords;
			is >> password;
		}
		else {
			nbPasswords = 1;
			password = line;
		}
		handle_password(password, nbPasswords, my_data);
	}

	readfile.close();
	pthread_exit(NULL);
}

bool Statsgen::operator==(const Statsgen& o) const {
	return results == o.results;
}

bool SecurityRules::operator==(const SecurityRules& o) const {
	return nbSecurePassword == o.nbSecurePassword
	&& minLength == o.minLength
	&& minSpecial == o.minSpecial
	&& minDigit == o.minDigit
	&& minLower == o.minLower
	&& minUpper == o.minUpper
	;
}