#include <map>
#include <iostream>
#include <unordered_map>
using namespace std;



template<typename K, typename V>
using UnorderedMapIterator = typename unordered_map<K, V>::const_iterator;

template<typename K, typename V>
using MapIterator = typename map<K, V>::const_iterator;


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









