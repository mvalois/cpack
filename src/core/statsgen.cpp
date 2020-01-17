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


#include "statsgen.h"
#include "utils.h"
using namespace std;

Statsgen::Statsgen(const std::string& name) {
	if (name == "-"){
		is_stdin = true;
		cerr << "[WARNING]" << " reading from stdin enabled, loading the whole list in memory" << endl;
	}
	filename = name;
}


void Statsgen::askSecurityRules() {
	int length, special, digit, upper, lower;
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

void Statsgen::setSecurityRules(const int& length, const int& special, const int& digit, const int& upper, const int& lower) {
	_sr = { _sr.nbSecurePassword, length, special, digit, upper, lower };
}

void Statsgen::configureThread(thread_data& td) const {
	td.filename = filename;
	td.current_regex = current_regex;
	td.use_regex = use_regex;
	td.withcount = withcount;
	td.limitSimplemask = limitSimplemask;
	td.limitAdvancedmask = limitAdvancedmask;
	td.sr = { 0, _sr.minLength, _sr.minSpecial, _sr.minDigit, _sr.minLower, _sr.minUpper };
}

void Statsgen::mergeThread(const thread_data& td){
	total_counter += td.total_counter;
	total_filter += td.total_filter;

	Policy min, max;
	min.digit = td.minMaxValue.mindigit;
	min.lower = td.minMaxValue.minlower;
	min.upper = td.minMaxValue.minupper;
	min.special = td.minMaxValue.minspecial;

	max.digit = td.minMaxValue.maxdigit;
	max.lower = td.minMaxValue.maxlower;
	max.upper = td.minMaxValue.maxupper;
	max.special = td.minMaxValue.maxspecial;

	_sr.nbSecurePassword += td.sr.nbSecurePassword;

	updateMinMax(minMaxValue, min);
	updateMinMax(minMaxValue, max);


	for(pair<int, uint64_t> occ: td.length){
		stats_length[occ.first] += occ.second;
	}

	for(pair<string, int> occ : td.charactersets){
		stats_charactersets[occ.first] += occ.second;
	}

	for(pair<string, int> occ: td.simplemasks){
		stats_simplemasks[occ.first] += occ.second;
	}

	for(pair<string, int> occ: td.advancedmasks){
		stats_advancedmasks[occ.first] += occ.second;
	}
}

int Statsgen::generate_stats() {
	uint64_t nbline = 0;
	if (!is_stdin){
		nbline = nbline_file(filename);
		if (!nbline){ // error reading the file
			cerr << "[ERROR] Empty file or not existing file" << endl;
			return 0;
		}
	}

	pthread_t threads[MAX_THREADS];
	struct thread_data td[MAX_THREADS];

	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// split stdin into nbThread files on disk
	if (is_stdin){
		string line;
		nbline = 0;
		while(getline(cin, line)){
			td[nbline%nbThread].password_queue.push(line);
			nbline++;
		}
	}

	for(int i = 0; i < nbThread; i++ ) {
		td[i].thread_id = i + 1;
		configureThread(td[i]);

		td[i].lineBegin = i*(nbline/nbThread) + 1;
		td[i].lineEnd = (i+1)*nbline/nbThread;
		if (i > 0) {
			while (td[i].lineBegin <= td[i-1].lineEnd) {
				td[i].lineBegin++;
			}
		}

		if(debug_enabled){
			cerr << "[DEBUG]" << "Thread" << td[i].thread_id << "analyse :" << td[i].lineBegin << " -->" << td[i].lineEnd << endl;
		}

		int rc;
		// We use std::queue if input is from stdin
		if (is_stdin) {
			rc = pthread_create(&threads[i], NULL, generate_stats_thread_queue, (void *)&td[i] );
		}
		// Else we split the file in nbThread threads
		else {
			rc = pthread_create(&threads[i], NULL, generate_stats_thread, (void *)&td[i] );
		}

		if (rc) {
			cerr << "[ERROR] unable to create thread," << rc << endl;
			exit(-1);
		}
	}

	void *status;
	pthread_attr_destroy(&attr);
	for(int i = 0; i < nbThread; i++ ) {
		int rc = pthread_join(threads[i], &status);
		if (rc) {
			cerr << "[ERROR] unable to join," << rc << endl;
			exit(-1);
		}
	}

	for(int i=0; i < nbThread; i++)	{
		mergeThread(td[i]);
	}

	if (!total_counter) {
		cerr << "[ERROR] Empty file or not existing file" << endl;
		return 0;
	}

	return 1;
}


void Statsgen::print_stats() {
	int count;
	float perc = percentage(total_filter, total_counter);

	cout << "\n\tSelected " << total_filter << " on " << total_counter << " passwords\t("
		<< perc << " %)" << endl;


	cout << "\nSecurity rules : " << endl;
	cout << "\tMinimal length of a password: " << _sr.minLength << endl;
	cout << "\tMinimum of special characters in a password: " << _sr.minSpecial << endl;
	cout << "\tMinimum of digits in a password: " << _sr.minDigit << endl;
	cout << "\tMinimum of lower characters in a password: " << _sr.minLower << endl;
	cout << "\tMinimum of upper characters in a password: " << _sr.minUpper << endl;

	float perce = percentage(_sr.nbSecurePassword, total_counter);
	cout << "\n\t\t--> " << _sr.nbSecurePassword << " passwords\t(" << perce << " %) respect the security rules\n" << endl;


	cout << "\nmin - max\n" << endl;
	cout << setw(43) << right << "digit:  "
			<< setw(2) << right << minMaxValue.mindigit << " - " << minMaxValue.maxdigit << endl;
	cout << setw(43) << right << "lower:  "
			<< setw(2) << right << minMaxValue.minlower << " - " << minMaxValue.maxlower << endl;
	cout << setw(43) << right << "upper:  "
			<< setw(2) << right << minMaxValue.minupper << " - " << minMaxValue.maxupper << endl;
	cout << setw(43) << right << "special:  "
			<< setw(2) << right << minMaxValue.minspecial << " - " << minMaxValue.maxspecial << endl;



	cout << "\nStatistics relative to length: \n" << endl;
	showMap(stats_length, top, total_counter, hiderare, count);

	cout << "\nStatistics relative to charsets: \n" << endl;
	showMap(stats_charactersets, -1, total_counter, hiderare, count);


	cout << "\nStatistics relative to simplemasks: \n" << endl;
	showMap(stats_simplemasks, top, total_counter, hiderare, count);

	if (limitSimplemask > 0) {
		cout << endl;
		readResult(stats_simplemasks["othermasks"], "othermasks", count, total_counter, hiderare);
	}


	cout << "\nStatistics relative to advancedmask: \n" << endl;
	showMap(stats_advancedmasks, top, total_counter, hiderare, count);

	if (outfile_name != ""){
		locale::global(locale("C"));
		ofstream outfile_stream(outfile_name);
		multimap<uint64_t, string> reverse = flip_map<string>(stats_advancedmasks);
		for(auto it=reverse.end();it!=reverse.begin();it--){
			if (it == reverse.end()) continue;
			if(it->second == "othermasks") continue;
			outfile_stream << it->second << "," << it->first << endl;
		}
		outfile_stream.close();
	}

	if (limitAdvancedmask > 0) {
		cout << endl;
		readResult(stats_advancedmasks["othermasks"], "othermasks", count, total_counter, hiderare);
	}
}


void analyse_letter(const char & letter, PasswordStats& c, char & last_simplemask, int & sizeAdvancedMask, int & sizeSimpleMask) {
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

PasswordStats analyze_password(const string & password, SecurityRules & sr, const int & limitAdvancedmask, const int & limitSimplemask) {
	PasswordStats c;
	c.pass_length = password.size();

	char last_simplemask = 'a';
	int sizeAdvancedMask = 0;
	int sizeSimpleMask = 0;

	for(wchar_t letter: password){
		analyse_letter(letter, c, last_simplemask, sizeAdvancedMask, sizeSimpleMask);
	}

	if(c.pol.satisfies(sr, password.size())){
		sr.nbSecurePassword++;
	}

	if (sizeAdvancedMask > limitAdvancedmask) {
		c.advancedmask_string = "othermasks";
	}

	if (sizeSimpleMask > limitSimplemask) {
		c.simplemask_string = "othermasks";
	}

	return c;
}



void updateMinMax(minMax & m, const Policy & pol) {
	if (m.mindigit == -1 || m.mindigit > pol.digit) {
		m.mindigit = pol.digit;
	}
	if (m.maxdigit == -1 || m.maxdigit < pol.digit) {
		m.maxdigit = pol.digit;
	}

	if (m.minlower == -1 || m.minlower > pol.lower) {
		m.minlower = pol.lower;
	}
	if (m.maxlower == -1 || m.maxlower < pol.lower) {
		m.maxlower = pol.lower;
	}

	if (m.minupper == -1 || m.minupper > pol.upper) {
		m.minupper = pol.upper;
	}
	if (m.maxupper == -1 || m.maxupper < pol.upper) {
		m.maxupper = pol.upper;
	}

	if (m.minspecial == -1 || m.minspecial > pol.special) {
		m.minspecial = pol.special;
	}
	if (m.maxspecial == -1 || m.maxspecial < pol.special) {
		m.maxspecial = pol.special;
	}
}


void * generate_stats_thread_queue(void * threadarg) {
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;

	string line;
	uint64_t nbline = 0;
	while(!my_data->password_queue.empty()) {
		++nbline;
		line = my_data->password_queue.front();
		my_data->password_queue.pop();

		if (line.size() == 0) {
			continue;
		}

		PasswordStats c;

		my_data->total_counter++;
		if ( !my_data->use_regex || (my_data->use_regex && regex_match(line,my_data->current_regex)) ) {
			c = analyze_password(line, my_data->sr, my_data->limitAdvancedmask, my_data->limitSimplemask);

			my_data->total_filter++;

			my_data->length[ c.pass_length ] += 1;
			my_data->charactersets[ c.pol ] += 1;
			my_data->simplemasks[ c.simplemask_string ] += 1;
			my_data->advancedmasks[ c.advancedmask_string ] += 1;
		}
		updateMinMax(my_data->minMaxValue, c.pol);
	}

	pthread_exit(NULL);
}


void * generate_stats_thread(void * threadarg) {
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;

	ifstream readfile(my_data->filename);
	string line;
	uint64_t nbline = 0;

	while(readfile.good()) {
		++nbline;
		getline(readfile, line);
		if (nbline < my_data->lineBegin) {
			continue;
		}
		if (nbline > my_data->lineEnd) {
			break;
		}


		if (line.size() == 0) {
			continue;
		}

		PasswordStats c;

		if (my_data->withcount) {
			uint64_t i = 0;
			bool number=false;
			for(i=0; i < line.length(); i++) {
				if(iswdigit(line.at(i))) {
					number=true;
				}
				else if (!iswdigit(line.at(i)) && number) {
					break;
				}
			}
			string password = line.substr(i+1,line.length());
			uint64_t nbPasswords = stoi(line.substr(0,i));

			my_data->total_counter += nbPasswords;

			if ( !my_data->use_regex || (my_data->use_regex && regex_match(password,my_data->current_regex)) ) {
				my_data->total_filter += nbPasswords;
				c = analyze_password(password, my_data->sr,my_data->limitAdvancedmask, my_data->limitSimplemask);

				my_data->length[ c.pass_length ] += nbPasswords;
				my_data->charactersets[ c.pol ] += nbPasswords;
				my_data->simplemasks[ c.simplemask_string ] += nbPasswords;
				my_data->advancedmasks[ c.advancedmask_string ] += nbPasswords;
			}
		}
		else {
			my_data->total_counter++;
			if ( !my_data->use_regex || (my_data->use_regex && regex_match(line,my_data->current_regex)) ) {
				c = analyze_password(line, my_data->sr, my_data->limitAdvancedmask, my_data->limitSimplemask);

				my_data->total_filter++;

				my_data->length[ c.pass_length ] += 1;
				my_data->charactersets[ c.pol ] += 1;
				my_data->simplemasks[ c.simplemask_string ] += 1;
				my_data->advancedmasks[ c.advancedmask_string ] += 1;
			}
		}
		updateMinMax(my_data->minMaxValue, c.pol);
	}

	readfile.close();

	pthread_exit(NULL);
}


uint64_t nbline_file(const string & filename) {
	ifstream readfile(filename);
	string line;
	uint64_t nb = 0;

	while(readfile.good()) {
		getline(readfile, line);
		++nb;
	}
	// we have not read the whole file
	if (readfile.fail() && !readfile.eof()){
		cerr << "[ERROR]" << " There was an error reading the file at line " << nb << endl;
		return 0;
	}

	return nb;
}