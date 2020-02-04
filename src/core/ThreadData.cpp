#include "ThreadData.h"

#include <iostream>

using namespace std;

void minMax::updateMinMax(const minMax &m){
	mindigit = std::min(mindigit, m.mindigit);
	minlower = std::min(minlower, m.minlower);
	minupper = std::min(minupper, m.minupper);
	minspecial = std::min(minspecial, m.minspecial);
	maxdigit = std::max(maxdigit, m.maxdigit);
	maxlower = std::max(maxlower, m.maxlower);
	maxupper = std::max(maxupper, m.maxupper);
	maxspecial = std::max(maxspecial, m.maxspecial);
}

void ThreadData::operator+=(const ThreadData& other){
	*this = *this + other;
}

ThreadData ThreadData::operator+(const ThreadData& other) const {
	ThreadData td(*this);
	td.total_counter = total_counter + other.total_counter;
	td.total_filter = total_filter + other.total_filter;

	td.sr.nbSecurePassword += other.sr.nbSecurePassword;

	td.minMaxValue.updateMinMax(other.minMaxValue);


	for(std::pair<int, uint64_t> occ: other.length){
		td.length[occ.first] += occ.second;
	}

	for(std::pair<std::string, int> occ : other.charactersets){
		td.charactersets[occ.first] += occ.second;
	}

	for(std::pair<std::string, int> occ: other.simplemasks){
		td.simplemasks[occ.first] += occ.second;
	}

	for(std::pair<std::string, int> occ: other.advancedmasks){
		td.advancedmasks[occ.first] += occ.second;
	}
	return td;
}