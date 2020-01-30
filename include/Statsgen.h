/**
 * @file statsgen.h
 * @brief Statsgen class and all needed structures
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 * Copyright (c) 2018 Jorand Jean-baptiste Bass Yannick
 * All rights reserved.
 *
 * Please see the attached LICENSE file for additional licensing information.
 */


#ifndef STATSGEN_H
#define STATSGEN_H


#include <string>
#include <unordered_map>
#include <iostream>
#include <regex>
#include <queue>
#include <thread>

#include "Policy.h"
#include "SecurityRules.h"

#define MAX_THREADS 32

typedef std::unordered_map<std::string, uint64_t> StringOccurrence;
typedef std::unordered_map<int, uint64_t> IntOccurrence;


struct PasswordStats {
	int pass_length = 0;
	std::string advancedmask_string = "";
	std::string simplemask_string = "";
	Policy pol;
};


/**
 * @brief Simplify number of arguments for functions
 */
struct minMax {
	int mindigit = -1;				// save the number minimum of digit in a password
	int maxdigit = -1;				// save the number maximum of digit in a password
	int minlower = -1;				// save the number minimum of lower in a password
	int maxlower = -1;				// save the number maximum of lower in a password
	int minupper = -1;				// save the number minimum of upper in a password
	int maxupper = -1;				// save the number maximum of upper in a password
	int minspecial = -1;			// save the number minimum of special character in a password
	int maxspecial = -1;			// save the number maximum of special character in a password
};

/**
 * @brief All needed variables to give to each thread
 */
struct thread_data {
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

	int limitSimplemask;
	int limitAdvancedmask;

	SecurityRules sr;

	bool finished = false;
};



/**
 * @brief Calcul and save all analysed statistics
 */
class Statsgen {
public:
	Statsgen(){}
	Statsgen(const std::string& name);

	/**
	 * @brief show all options for the command
	 */
	void showHelp();

	/**
	 * @brief useful to hide statistics below 1%
	 * @param hr: 1 to hide, else 0
	 */
	inline void setHiderare(const int& hr) { hiderare = hr; }

	/**
	 * @brief Defining the number of best results the user wants to see,
	 * and so the result should be clearer
	 * @param t: number of interesting results
	 */
	inline void setTop(const int& t) { top = t; }

	/**
	 * @brief Defining a regular expression to analyse only
	 * some interesting passwords
	 * @param reg: regular expression
	 */
	inline void setRegex(const std::string& reg) {
		current_regex = reg;
		use_regex = true;
	}

	/**
	 * @brief Useful to know if the database uses the format withcount
	 * @param var: true if database uses the format withcount, else false
	 */
	inline void setWithcount(const bool& wc) { withcount = wc; }

	/**
	 * @brief Filter for the size of the simple masks, can be
	 * used as an optimisation option
	 * @param limit: number that limit the size of the simple masks
	 */
	inline void setLimitSimplemask(const int& limit) { limitSimplemask = limit; }

	/**
	 * @brief Filter for the size of the advanced masks, can be
	 * used as an optimisation option
	 * @param limit: number that limit the size of the advanced masks
	 */
	inline void setLimitAdvancedmask(const int& limit) { limitAdvancedmask = limit; }

	/**
	 * @brief Number of threads the user wants to use
	 * @param nb: number of usable threads
	 */
	inline void setNbThread(const int& nb) {
		int max = std::thread::hardware_concurrency();
		if (nb > max) {
			nbThread = max;
		} else {
			nbThread = nb;
		}
	}

	void configureThread(thread_data& td) const;
	void mergeThread(const thread_data& td);


	/**
	 * @brief Defining all security rules
	 */
	void askSecurityRules();
	void setSecurityRules(const int& length, const int& special, const int& digit, const int& upper, const int& lower);

	/**
	 * @brief Where to write masks
	 * @param outfile: the file where to write masks
	 */
	inline void setOutfile(const std::string& outfile) { outfile_name = outfile; }

