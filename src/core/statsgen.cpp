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
#include <vector>
#include <thread>
#include <pthread.h>
#include <cstdarg>


#include "statsgen.h"
#include "utils.h"
using namespace std;


Statsgen::Statsgen(const std::string& name) {
	if (name == "-"){
		is_stdin = true;
		cerr << "[WARNING]" << " reading from stdin enabled, loading the whole list in memory" << endl;
	}
	else {
		filename = name;
	}
}


void Statsgen::askSecurityRules() {
	wcout << "\nMinimal length of a password:" << endl;
	cin >> minLength;

	wcout << "\nMinimum of special characters in a password:" << endl;
	cin >> minSpecial;

	wcout << "\nMinimum of digits in a password:" << endl;
	cin >> minDigit;

	wcout << "\nMinimum of lower characters in a password:" << endl;
	cin >> minLower;

	wcout << "\nMinimum of upper characters in a password:" << endl;
	cin >> minUpper;
}

void Statsgen::setSecurityRules(const int& length, const int& special, const int& digit, const int& upper, const int& lower) {
    minLength = length;
    minSpecial = special;
    minDigit = digit;
    minUpper = upper;
    minLower = lower;
}



int Statsgen::generate_stats() {
	uint64_t nbline = 0;
	if (!is_stdin){
		nbline = nbline_file(filename);
		if (!nbline){ // error reading the file
			return 0;
		}
	}

	int rc;
	int i;
	pthread_t threads[MAX_THREADS];
	struct thread_data td[MAX_THREADS];

	pthread_attr_t attr;
	void *status;

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

	for( i = 0; i < nbThread; i++ ) {
		td[i].filename = filename;
		td[i].thread_id = i + 1;
		td[i].current_regex = current_regex;
		td[i].use_regex = use_regex;
		td[i].withcount = withcount;
		td[i].limitSimplemask = limitSimplemask;
		td[i].limitAdvancedmask = limitAdvancedmask;

		td[i].lineBegin = i*(nbline/nbThread) + 1;
		td[i].lineEnd = (i+1)*nbline/nbThread;
		if (i > 0) {
			while (td[i].lineBegin <= td[i-1].lineEnd) {
				td[i].lineBegin++;
			}
		}


		td[i].sr.nbSecurePassword = 0;
		td[i].sr.minLength = minLength;
		td[i].sr.minSpecial = minSpecial;
		td[i].sr.minDigit = minDigit;
		td[i].sr.minLower = minLower;
		td[i].sr.minUpper = minUpper;

		if(debug_enabled)
			cerr << "[DEBUG]" << "Thread" << td[i].thread_id << "analyse :" << td[i].lineBegin << " -->" << td[i].lineEnd << endl;
		// We use std::queue if input is from stdin
		if (is_stdin) {
			rc = pthread_create(&threads[i], NULL, generate_stats_thread_queue, (void *)&td[i] );
		}
		// Else we split the file in nbThread threads
		else {
			rc = pthread_create(&threads[i], NULL, generate_stats_thread, (void *)&td[i] );
		}

		if (rc) {
			cout << "Error:unable to create thread," << rc << endl;
			exit(-1);
		}
	}

	pthread_attr_destroy(&attr);
	for( i = 0; i < nbThread; i++ ) {
		rc = pthread_join(threads[i], &status);
		if (rc) {
			cout << "Error:unable to join," << rc << endl;
			exit(-1);
		}
	}


	Policy policyMin;
	Policy policyMax;

	for(i=0;i<nbThread;i++)	{
		total_counter+=td[i].total_counter;
		total_filter+=td[i].total_filter;

		policyMin.digit = td[i].minMaxValue.mindigit;
		policyMin.lower = td[i].minMaxValue.minlower;
		policyMin.upper = td[i].minMaxValue.minupper;
		policyMin.special = td[i].minMaxValue.minspecial;

		policyMax.digit = td[i].minMaxValue.maxdigit;
		policyMax.lower = td[i].minMaxValue.maxlower;
		policyMax.upper = td[i].minMaxValue.maxupper;
		policyMax.special = td[i].minMaxValue.maxspecial;

		nbSecurePassword += td[i].sr.nbSecurePassword;

		updateMinMax(minMaxValue, policyMin);
		updateMinMax(minMaxValue, policyMax);


		for(auto it=td[i].length.begin();it!=td[i].length.end();it++)
		{
			stats_length[it->first]+=it->second;
		}
		td[i].length.clear();
		for(auto it=td[i].charactersets.begin();it!=td[i].charactersets.end();it++)
		{
			stats_charactersets[it->first]+=it->second;
		}
		td[i].charactersets.clear();
		for(auto it=td[i].simplemasks.begin();it!=td[i].simplemasks.end();it++)
		{
			stats_simplemasks[it->first]+=it->second;
		}
		td[i].simplemasks.clear();
		for(auto it=td[i].advancedmasks.begin();it!=td[i].advancedmasks.end();it++)
		{
			stats_advancedmasks[it->first]+=it->second;
		}
		td[i].advancedmasks.clear();
	}



	if (!total_counter) {
		wcerr << "Empty file or not existing file" << endl;
		return 0;
	}

	return 1;
}


