#include "ThreadData.h"

#include <iostream>

using namespace std;

void minMax::updateMinMax(const Policy& pol) {
	mindigit = std::min(mindigit, pol.digit);
	maxdigit = std::max(maxdigit, pol.digit);
	minlower = std::min(minlower, pol.lower);
	maxlower = std::max(maxlower, pol.lower);
	minupper = std::min(minupper, pol.upper);
	maxupper = std::max(maxupper, pol.upper);
	minspecial = std::min(minspecial, pol.special);
	maxspecial = std::max(maxspecial, pol.special);
}

void ThreadData::operator+=(const ThreadData& other){
	*this = *this + other;
}

ThreadData ThreadData::operator+(const ThreadData& other){
	ThreadData td(*this);
	td.total_counter = total_counter + other.total_counter;
	td.total_filter = total_filter + other.total_filter;

	Policy min, max;
	min.digit = other.minMaxValue.mindigit;
	min.lower = other.minMaxValue.minlower;
	min.upper = other.minMaxValue.minupper;
	min.special = other.minMaxValue.minspecial;

	max.digit = other.minMaxValue.maxdigit;
	max.lower = other.minMaxValue.maxlower;
	max.upper = other.minMaxValue.maxupper;
	max.special = other.minMaxValue.maxspecial;

	td.sr.nbSecurePassword += other.sr.nbSecurePassword;

	td.minMaxValue.updateMinMax(min);
	td.minMaxValue.updateMinMax(max);


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