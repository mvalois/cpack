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
#include <iostream>
#include <thread>

#include "Policy.h"
#include "SecurityRules.h"
#include "ThreadData.h"


struct PasswordStats {
	int pass_length = 0;
	std::string advancedmask_string = "";
	std::string simplemask_string = "";
	Policy pol;
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

	void configureThread(ThreadData& td) const;


	/**
	 * @brief Defining all security rules
	 */
	void askSecurityRules();
	void setSecurityRules(const uint& length, const uint& special, const uint& digit, const uint& upper, const uint& lower);

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

	inline uint64_t getTotalCounter() const { return td[0].total_counter; }
	inline uint64_t getTotalFilter() const { return td[0].total_filter; }
	inline uint64_t getNbSecurePasswords() const { return td[0].sr.nbSecurePassword; }
	inline int getNbThreads() const { return nbThread; }
	inline const IntOccurrence& getStatsLength() const { return td[0].length; }
	inline const StringOccurrence& getStatsCharsets() const { return td[0].charactersets; }
	inline const StringOccurrence& getStatsSimple() const { return td[0].simplemasks; }
	inline const StringOccurrence& getStatsAdvanced() const { return td[0].advancedmasks; }
	inline const std::vector<ThreadData>& getThreadsData() const { return td; }
	inline bool allFinished() const { return finished == nbThread; }
	inline bool allStarted() const { return started; }
	bool operator==(const Statsgen& other) const;

	/**
	* @brief Given parameters in threadarg, compute statistics on partition of the file
	* @param threadarg : parameters and result storage
	*/
	static void* generate_stats_thread(void * threadarg);

private:
	std::string filename;
	// results of the computation
	ThreadData results;
	// Data computed from within a thread, for read-accessibility
	std::vector<ThreadData> td;

	// Filters

	int hiderare = 0; 				// Hide low statistics
	int top = 10;					// Show only a top of statistics
	std::regex current_regex;		// Regex for the interesting passwords
	bool use_regex = false;			// Know if we use a regex or not
	bool withcount = false;			// Know if the database is at the format withcount or not
	int limitSimplemask = 12;		// Limit the size of Simple Mask
	int limitAdvancedmask = 12;		// Limit the size of Advanced Mask
	uint nbThread = 1;				// Number of usable threads, default 1
	std::string outfile_name;		// File where to write masks
	bool debug_enabled = false;		// Enable debug output

	// Security policy
	SecurityRules _sr = { 0, 8, 0, 1, 1, 1	};

	// threads which have finished
	uint finished = 0;
	// all threads have been started
	bool started = false;
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
 * @brief Update minima and maxima of all general data from analysed passwords
 * with the analysed passwords
 * @param minMaxValue: save all minima and maxima
 * @param pol : policy of the last analyzed password
 */
void updateMinMax(minMax & minMaxValue, const Policy & pol);


/**
 * @brief Compute statistics by consumming password from a queue
 * @param threadarg : arguments required by the thread
 */
void * generate_stats_thread_queue(void * threadarg);

#endif
