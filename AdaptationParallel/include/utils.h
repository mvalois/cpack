/**
 * @file utils.h
 * @brief useful functions
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 */

#include <map>
#include <iostream>
#include <unordered_map>
using namespace std;



template<typename K, typename V>
using UnorderedMapIterator = typename unordered_map<K, V>::const_iterator;


template<typename K, typename V>
using MapIterator = typename map<K, V>::const_iterator;



/**
 * @brief Useful functions to order an unoredered map
 * whatever the key's type
 * @param src : unoredered map than needed to be ordered
 * @return ordered map
 */
template<typename A> 
multimap<int, A> flip_map(const std::unordered_map<A, int> & src) {
    multimap<int, A> dst;

    for(UnorderedMapIterator<A, int> it = src.begin(); it != src.end(); ++it)
        dst.insert(std::pair<int, A>(it->second, it->first));

    return dst;
}



/**
 * @brief print some results
 * @param 
 */
template<typename Type>
void readResult(int res, Type carac, int & count, const int & total_counter, const int & hiderare) {
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


/**
 * @brief print an unordered map
 * @param stats : map to show
 * @param top : number of results to show
 * @param total_counter : number of finded passwords
 * @param hiderare : low statistics to hide
 * @param count : number of shown results
 */
template<typename Type>
void showMap(const unordered_map<Type, int> & stats, const int & top, const int & total_counter, const int & hiderare, int & count) {
	count = 0;
    multimap<int, Type> reverse = flip_map<Type>(stats);
    
    MapIterator<int, Type> it;
	for(it = reverse.end(); it != reverse.begin(); it--) {
        if (it == reverse.end()) continue;

        readResult<Type>(it->first, it->second, count, total_counter, hiderare);
        if (top != -1 && count == top) break;
    }

    if (count != top) {
        readResult<Type>(it->first, it->second, count, total_counter, hiderare);
    }
}




/**
 * @brief analyse a letter in a password
 * @param letter : current letter
 * @param last_simplemask : last analyzed part of the simple masks 
 * @param simplemask_string : current simple mask
 * @param advancedmask_string : current advanced mask
 * @param policy : current number of digit, lower, upper and special for the current password
 * @param sizeAdvancedMask : size of the current advanced mask 
 * @param sizeSimpleMask : size of the current simple mask
 */
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



/**
 * @brief analyse the characterset of the current password
 * @param charset : characterset of the current password
 * @param policy : current number of digit, lower, upper and special for the current password
 */
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



/**
 * @brief analyse a password
 * @param password : current password
 * @param c : container of all useful data of the password
 */
void analyze_password(const wstring & password, Container & c) {
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
}




/**
 * @brief update minima and maxima of all general data from analysed passwords 
 * with the analysed passwords
 * @param minMaxValue : save all minima and maxima
 * @param pol : policy of the last analyzed password
 */
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





/**
 * @brief action of all threads
 * @param threadarg : all useful argument for the thread
 */
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
            wcout << "Error empty password, line " << my_data->total_counter << endl;
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
         updateMinMax(my_data->minMaxValue, c.pol);
    }

    readfile.close();

    pthread_exit(NULL);
}



/**
 * @brief calculate the number of line in a file
 * @param filename : name of the file
 * @return : number of line
 */
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







