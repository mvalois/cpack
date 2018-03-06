/**
 * @file utils.h
 * @brief useful functions
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 * Copyright (c) 2018 Jorand Jean-baptiste Bass Yannick
 * All rights reserved.
 *
 * Please see the attached LICENSE file for additional licensing information.
 */

#ifndef UTILS_STATSGEN_H
#define UTILS_STATSGEN_H

#include <map>
#include <iostream>
#include <unordered_map>
using namespace std;




template<typename K, typename V>
using UnorderedMapIterator = typename unordered_map<K, V>::const_iterator;


template<typename K, typename V>
using MapIterator = typename map<K, V>::const_iterator;



/**
 * @brief Order an unoredered map whatever the key's type
 * @param src: unoredered map than needed to be ordered
 * @return ordered map
 */
template<typename A> 
multimap<uint64_t, A> flip_map(const std::unordered_map<A, uint64_t> & src) {
	multimap<uint64_t, A> dst;

	for(UnorderedMapIterator<A, uint64_t> it = src.begin(); it != src.end(); ++it)
		dst.insert(std::pair<uint64_t, A>(it->second, it->first));

	return dst;
}



/**
 * @brief Print a line of statistics
 * @param res: value of this statistic
 * @param crac: the statistic to display
 * @param count: the number of statistics already displayed
 * @param total_counter:total number of password analysed
 * @param hiderare: flag to hide statistics under 1%
 */
template<typename Type>
void readResult(uint64_t res, Type carac, int & count, const uint64_t & total_counter, const int & hiderare) {
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
 * @brief Print an unordered map
 * @param stats: map to show
 * @param top: number of results to show
 * @param total_counter: number of finded passwords
 * @param hiderare: low statistics to hide
 * @param count: number of shown results
 */
template<typename Type>
void showMap(const unordered_map<Type, uint64_t> & stats, const int & top, const uint64_t & total_counter, const int & hiderare, int & count) {
	count = 0;
	multimap<uint64_t, Type> reverse = flip_map<Type>(stats);
	
	MapIterator<uint64_t, Type> it;
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
 * @brief Calculate the number of line in a file
 * @param filename: name of the file
 * @return number of line
 */
uint64_t nbline_file(const string & filename) {
	wifstream readfile(filename);
	wstring line;
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



#endif



