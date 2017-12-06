#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <regex>

#include "statsgen.h"
#include "utils.h"
#include "ThreadPool.h"
using namespace std;



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


Conteneur analyze_password(const wstring & password) {
    Conteneur cont;
    cont.pass_length = password.size();
    
    char last_simplemask = 'a';
    int sizeAM = 0;
    int sizeSM = 0;
    wchar_t letter;

    for (int i=0; i<cont.pass_length; i++) {
    	letter = password[i];
        analyse_letter(letter, last_simplemask, cont.simplemask_string, cont.advancedmask_string, cont.pol, sizeAM, sizeSM);
    }

    analyse_charset(cont.characterset, cont.pol);

    return cont;
}





/**********************************************************************
 *                        ANALYSE STATISTIQUE                         *
 **********************************************************************/



void Statsgen::updateMinMax(const Policy & pol) {
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



int Statsgen::generate_stats(const string & filename) {
    wifstream readfile(filename);
    wstring line;


    ThreadPool pool(4);
    vector< future<Conteneur> > results;


    while(readfile.good()) {
        getline(readfile, line);
        total_counter++;

        if (line.size() == 0) {
            wcout << "Error empty password, line " << total_counter << endl;
            continue;
        }
        
        total_filter++;
        
        results.emplace_back(
            pool.enqueue([line] {
                return analyze_password(line);
            })
        );
    }

    for(auto && result: results) {
        Conteneur c = result.get();
        stats_length[ c.pass_length ] += 1;
        stats_charactersets[ c.characterset ] += 1;
        stats_simplemasks[ c.simplemask_string ] += 1;
        stats_advancedmasks[ c.advancedmask_string ] += 1;
        updateMinMax(c.pol);
    }

    if (!total_counter) {
        wcerr << "Empty file or not existing file" << endl;
        return 0;
    }

    readfile.close();
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