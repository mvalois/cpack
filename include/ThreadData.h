#ifndef THREAD_DATA_H
#define THREAD_DATA_H

#include <string>
#include <unordered_map>
#include <queue>
#include <climits>
#include <regex>

#include "Policy.h"
#include "SecurityRules.h"

typedef std::unordered_map<std::string, uint64_t> StringOccurrence;
typedef std::unordered_map<int, uint64_t> IntOccurrence;

/**
 * @brief minimal number of digits of a password, etc.
 */
class minMax {
public:
	void updateMinMax(const Policy& pol);
	uint mindigit = UINT_MAX;
	uint maxdigit = 0;
	uint minlower = UINT_MAX;
	uint maxlower = 0;
	uint minupper = UINT_MAX;
	uint maxupper = 0;
	uint minspecial = UINT_MAX;
	uint maxspecial = 0;
};

class ThreadData {
public:
	ThreadData operator+(const ThreadData& other);
	void operator+=(const ThreadData& other);
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
	std::queue<std::string> password_queue;

	minMax minMaxValue;

	std::regex current_regex;
	bool use_regex = false;
	bool withcount = false;

	uint limitSimplemask;
	uint limitAdvancedmask;

	SecurityRules sr;

	bool finished = false;
};

#endif // THREAD_DATA_H