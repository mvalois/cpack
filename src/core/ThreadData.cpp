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

ThreadData ThreadData::operator+=(const ThreadData& other) const{
	return ThreadData(*this) + other;
}

ThreadData ThreadData::operator+(const ThreadData& other){
	total_counter += other.total_counter;
	total_filter += other.total_filter;

	Policy min, max;
	min.digit = other.minMaxValue.mindigit;
	min.lower = other.minMaxValue.minlower;
	min.upper = other.minMaxValue.minupper;
	min.special = other.minMaxValue.minspecial;

	max.digit = other.minMaxValue.maxdigit;
	max.lower = other.minMaxValue.maxlower;
	max.upper = other.minMaxValue.maxupper;
	max.special = other.minMaxValue.maxspecial;

	sr.nbSecurePassword += other.sr.nbSecurePassword;

	minMaxValue.updateMinMax(min);
	minMaxValue.updateMinMax(max);


	for(std::pair<int, uint64_t> occ: other.length){
		length[occ.first] += occ.second;
	}

	for(std::pair<std::string, int> occ : other.charactersets){
		charactersets[occ.first] += occ.second;
	}

	for(std::pair<std::string, int> occ: other.simplemasks){
		simplemasks[occ.first] += occ.second;
	}

	for(std::pair<std::string, int> occ: other.advancedmasks){
		advancedmasks[occ.first] += occ.second;
	}
	return *this;
}