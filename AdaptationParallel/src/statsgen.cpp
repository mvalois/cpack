#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <regex>
#include <pthread.h>

#include "statsgen.h"
#include "utils.h"
using namespace std;



#define NB_THREAD 4


/**********************************************************************
 *                           INITIALISATION                           *
 **********************************************************************/


void Statsgen::showHelp() {
    wcout << "\nUsage: database.txt [options]\n" << endl;

    wcout << "Options:" << endl;
    wcout << "\t--help, -h\t\t:\tShow this help message" << endl;
    wcout << "\t--withcount, -w\t\t:\tMendatory if the input database has the following format : [number of occurence] [password]\n" << endl;

    wcout << "\t--hiderare, -hr\t\t:\tHide all statistics below 1%" << endl;
    wcout << "\t--top, -t [value]\t:\tShow only [value] first results" << endl;
    wcout << "\t--regex, -r [value]\t\t:\tShow result for password, using the regular expression [value]" << endl;

    wcout << "\n\nOptimisation options to reduce the execution time : " << endl;
    wcout << "\t--limitadvancedmasks, -lam [value]\t:\tLimit the size of the advanced masks at [value], if size>[value]: othermasks" << endl;
    wcout << "\t--limitsimplemasks, -lsm [value]\t:\tLimit the size of the simple masks at [value], if size>[value]: othermasks" << endl;
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



/**********************************************************************
 *                          ANALYSE PASSWORD                          *
 **********************************************************************/


void analyse_letter(const char & letter, char & last_simplemask, wstring & simplemask_string, wstring & advancedmask_string, Policy & policy, int & sizeAM, int & sizeSM) {
    sizeAM++;

    if (letter >= L'0' && letter <= L'9') {
        policy.digit++;
        advancedmask_string += L"?d";
        if (last_simplemask != 'd') {
            sizeSM++;
            simplemask_string += L"digit";
            last_simplemask = 'd';
        }
    }
    else if(letter >= L'a' && letter <= L'z') {
        policy.lower++;
        advancedmask_string += L"?l";
        if (last_simplemask != 'l') {
            sizeSM++;
            simplemask_string += L"lower";
            last_simplemask = 'l';
        }
    }
    else if(letter >= L'A' && letter <= L'Z') {
        policy.upper++;
        advancedmask_string += L"?u";
        if (last_simplemask != 'u') {
            sizeSM++;
            simplemask_string += L"upper";
            last_simplemask = 'u';
        }
    }
    else {
        policy.special++;
        advancedmask_string += L"?s";

        if (last_simplemask != 's') {
            sizeSM++;
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


void analyze_password(const wstring & password, Conteneur & c) {
    c.pass_length = password.size();

    char last_simplemask = 'a';
    int sizeAM = 0;
    int sizeSM = 0;
    wchar_t letter;

    for (int i=0; i < c.pass_length; i++) {
    	letter = password[i];
        analyse_letter(letter, last_simplemask, c.simplemask_string, c.advancedmask_string, c.pol, sizeAM, sizeSM);
    }

    analyse_charset(c.characterset, c.pol);
}





/**********************************************************************
 *                        ANALYSE STATISTIQUE                         *
 **********************************************************************/



void updateMinMax(int & mindigit, int & maxdigit, int & minlower, int & maxlower, int & minupper, int & maxupper, int & minspecial, int & maxspecial, const Policy & pol) {
    if (mindigit == -1 || mindigit > pol.digit) {
        mindigit = pol.digit;
    }
    if (maxdigit == -1 || maxdigit < pol.digit) {
        maxdigit = pol.digit;
    }

    if (minlower == -1 || minlower > pol.lower) {
        minlower = pol.lower;
    }
    if (maxlower == -1 || maxlower < pol.lower) {
        maxlower = pol.lower;
    }

    if (minupper == -1 || minupper > pol.upper) {
        minupper = pol.upper;
    }
    if (maxupper == -1 || maxupper < pol.upper) {
        maxupper = pol.upper;
    }

    if (minspecial == -1 || minspecial > pol.special) {
        minspecial = pol.special;
    }
    if (maxspecial == -1 || maxspecial < pol.special) {
        maxspecial = pol.special;
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
        if (nbline >= my_data->lineEnd) {
            break;
        }


        if (line.size() == 0) {
            wcout << "Error empty password, line " << my_data->total_counter << endl;
            continue;
        }

        Conteneur c;

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
                analyze_password(password, c);

                my_data->length[ c.pass_length ] += nbPasswords;
                my_data->charactersets[ c.characterset ] += nbPasswords;
                my_data->simplemasks[ c.simplemask_string ] += nbPasswords;
                my_data->advancedmasks[ c.advancedmask_string ] += nbPasswords;
            }
        }
        else {
            my_data->total_counter++;
            if ( !my_data->use_regex || (my_data->use_regex && regex_match(line,my_data->current_regex)) ) {
                analyze_password(line, c);
                
                my_data->total_filter++;

                my_data->length[ c.pass_length ] += 1;
                my_data->charactersets[ c.characterset ] += 1;
                my_data->simplemasks[ c.simplemask_string ] += 1;
                my_data->advancedmasks[ c.advancedmask_string ] += 1;
            }
        }
         updateMinMax(my_data->mindigit, my_data->maxdigit, my_data->minlower, my_data->maxlower, my_data->minupper, my_data->maxupper, my_data->minspecial, my_data->maxspecial, c.pol);
    }

    readfile.close();

    pthread_exit(NULL);
}


int nbline_file(const string & filename) {
    wifstream readfile(filename);
    wstring line;
    int nb = 0;

    while(readfile.good()) {
        getline(readfile, line);
        ++nb;
    }

    return nb;
}





int Statsgen::generate_stats(const string & filename) {
    int nbline = nbline_file(filename);

    int rc;
    int i;
    pthread_t threads[NB_THREAD];
    struct thread_data td[NB_THREAD];

    pthread_attr_t attr;
    void *status;

   // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for( i = 0; i < NB_THREAD; i++ ) {
        td[i].filename = filename;
        td[i].thread_id = i;
        td[i].current_regex = current_regex;
        td[i].use_regex = use_regex;
        td[i].withcount = withcount;

        td[i].lineBegin = i*(nbline/NB_THREAD) + 1;
        td[i].lineEnd = (i+1)*nbline/NB_THREAD;

        wcout << "Thread " << i << " analyse : " << td[i].lineBegin << " --> " << td[i].lineEnd << endl;
        rc = pthread_create(&threads[i], NULL, generate_stats_thread, (void *)&td[i] );
      
        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }

    // free attribute and wait for the other threads
    pthread_attr_destroy(&attr);
    for( i = 0; i < NB_THREAD; i++ ) {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            cout << "Error:unable to join," << rc << endl;
            exit(-1);
        }
    }

	for(i=0;i<NB_THREAD;i++)
	{
		total_counter+=td[i].total_counter;
		total_filter+=td[i].total_filter;
		
		if(mindigit == -1 || mindigit>td[i].mindigit)
		{
			mindigit=td[i].mindigit;
		}
		if(minlower == -1 || minlower>td[i].minlower)
		{
			minlower=td[i].minlower;
		}
		if(minupper == -1 || minupper>td[i].minupper)
		{
			minupper=td[i].minupper;
		}
		if(minspecial == -1 || minspecial>td[i].minspecial)
		{
			minspecial=td[i].minspecial;
		}
		if(maxdigit<td[i].maxdigit)
		{
			maxdigit=td[i].maxdigit;
		}
		if(maxlower<td[i].maxlower)
		{
			maxlower=td[i].maxlower;
		}
		if(maxupper<td[i].maxupper)
		{
			maxupper=td[i].maxupper;
		}
		if(maxspecial<td[i].maxspecial)
		{
			maxspecial=td[i].maxspecial;
		}
		
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






/**********************************************************************
 *                         PRINT STATISTIQUE                          *
 **********************************************************************/



void Statsgen::print_stats() {
    int count;
    float perc = (float) 100 * (total_filter / total_counter);

    wcout << "\n\tSelected " << total_filter << " on " << total_counter << " passwords\t(" 
        << perc << " %)" << endl;



    wcout << "\nmin - max\n" << endl;
    wcout << setw(43) << right << "digit:  " 
            << setw(2) << right << mindigit << " - " << maxdigit << endl;
    wcout << setw(43) << right << "lower:  " 
            << setw(2) << right << minlower << " - " << maxlower << endl;
    wcout << setw(43) << right << "upper:  " 
            << setw(2) << right << minupper << " - " << maxupper << endl;
    wcout << setw(43) << right << "special:  " 
            << setw(2) << right << minspecial << " - " << maxspecial << endl;       



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
