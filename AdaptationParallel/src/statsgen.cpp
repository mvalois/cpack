/**
 * @file statsgen.cpp
 * @brief Creation of all methods from Statsgen
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
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




void Statsgen::showHelp() {
    wcout << "\nUsage: database.txt [options]\n" << endl;

    wcout << "Options:" << endl;
    wcout << "\t--help, -h\t\t:\tShow this help message" << endl;
    wcout << "\t--withcount, -w\t\t:\tMendatory if the input database has the following format : [number of occurence] [password]\n" << endl;

    wcout << "\t--hiderare, -hr\t\t:\tHide all statistics below 1%" << endl;
    wcout << "\t--top, -t [value]\t:\tShow only [value] first results" << endl;
    wcout << "\t--regex, -r [value]\t:\tShow result for password, using the regular expression [value]" << endl;

    wcout << "\n\nOptimisation options to reduce the execution time : " << endl;
    wcout << "\t--limitadvancedmasks, -lam [value]\t:\tLimit the size of the advanced masks at [value], if size>[value]: othermasks" << endl;
    wcout << "\t--limitsimplemasks, -lsm [value]\t:\tLimit the size of the simple masks at [value], if size>[value]: othermasks" << endl;
    wcout << "\t--parallel, -p [value]\t\t\t:\tNumber of usable threads" << endl;
    
    wcout << "\n" << endl;
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



int Statsgen::generate_stats(const string & filename) {
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

        td[i].lineBegin = i*(nbline/nbThread) + 1;
        td[i].lineEnd = (i+1)*nbline/nbThread;

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

        updateMinMax(minMaxValue, policyMin);
        updateMinMax(minMaxValue, policyMax);

		
		for(auto it=td[i].length.begin();it!=td[i].length.end();it++)
		{
			stats_length[it->first]+=it->second;
		}
		for(auto it=td[i].charactersets.begin();it!=td[i].charactersets.end();it++)
		{
			stats_charactersets[it->first]+=it->second;
		}
		for(auto it=td[i].simplemasks.begin();it!=td[i].simplemasks.end();it++)
		{
			stats_simplemasks[it->first]+=it->second;
		}
		for(auto it=td[i].advancedmasks.begin();it!=td[i].advancedmasks.end();it++)
		{
			stats_advancedmasks[it->first]+=it->second;
		}
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
    showMap(stats_charactersets, top, total_counter, hiderare, count);


    wcout << "\nStatistics relative to simplemasks: \n" << endl;
    showMap(stats_simplemasks, top, total_counter, hiderare, count);

    if (limitSimplemask > 0) {
        wcout << endl;
        readResult(stats_simplemasks[L"othermasks"], L"othermasks", count, total_counter, hiderare);
    }


    wcout << "\nStatistics relative to advancedmask: \n" << endl;
    showMap(stats_advancedmasks, top, total_counter, hiderare, count);
    
    if (limitAdvancedmask > 0) {
        wcout << endl;
        readResult(stats_advancedmasks[L"othermasks"], L"othermasks", count, total_counter, hiderare);
    }
}