	/**
	 * @brief enable debugging
	 */
	inline void enableDebug() { debug_enabled = true; }


	/**
	 * @brief Calculate all statistics for a database
	 * @return 0 if error, 1 if success
	 */
	int generate_stats();


	/**
	 * @brief Print all calculated statistics
	 */
	void print_stats();

	inline uint64_t getTotalCounter() const { return total_counter; }
	inline uint64_t getTotalFilter() const { return total_filter; }
	inline uint64_t getNbSecurePasswords() const { return _sr.nbSecurePassword; }
	inline int getNbThreads() const { return nbThread; }
	inline const IntOccurrence& getStatsLength() const { return stats_length; }
	inline const StringOccurrence& getStatsCharsets() const { return stats_charactersets; }
	inline const thread_data* getThreadsData() const { return td; }

private:
	std::string filename;
	struct thread_data td[MAX_THREADS];

	// Filters

	int hiderare = 0; 				// Hide low statistics
	int top = 10;					// Show only a top of statistics
	std::regex current_regex;		// Regex for the interesting passwords
	bool use_regex = false;			// Know if we use a regex or not
	bool withcount = false;			// Know if the database is at the format withcount or not
	int limitSimplemask = 12;		// Limit the size of Simple Mask
	int limitAdvancedmask = 12;		// Limit the size of Advanced Mask
	int nbThread = 1;				// Number of usable threads, default 1
	std::string outfile_name;		// File where to write masks
	bool is_stdin = false;				// If filename is stdin
	bool debug_enabled = false;		// Enable debug output


	// Dictionary

	IntOccurrence stats_length;					// Passwords' length linked to their occurrence
	StringOccurrence stats_simplemasks;	// Passwords' simple mask linked to their occurrence
	StringOccurrence stats_advancedmasks;	// Passwords' advanced mask linked to their occurrence
	StringOccurrence stats_charactersets;	// Passwords' characterset linked to their occurrence


	// Counters

	uint64_t total_counter = 0;		// number of analysed passwords
	uint64_t total_filter = 0;		// number of analysed passwords after a filter

	minMax minMaxValue;				// save all genral data from passwords


	// Security policy
	SecurityRules _sr = { 0, 8, 0, 1, 1, 1	};
};



/**
 * @brief Analyse a letter in a password
 * @param letter: current letter
 * @param last_simplemask: last analyzed part of the simple masks
 * @param simplemask_string: current simple mask
 * @param advancedmask_string: current advanced mask
 * @param policy: current number of digit, lower, upper and special for the current password
 * @param sizeAdvancedMask: size of the current advanced mask
 * @param sizeSimpleMask: size of the current simple mask
 */
// void analyse_letter(const char & letter, char & last_simplemask, std::string & simplemask_string, std::string & advancedmask_string, Policy & policy, int & sizeAdvancedMask, int & sizeSimpleMask);

/**
 * @brief Analyse a password
 * @param password: current password
 * @param c: container of all useful data of the password
 * @param sr: get the actual security rules and count the number of password respecting them
 * @param limitAdvancedmask: define the limit for the size of advanced masks
 * @param limitSimplemask: define the limit for the size of simple masks
 */
// Container analyze_password(const std::string & password, SecurityRules & sr, const int & limitAdvancedmask, const int & limitSimplemask);

/**
 * @brief Update minima and maxima of all general data from analysed passwords
 * with the analysed passwords
 * @param minMaxValue: save all minima and maxima
 * @param pol : policy of the last analyzed password
 */
void updateMinMax(minMax & minMaxValue, const Policy & pol);

/**
* @brief Action of all threads
* @param threadarg : all useful argument for the thread
*/
void * generate_stats_thread(void * threadarg);

/**
 * @brief Compute statistics by consumming password from a queue
 * @param threadarg : arguments required by the thread
 */
void * generate_stats_thread_queue(void * threadarg);

#endif
