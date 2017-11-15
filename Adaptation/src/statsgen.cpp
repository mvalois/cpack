#include <fstream>
#include <iomanip>
#include <algorithm>
#include <regex>
#include <time.h>
#include "statsgen.h"
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
    wcout << "\t--regex [value]\t\t:\tShow result for password, using the regular expression [value]" << endl;

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




/**********************************************************************
 *                          USEFUL FUNCTIONS                          *
 **********************************************************************/



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


void duree(time_t _begin, time_t _end) {
    double temp; 
    double hours=0, min=0, sec=0; 
    double dureeCalc = difftime(_end, _begin);
    temp = modf(dureeCalc/3600., &hours); 
    temp = modf(temp*60., &min); 
    temp = modf(temp*60., &sec); 
    cout<<"Duree du calcul : "<<hours<<" h "<<min<<" min "<<sec<<" sec"<<endl; 
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


    clock_t t1, t2, t3, t4;
    float tmp1, tmp2;
 

    for (int i=0; i<length; i++) {
    	wchar_t letter = password[i];
        
        t1 = clock();
        analyse_letter(letter, last_simplemask, simplemask_string, advancedmask_string, policy);
        t2 = clock();
        tmp1 = difftime(t2, t1);

        if (tmp1 < timeLetterMin || timeLetterMin == 0) timeLetterMin = tmp1;
        if (tmp1 > timeLetterMin) timeLetterMax = tmp1;
        timeLetterTotal += tmp1;
    }

    t3 = clock();
    analyse_charset(charset, policy);
    t4 = clock();
    tmp2 = difftime(t4, t3);

    if (tmp2 < timeCharsetMin || timeCharsetMin == 0) timeCharsetMin = tmp2;
    if (tmp2 > timeCharsetMin) timeCharsetMax = tmp2;
    timeCharsetTotal += tmp2;
}





/**********************************************************************
 *                        ANALYSE STATISTIQUE                         *
 **********************************************************************/



bool analyse_withcount(wstring & line) {
    wstring key  = wstring(L"^\\s+");
    wstring repl = wstring(L"");
    wstring key2  = wstring(L"(\\d+) (.+)");
    
    wstring cleanLine = regex_replace(line, std::wregex(key),  repl);
    wsmatch m;
    regex_match(cleanLine,m,std::wregex(key2));

    return (m.size() > 2);
}


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
    int nbline = 0;



    clock_t t1, t2;

    t1 = clock();

    while(readfile.good()) {
        getline(readfile, line);
        nbline++;

        if (line.size() == 0) {
            wcout << "Error empty password, line " << nbline << endl;
            continue;
        }


        if (withcount) {
            int i = 0;
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
            
            total_counter += nbPasswords;

            if ( !use_regex || (use_regex && regex_match(password,current_regex)) ) {
                total_filter += nbPasswords;
                
                analyze_password(password, pass_length, characterset, simplemask_string, advancedmask_string, pol);

                stats_length[pass_length] += nbPasswords;
                stats_charactersets[characterset] += nbPasswords;
                stats_simplemasks[simplemask_string] += nbPasswords;
                stats_advancedmasks[advancedmask_string] += nbPasswords;
            }
        }

        else {
            if ( !use_regex || (use_regex && regex_match(line,current_regex)) ) {
                total_filter++;

                analyze_password(line, pass_length, characterset, simplemask_string, advancedmask_string, pol);

                stats_length[pass_length] += 1;
                stats_charactersets[characterset] += 1;
                stats_simplemasks[simplemask_string] += 1;
                stats_advancedmasks[advancedmask_string] += 1;
            }
            total_counter++;
        }

        updateMinMax(pol);
    }

    t2 = clock();

    timeTotal = difftime(t2,t1);

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


void Statsgen::readResult(int res, int length, int & count) {
    float percentage;

    percentage = (float) (100*res) / total_counter;

    if (percentage >= hiderare) {
        wstring value = to_wstring(percentage);
        value = value.substr(0,5);

        wcout << setw(40) << right << length << ":  " 
            << setw(5) << right << value << "%" 
            << setw(5) << right << "(" << res << ")" << endl;

        count++;
    }
}

void Statsgen::readResult(int res, wstring carac, int & count) {
    float percentage;
    percentage = (float) (100*res) / total_counter;

    if (percentage >= hiderare) {
        wstring value = to_wstring(percentage);
        value = value.substr(0,5);

        wcout << setw(40) << right << carac << ":  " 
            << setw(5) << right << value << "%" 
            << setw(5) << right << "(" << res << ")" << endl;

        count++;
    }
}



void Statsgen::print_stats() {
    int count;
    float perc = (float) (100*total_filter) / total_counter;

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

    count = 0;
    multimap<int, int> reverseLength = flip_map(stats_length);
    map<int, int>::const_iterator it1;
    for(it1 = reverseLength.end(); it1 != reverseLength.begin(); it1--) {
        if (it1 == reverseLength.end()) continue;
        
        readResult(it1->first, it1->second, count);
        if (top != -1 && count == top) break;
    }

    if (count != top) {
        readResult(it1->first, it1->second, count);
    }
    




    

    wcout << "\nStatistics relative to charsets: \n" << endl;
    
    count = 0;
    multimap<int, wstring> reverseCharsets = flip_map(stats_charactersets);
    map<int, wstring>::const_iterator it2;

    for(it2 = reverseCharsets.end(); it2 != reverseCharsets.begin(); it2--) {
        if (it2 == reverseCharsets.end()) continue;

        readResult(it2->first, it2->second, count);
        if (top != -1 && count == top) break;
    }

    if (count != top) {
        readResult(it2->first, it2->second, count);
    }
    



    wcout << "\nStatistics relative to simplemasks: \n" << endl;

    count = 0;
    multimap<int, wstring> reverseSimpleMasks = flip_map(stats_simplemasks);
    map<int, wstring>::const_iterator it3;

    for(it3 = reverseSimpleMasks.end(); it3 != reverseSimpleMasks.begin(); it3--) {
        if (it3 == reverseSimpleMasks.end()) continue;
        
        readResult(it3->first, it3->second, count);
        if (top != -1 && count == top) break;
    }

    if (count != top) {
        readResult(it3->first, it3->second, count);
    }




    wcout << "\nStatistics relative to advancedmask: \n" << endl;

    count = 0;
    multimap<int, wstring> reverseAdvancedMask = flip_map(stats_advancedmasks);
    map<int, wstring>::const_iterator it4;

    for(it4 = reverseAdvancedMask.end(); it4 != reverseAdvancedMask.begin(); it4--) {
        if (it4 == reverseAdvancedMask.end()) continue;
        
        readResult(it4->first, it4->second, count);
        if (top != -1 && count == top) break;
    }

    if (count != top) {
        readResult(it4->first, it4->second, count);
    }


    wcout << "\n\nLetter --> min : " << timeLetterMin << " / max : " << timeLetterMax << " / total : " << timeLetterTotal << endl;
    wcout << "Charset --> min : " << timeCharsetMin << " / max : " << timeCharsetMax << " / total : " << timeCharsetTotal << endl;
    
    wcout << "\nTotal temps : " << timeTotal << endl;
    float lost = timeTotal - timeLetterTotal - timeCharsetTotal;

    wcout << "\nTotal restant : " << lost << endl;

}

