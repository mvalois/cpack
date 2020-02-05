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

	int generate_stats();
	void print_stats() const;

	inline void setHiderare(const int& hr) { hiderare = hr; }
	inline void setTop(const int& t) { top = t; }
	inline void setRegex(const std::string& reg) {
		current_regex = reg;
		use_regex = true;
	}
	inline void setWithcount(const bool& wc) { withcount = wc; }
	inline void setLimitSimplemask(const int& limit) { limitSimplemask = limit; }
	inline void setLimitAdvancedmask(const int& limit) { limitAdvancedmask = limit; }
	inline void setNbThread(const int& nb) { nbThread = nb; }
	void configureThread(ThreadData& td) const;
	void setSecurityRules(const uint& length, const uint& special, const uint& digit, const uint& upper, const uint& lower);
	inline void setOutfile(const std::string& outfile) { outfile_name = outfile; }
	inline void enableDebug() { debug_enabled = true; }

	inline int getNbThreads() const { return nbThread; }
	inline uint64_t getNbLines() const { return nblines; }
	inline uint64_t getProcessed() const { return processed; }
	inline const ThreadData& getResults() const { return results; }
	inline bool allFinished() const { return finished; }
	inline bool allStarted() const { return started; }
	bool operator==(const Statsgen& other) const;

private:
	std::string filename;
	// results of the computation
	ThreadData results;

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

	// nb lines processed
	uint64_t nblines = 0;
	// nb lines processed
	uint64_t processed = 0;
	// all threads have finished
	uint finished = false;
	// all threads have been started
	bool started = false;

	void handle_password(const std::string& password, const uint64_t& nbPasswords, ThreadData& td) const;
	std::pair<uint, uint> get_masks(const std::string& password, PasswordStats& c) const;
};

#endif
