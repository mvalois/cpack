#include <map>


template<typename A>
std::multimap<int, A> flip_map(const std::map<A, int> & src) {
    std::multimap<int, A> dst;

    std::map<A, int>::const_iterator it;
    for(it = src.begin(); it != src.end(); ++it)
        dst.insert(std::pair<int, A>(it -> second, it -> first));

    return dst;
}
