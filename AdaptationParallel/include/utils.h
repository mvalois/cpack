/**
 * @file utils.h
 * @brief useful functions
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 *Copyright © 31/01/2018, Yannick BASS Jean-Baptiste JORAND
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”),
 *to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *The Software is provided “as is”, without warranty of any kind, express or implied, including but not limited to the warranties of merchantability,
 *fitness for a particular purpose and noninfringement. In no event shall the authors or copyright holders X be liable for any claim, damages or other liability,
 *whether in an action of contract, tort or otherwise, arising from, out of or in connection with the software or the use or other dealings in the Software.
 *Except as contained in this notice, the name of Yannick BASS and Jean-Baptiste JORAND shall not be used in advertising or otherwise to promote the sale,
 *use or other dealings in this Software without prior written authorization from Yannick BASS and Jean-Baptiste JORAND. 
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
multimap<double, A> flip_map(const std::unordered_map<A, double> & src) {
    multimap<double, A> dst;

    for(UnorderedMapIterator<A, double> it = src.begin(); it != src.end(); ++it)
        dst.insert(std::pair<double, A>(it->second, it->first));

    return dst;
}



/**
 * @brief print a line of statistics
 * @param res : value of this statistic
 * @param crac: the statistic to display
 * @param count: the number of statistics already displayed
 * @param total_counter:total number of password analysed
 * @param hiderare: flag to hide statistics under 1%
 */
template<typename Type>
void readResult(double res, Type carac, int & count, const double & total_counter, const int & hiderare) {
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
void showMap(const unordered_map<Type, double> & stats, const int & top, const double & total_counter, const int & hiderare, int & count) {
	count = 0;
    multimap<double, Type> reverse = flip_map<Type>(stats);
    
    MapIterator<double, Type> it;
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







