#include <fstream>
#include <iomanip>
#include <algorithm>
#include "statsgen.h"
using namespace std;



void Statsgen::showHelp() {
    wcout << "\nUsage database.txt [options]\n" << endl;

    wcout << "Options:" << endl;
    wcout << "\t--help, -h\t\t:\tShow this help message" << endl;
    wcout << "\t--hiderare, -r\t\t:\tHide all statistics below 1%" << endl;
    wcout << "\t--top, -t [value]\t:\tShow only [value] first results" << endl;
    wcout << "\t--regex [value]\t\t:\tShow result for password, using the regular expression [value]" << endl;

    wcout << "\n" << endl;
}


void Statsgen::setHiderare(int val) {
    hiderare = val;
}



void Statsgen::setTop(int val) {
    top = val;
}



multimap<int, int> flip_map(map<int,int> & src) {
    multimap<int,int> dst;

    for(map<int, int>::const_iterator it = src.begin(); it != src.end(); ++it)
        dst.insert(pair<int, int>(it -> second, it -> first));

    return dst;
}


multimap<int, wstring> flip_map(map<wstring,int> & src) {
    multimap<int, wstring> dst;

    for(map<wstring, int>::const_iterator it = src.begin(); it != src.end(); ++it)
        dst.insert(pair<int, wstring>(it -> second, it -> first));

    return dst;
}




/**********************************************************************
 *                          ANALYSE PASSWORD                          *
 **********************************************************************/


void Statsgen::analyse_letter(const char & letter, char & last_simplemask, wstring & simplemask_string, wstring & advancedmask_string, Policy & policy) {
    if (letter >= L'0' && letter <= L'9') {
        policy.digit++;
        advancedmask_string += L"?d";
        if (last_simplemask != 'd') {
            simplemask_string += L"digit";
            last_simplemask = 'd';
        }
    }
    else if(letter >= L'a' && letter <= L'z') {
        policy.lower++;
        advancedmask_string += L"?l";
        if (last_simplemask != 'l') {
            simplemask_string += L"lower";
            last_simplemask = 'l';
        }
    }
    else if(letter >= L'A' && letter <= L'Z') {
        policy.upper++;
        advancedmask_string += L"?u";
        if (last_simplemask != 'u') {
            simplemask_string += L"upper";
            last_simplemask = 'u';
        }
    }
    else {
        policy.special++;
        advancedmask_string += L"?s";
        if (last_simplemask != 's') {
            simplemask_string += L"special";
            last_simplemask = 's';
        }
    }
}


void Statsgen::analyse_charset(wstring & charset, const Policy & policy) {
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


void Statsgen::analyze_password(const wstring & password, int & length, wstring & charset, wstring & simplemask_string, wstring & advancedmask_string, Policy & policy) {
	length = password.size();

	policy.digit = 0;
    policy.lower = 0;
    policy.upper = 0;
    policy.special = 0;

    advancedmask_string = L"";
    simplemask_string = L"";
    char last_simplemask = 'a';


    for (int i=0; i<length; i++) {
    	wchar_t letter = password[i];
        analyse_letter(letter, last_simplemask, simplemask_string, advancedmask_string, policy);
    }

    analyse_charset(charset, policy);
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

    int pass_length;
    wstring characterset;
    wstring advancedmask_string;
    wstring simplemask_string;
    Policy pol;

    while(readfile.good()) {
        getline(readfile, line);
        
        if (line.size() == 0) {
            continue;
        }

        total_counter++;

        analyze_password(line, pass_length, characterset, simplemask_string, advancedmask_string, pol);

        updateMinMax(pol);
        stats_length[pass_length] += 1;
        stats_charactersets[characterset] += 1;
        stats_simplemasks[simplemask_string] += 1;
        stats_advancedmasks[advancedmask_string] += 1;
    }


    if (!total_counter) {
        wcerr << "Empty file or not existing file" << endl;
        return 0;
    }

    readfile.close();
    return 1;
}




void Statsgen::print_stats() {
    wcout << "Analyzing " << total_counter << " passwords" << endl;

    float percentage;
    int count;

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
    
    count = 0;
    multimap<int, int> reverseLength = flip_map(stats_length);

    for(map<int, int>::const_iterator it = reverseLength.end(); it != reverseLength.begin(); it--) {
        if (it == reverseLength.end()) continue;
        
        percentage = (float) (100*it->first) / total_counter;

        if (percentage >= hiderare) {
            wstring value = to_wstring(percentage);
            value = value.substr(0,5);

            wcout << setw(40) << right << it->second << ":  " 
                << setw(5) << right << value << "%" 
                << setw(5) << right << "(" << it->first << ")" << endl;

            count++;
            if (top != -1 && count == top) break;
        }
    }
    


    wcout << "\nStatistics relative to charsets: \n" << endl;
    
    count = 0;
    multimap<int, wstring> reverseCharsets = flip_map(stats_charactersets);

    for(map<int, wstring>::const_iterator it = reverseCharsets.end(); it != reverseCharsets.begin(); it--) {
        if (it == reverseCharsets.end()) continue;

        percentage = (float) (100*it->first) / total_counter;
        
        if (percentage >= hiderare) {
            wstring value = to_wstring(percentage);
            value = value.substr(0,5);

            wcout << setw(40) << right << it->second << ":  " 
                << setw(5) << right << value << "%" 
                << setw(5) << right << "(" << it->first << ")" << endl;

            count++;
            if (top != -1 && count == top) break;
        }
    }



    wcout << "\nStatistics relative to simplemasks: \n" << endl;

    count = 0;
    multimap<int, wstring> reverseSimpleMasks = flip_map(stats_simplemasks);

    for(map<int, wstring>::const_iterator it = reverseSimpleMasks.end(); it != reverseSimpleMasks.begin(); it--) {
        if (it == reverseSimpleMasks.end()) continue;
        
        percentage = (float) (100*it->first) / total_counter;

        if (percentage >= hiderare) {
            wstring value = to_wstring(percentage);
            value = value.substr(0,5);

            wcout << setw(40) << right << it->second << ":  " 
                << setw(5) << right << value << "%" 
                << setw(5) << right << "(" << it->first << ")" << endl;

            count++;
            if (top != -1 && count == top) break;
        }
    }



    wcout << "\nStatistics relative to advancedmask: \n" << endl;

    count = 0;
    multimap<int, wstring> reverseAdvancedMask = flip_map(stats_advancedmasks);

    for(map<int, wstring>::const_iterator it = reverseAdvancedMask.end(); it != reverseAdvancedMask.begin(); it--) {
        if (it == reverseAdvancedMask.end()) continue;
        
        percentage = (float) (100*it->first) / total_counter;
        
        if (percentage >= hiderare) {
            wstring value = to_wstring(percentage);
            value = value.substr(0,5);

            wcout << setw(40) << right << it->second << ":  " 
                << setw(5) << right << value << "%" 
                << setw(5) << right << "(" << it->first << ")" << endl;

            count++;
            if (top != -1 && count == top) break;
        }
    }
}

