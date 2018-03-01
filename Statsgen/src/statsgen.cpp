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


#include "statsgen.h"
#include "utils.h"
using namespace std;


void Statsgen::setFilename(std::string name) {
	filename = name;
}


void Statsgen::setHiderare(int val) {
	hiderare = val;
}



void Statsgen::setTop(int val) {
	top = val;
}


void Statsgen::setRegex(string expr) {
	wstring tmp(expr.length(),L' ');
	copy(expr.begin(), expr.end(), tmp.begin());

	wregex tmp_reg(tmp);
	current_regex = tmp_reg;

	use_regex = true;
}


void Statsgen::setWithcount(bool val) {
	withcount = val;
}


void Statsgen::setLimitSimplemask(int val) {
	limitSimplemask = val;
}

void Statsgen::setLimitAdvancedmask(int val) {
	limitAdvancedmask = val;
}


void Statsgen::setNbThread(int nb) {
	int max = thread::hardware_concurrency();
	if (nb > max) {
		nbThread = max;
	} else {
		nbThread = nb;
	}
}

void Statsgen::setOutfile(string outfile){
	outfile_name = outfile;
}


void Statsgen::setSecurityRules() {
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



int Statsgen::generate_stats() {
	int nbline = nbline_file(filename);

	int rc;
	int i;
	pthread_t threads[nbThread];
	struct thread_data td[nbThread];

	pthread_attr_t attr;
	void *status;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

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

		wcout << "Thread " << td[i].thread_id << " analyse : " << td[i].lineBegin << " --> " << td[i].lineEnd << endl;
		rc = pthread_create(&threads[i], NULL, generate_stats_thread, (void *)&td[i] );
	  
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
		readResult(stats_simplemasks[L"othermasks"], L"othermasks", count, total_counter, hiderare);
	}


	wcout << "\nStatistics relative to advancedmask: \n" << endl;
	showMap(stats_advancedmasks, top, total_counter, hiderare, count);

	if (outfile_name != ""){
		locale::global(locale("C"));
		wofstream outfile_stream(outfile_name);
		multimap<double, wstring> reverse = flip_map<wstring>(stats_advancedmasks);
		for(auto it=reverse.end();it!=reverse.begin();it--){
			if (it == reverse.end()) continue;
			if(it->second == L"othermasks") continue;
			outfile_stream << it->second << "," << it->first << endl;
		}
		outfile_stream.close();
	}
	
	if (limitAdvancedmask > 0) {
		wcout << endl;
		readResult(stats_advancedmasks[L"othermasks"], L"othermasks", count, total_counter, hiderare);
	}
}






void analyse_letter(const char & letter, char & last_simplemask, wstring & simplemask_string, wstring & advancedmask_string, Policy & policy, int & sizeAdvancedMask, int & sizeSimpleMask) {
	sizeAdvancedMask++;

	if (letter >= L'0' && letter <= L'9') {
		policy.digit++;
		advancedmask_string += L"?d";
		if (last_simplemask != 'd') {
			sizeSimpleMask++;
			simplemask_string += L"digit";
			last_simplemask = 'd';
		}
	}
	else if(letter >= L'a' && letter <= L'z') {
		policy.lower++;
		advancedmask_string += L"?l";
		if (last_simplemask != 'l') {
			sizeSimpleMask++;
			simplemask_string += L"lower";
			last_simplemask = 'l';
		}
	}
	else if(letter >= L'A' && letter <= L'Z') {
		policy.upper++;
		advancedmask_string += L"?u";
		if (last_simplemask != 'u') {
			sizeSimpleMask++;
			simplemask_string += L"upper";
			last_simplemask = 'u';
		}
	}
	else {
		policy.special++;
		advancedmask_string += L"?s";

		if (last_simplemask != 's') {
			sizeSimpleMask++;
			simplemask_string += L"special";
			last_simplemask = 's';
		}
	}
}


void analyse_charset(wstring & charset, const Policy & policy) {
	if (policy.digit && !policy.lower && !policy.upper && !policy.special) {
		charset = L"numeric";
	}
	else if (!policy.digit && policy.lower && !policy.upper && !policy.special) {
		charset = L"loweralpha";
	}
	else if (!policy.digit && !policy.lower && policy.upper && !policy.special) {
		charset = L"upperalpha";
	}
	else if (!policy.digit && !policy.lower && !policy.upper && policy.special) {
		charset = L"special";
	}
	else if (!policy.digit && policy.lower && policy.upper && !policy.special) {
		charset = L"mixedalpha";
	}
	else if (policy.digit && policy.lower && !policy.upper && !policy.special) {
		charset = L"loweralphanum";
	}
	else if (policy.digit && !policy.lower && policy.upper && !policy.special) {
		charset = L"upperalphanum";
	}
	else if (!policy.digit && policy.lower && !policy.upper && policy.special) {
		charset = L"loweralphaspecial";
	}
	else if (!policy.digit && !policy.lower && policy.upper && policy.special) {
		charset = L"upperalphaspecial";
	}
	else if (policy.digit && !policy.lower && !policy.upper && policy.special) {
		charset = L"specialnum";
	}

	else if (!policy.digit && policy.lower && policy.upper && policy.special) {
		charset = L"mixedalphaspecial";
	}
	else if (policy.digit && !policy.lower && policy.upper && policy.special) {
		charset = L"upperalphaspecialnum";
	}
	else if (policy.digit && policy.lower && !policy.upper && policy.special) {
		charset = L"loweralphaspecialnum";
	}
	else if (policy.digit && policy.lower && policy.upper && !policy.special) {
		charset = L"mixedalphanum";
	}

	else {
		charset = L"all";
	}
}