void Statsgen::print_stats() {
	int count;
	float perc = (float) 100 * (total_filter / total_counter);

	wcout << "\n\tSelected " << total_filter << " on " << total_counter << " passwords\t("
		<< perc << " %)" << endl;


	wcout << "\nSecurity rules : " << endl;
	wcout << "\tMinimal length of a password: " << minLength << endl;
	wcout << "\tMinimum of special characters in a password: " << minSpecial << endl;
	wcout << "\tMinimum of digits in a password: " << minDigit << endl;
	wcout << "\tMinimum of lower characters in a password: " << minLower << endl;
	wcout << "\tMinimum of upper characters in a password: " << minUpper << endl;

	float perce = (float) 100 * (nbSecurePassword / total_counter);
	wcout << "\n\t\t--> " << nbSecurePassword << " passwords\t(" << perce << " %) respect the security rules\n" << endl;


	wcout << "\nmin - max\n" << endl;
	wcout << setw(43) << right << "digit:  "
			<< setw(2) << right << minMaxValue.mindigit << " - " << minMaxValue.maxdigit << endl;
	wcout << setw(43) << right << "lower:  "
			<< setw(2) << right << minMaxValue.minlower << " - " << minMaxValue.maxlower << endl;
	wcout << setw(43) << right << "upper:  "
			<< setw(2) << right << minMaxValue.minupper << " - " << minMaxValue.maxupper << endl;
	wcout << setw(43) << right << "special:  "
			<< setw(2) << right << minMaxValue.minspecial << " - " << minMaxValue.maxspecial << endl;



	wcout << "\nStatistics relative to length: \n" << endl;
	showMap(stats_length, top, total_counter, hiderare, count);

	wcout << "\nStatistics relative to charsets: \n" << endl;
	showMap(stats_charactersets, -1, total_counter, hiderare, count);


	wcout << "\nStatistics relative to simplemasks: \n" << endl;
	showMap(stats_simplemasks, top, total_counter, hiderare, count);

	if (limitSimplemask > 0) {
		wcout << endl;
		readResult(stats_simplemasks["othermasks"], "othermasks", count, total_counter, hiderare);
	}


	wcout << "\nStatistics relative to advancedmask: \n" << endl;
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
		wcout << endl;
		readResult(stats_advancedmasks["othermasks"], "othermasks", count, total_counter, hiderare);
	}
}






