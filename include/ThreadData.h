#ifndef THREAD_DATA_H
#define THREAD_DATA_H

#include <string>
#include <unordered_map>
#include <climits>
#include <regex>

#include "SecurityRules.h"

typedef std::unordered_map<std::string, uint64_t> StringOccurrence;
typedef std::unordered_map<int, uint64_t> IntOccurrence;

/**
 * @brief minimal number of digits of a password, etc.
 */
struct minMax {
	/**
	 * @brief Update minima and maxima of all general data from analysed passwords
	 * with the analysed passwords
	 * @param m: other minMaxValue to merge with
	 */
	void updateMinMax(const minMax &m);
	bool operator==(const minMax& m) const;
	uint mindigit = UINT_MAX;
	uint maxdigit = 0;
	uint minlower = UINT_MAX;
	uint maxlower = 0;
	uint minupper = UINT_MAX;
	uint maxupper = 0;
	uint minspecial = UINT_MAX;
	uint maxspecial = 0;
};

struct ThreadData {
	ThreadData operator+(const ThreadData& other) const;
	void operator+=(const ThreadData& other);
	bool operator==(const ThreadData& other) const;
	int thread_id;
	std::string filename;
	uint64_t lineBegin = 0;
	uint64_t lineEnd = 0;

	uint64_t total_counter = 0;
	uint64_t total_filter = 0;

	IntOccurrence length;
	StringOccurrence simplemasks;
	StringOccurrence advancedmasks;
	StringOccurrence charactersets;

	minMax minMaxValue;

	std::regex current_regex;
	bool use_regex = false;
	bool withcount = false;

	uint limitSimplemask;
	uint limitAdvancedmask;

	SecurityRules sr;
};

#endif // THREAD_DATA_H