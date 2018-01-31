 
/**Copyright © 31/01/2018, Yannick BASS Jean-Baptiste JORAND
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
using UnorderedMapIterator = typename std::unordered_map<K, V>::const_iterator;

template<typename K, typename V>
using MapIterator = typename std::map<K, V>::const_iterator;


template<typename A> 
multimap<int, A> flip_map(const std::unordered_map<A, int> & src) {
    multimap<int, A> dst;

    for(UnorderedMapIterator<A, int> it = src.begin(); it != src.end(); ++it)
        dst.insert(std::pair<int, A>(it->second, it->first));

    return dst;
}




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