void analyse_letter(const char & letter, char & last_simplemask, string & simplemask_string, string & advancedmask_string, Policy & policy, int & sizeAdvancedMask, int & sizeSimpleMask) {
	sizeAdvancedMask++;

	if (letter >= '0' && letter <= '9') {
		policy.digit++;
		advancedmask_string += "?d";
		if (last_simplemask != 'd') {
			sizeSimpleMask++;
			simplemask_string += "digit";
			last_simplemask = 'd';
		}
	}
	else if(letter >= 'a' && letter <= 'z') {
		policy.lower++;
		advancedmask_string += "?l";
		if (last_simplemask != 'l') {
			sizeSimpleMask++;
			simplemask_string += "lower";
			last_simplemask = 'l';
		}
	}
	else if(letter >= 'A' && letter <= 'Z') {
		policy.upper++;
		advancedmask_string += "?u";
		if (last_simplemask != 'u') {
			sizeSimpleMask++;
			simplemask_string += "upper";
			last_simplemask = 'u';
		}
	}
	else {
		policy.special++;
		advancedmask_string += "?s";

		if (last_simplemask != 's') {
			sizeSimpleMask++;
			simplemask_string += "special";
			last_simplemask = 's';
		}
	}
}


const string analyse_charset(const Policy & policy) {
	string charset;
	if (policy.digit && !policy.lower && !policy.upper && !policy.special) {
		charset = "numeric";
	}
	else if (!policy.digit && policy.lower && !policy.upper && !policy.special) {
		charset = "loweralpha";
	}
	else if (!policy.digit && !policy.lower && policy.upper && !policy.special) {
		charset = "upperalpha";
	}
	else if (!policy.digit && !policy.lower && !policy.upper && policy.special) {
		charset = "special";
	}
	else if (!policy.digit && policy.lower && policy.upper && !policy.special) {
		charset = "mixedalpha";
	}
	else if (policy.digit && policy.lower && !policy.upper && !policy.special) {
		charset = "loweralphanum";
	}
	else if (policy.digit && !policy.lower && policy.upper && !policy.special) {
		charset = "upperalphanum";
	}
	else if (!policy.digit && policy.lower && !policy.upper && policy.special) {
		charset = "loweralphaspecial";
	}
	else if (!policy.digit && !policy.lower && policy.upper && policy.special) {
		charset = "upperalphaspecial";
	}
	else if (policy.digit && !policy.lower && !policy.upper && policy.special) {
		charset = "specialnum";
	}

	else if (!policy.digit && policy.lower && policy.upper && policy.special) {
		charset = "mixedalphaspecial";
	}
	else if (policy.digit && !policy.lower && policy.upper && policy.special) {
		charset = "upperalphaspecialnum";
	}
	else if (policy.digit && policy.lower && !policy.upper && policy.special) {
		charset = "loweralphaspecialnum";
	}
	else if (policy.digit && policy.lower && policy.upper && !policy.special) {
		charset = "mixedalphanum";
	}

	else {
		charset = "all";
	}
	return charset;
}


Container analyze_password(const string & password, SecurityRules & sr, const int & limitAdvancedmask, const int & limitSimplemask) {
	Container c;
	c.pass_length = password.size();

	char last_simplemask = 'a';
	int sizeAdvancedMask = 0;
	int sizeSimpleMask = 0;

	for(wchar_t letter: password){
		analyse_letter(letter, last_simplemask, c.simplemask_string, c.advancedmask_string, c.pol, sizeAdvancedMask, sizeSimpleMask);
	}

	c.characterset = analyse_charset(c.pol);

	if ( (c.pass_length >= sr.minLength) &&
		 (c.pol.digit >= sr.minDigit) &&
		 (c.pol.lower >= sr.minLower) &&
		 (c.pol.upper >= sr.minUpper) &&
		 (c.pol.special >= sr.minSpecial) ) {
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

		Container c;

		my_data->total_counter++;
		if ( !my_data->use_regex || (my_data->use_regex && regex_match(line,my_data->current_regex)) ) {
			c = analyze_password(line, my_data->sr, my_data->limitAdvancedmask, my_data->limitSimplemask);

			my_data->total_filter++;

			my_data->length[ c.pass_length ] += 1;
			my_data->charactersets[ c.characterset ] += 1;
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

		Container c;

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
				my_data->charactersets[ c.characterset ] += nbPasswords;
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
				my_data->charactersets[ c.characterset ] += 1;
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
		cerr << "There was an error reading the file at line " << nb << endl;
		return 0;
	}

	return nb;
}