void analyze_password(const wstring & password, Container & c, SecurityRules & sr, const int & limitAdvancedmask, const int & limitSimplemask) {
	c.pass_length = password.size();

	char last_simplemask = 'a';
	int sizeAdvancedMask = 0;
	int sizeSimpleMask = 0;
	wchar_t letter;

	for (int i=0; i < c.pass_length; i++) {
		letter = password[i];
		analyse_letter(letter, last_simplemask, c.simplemask_string, c.advancedmask_string, c.pol, sizeAdvancedMask, sizeSimpleMask);
	}

	analyse_charset(c.characterset, c.pol);

	if ( (c.pass_length >= sr.minLength) && 
		 (c.pol.digit >= sr.minDigit) && 
		 (c.pol.lower >= sr.minLower) && 
		 (c.pol.upper >= sr.minUpper) && 
		 (c.pol.special >= sr.minSpecial) ) {
		sr.nbSecurePassword++;
	}

	if (sizeAdvancedMask > limitAdvancedmask) {
		c.advancedmask_string = L"othermasks";
	}

	if (sizeSimpleMask > limitSimplemask) {
		c.simplemask_string = L"othermasks";
	}
}



void updateMinMax(minMax & minMaxValue, const Policy & pol) {
	if (minMaxValue.mindigit == -1 || minMaxValue.mindigit > pol.digit) {
		minMaxValue.mindigit = pol.digit;
	}
	if (minMaxValue.maxdigit == -1 || minMaxValue.maxdigit < pol.digit) {
		minMaxValue.maxdigit = pol.digit;
	}

	if (minMaxValue.minlower == -1 || minMaxValue.minlower > pol.lower) {
		minMaxValue.minlower = pol.lower;
	}
	if (minMaxValue.maxlower == -1 || minMaxValue.maxlower < pol.lower) {
		minMaxValue.maxlower = pol.lower;
	}

	if (minMaxValue.minupper == -1 || minMaxValue.minupper > pol.upper) {
		minMaxValue.minupper = pol.upper;
	}
	if (minMaxValue.maxupper == -1 || minMaxValue.maxupper < pol.upper) {
		minMaxValue.maxupper = pol.upper;
	}

	if (minMaxValue.minspecial == -1 || minMaxValue.minspecial > pol.special) {
		minMaxValue.minspecial = pol.special;
	}
	if (minMaxValue.maxspecial == -1 || minMaxValue.maxspecial < pol.special) {
		minMaxValue.maxspecial = pol.special;
	}
}



void * generate_stats_thread(void * threadarg) {
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;

	wifstream readfile(my_data->filename);
	wstring line;
	int nbline = 0;

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
			uint i = 0;
			bool number=false;
			for(i=0; i < line.length(); i++) {
				if(iswdigit(line.at(i))) {
					number=true;
				}
				else if (!iswdigit(line.at(i)) && number) {
					break;
				}
			}
			wstring password = line.substr(i+1,line.length());
			int nbPasswords = stoi(line.substr(0,i));
			
			my_data->total_counter += nbPasswords;

			if ( !my_data->use_regex || (my_data->use_regex && regex_match(password,my_data->current_regex)) ) {
				my_data->total_filter += nbPasswords;
				analyze_password(password, c, my_data->sr,my_data->limitAdvancedmask, my_data->limitSimplemask);

				my_data->length[ c.pass_length ] += nbPasswords;
				my_data->charactersets[ c.characterset ] += nbPasswords;
				my_data->simplemasks[ c.simplemask_string ] += nbPasswords;
				my_data->advancedmasks[ c.advancedmask_string ] += nbPasswords;
			}
		}
		else {
			my_data->total_counter++;
			if ( !my_data->use_regex || (my_data->use_regex && regex_match(line,my_data->current_regex)) ) {
				analyze_password(line, c, my_data->sr, my_data->limitAdvancedmask, my_data->limitSimplemask);
				
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

