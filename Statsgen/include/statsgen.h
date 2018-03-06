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

#define MAX_THREADS 32



/**
 * @brief Simplify number of arguments for functions
 */
struct Policy{
	int digit = 0;
	int lower = 0;
	int upper = 0;
	int special = 0;
};


/**
 * @brief Simplify number of arguments for functions
 */
struct Container {
	int pass_length = 0;
	std::wstring characterset = L"";
	std::wstring advancedmask_string = L"";
	std::wstring simplemask_string = L"";
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
 * @brief All needed variables for the security rules
 */
struct SecurityRules {
	uint64_t nbSecurePassword;
	int minLength;
	int minSpecial;
	int minDigit;
	int minLower;
	int minUpper;
};


/**
 * @brief All needed variables to give to each thread
 */
struct thread_data {
	int thread_id;
	std::string filename;
	uint64_t lineBegin;
	uint64_t lineEnd;

	uint64_t total_counter = 0;
	uint64_t total_filter = 0;

	std::unordered_map<int, uint64_t> length;
	std::unordered_map<std::wstring, uint64_t> simplemasks;
	std::unordered_map<std::wstring, uint64_t> advancedmasks;
	std::unordered_map<std::wstring, uint64_t> charactersets;
	std::queue<std::wstring> password_queue;

	minMax minMaxValue;

	std::wregex current_regex;
	bool use_regex = false;
	bool withcount = false;

	int limitSimplemask;
	int limitAdvancedmask;

	SecurityRules sr;
};



/**
 * @brief Calcul and save all analysed statistics
 */
class Statsgen {
public:
	Statsgen() {};

	/**
	 * @brief show all options for the command
	 */
	void showHelp();

	/**
	 * @brief Initialise the name of the database
	 * @param name: name of the file
	 */
	void setFilename(std::string name);


	/**
	 * @brief useful to hide statistics below 1%
	 * @param hr: 1 to hide, else 0
	 */
	void setHiderare(int hr);

	/**
	 * @brief Defining the number of best results the user wants to see,
	 * and so the result should be clearer
	 * @param t: number of interesting results
	 */
	void setTop(int t);

	/**
	 * @brief Defining a regular expression to analyse only
	 * some interesting passwords
	 * @param reg: regular expression
	 */
	void setRegex(std::string reg);

	/**
	 * @brief Useful to know if the database uses the format withcount
	 * @param var: true if database uses the format withcount, else false
	 */
	void setWithcount(bool var);

	/**
	 * @brief Filter for the size of the simple masks, can be
	 * used as an optimisation option
	 * @param limit: number that limit the size of the simple masks
	 */
	void setLimitSimplemask(int limit);

	/**
	 * @brief Filter for the size of the advanced masks, can be
	 * used as an optimisation option
	 * @param limit: number that limit the size of the advanced masks
	 */
	void setLimitAdvancedmask(int limit);

	/**
	 * @brief Number of threads the user wants to use
	 * @param nb: number of usable threads
	 */
	void setNbThread(int nb);


	/**
	 * @brief Defining all security rules
	 */
	void setSecurityRules();

	/**
	 * @brief Where to write masks
	 * @param outfie: the file where to write masks
	 */
	void setOutfile(std::string outfile);

	/**
	 * @brief Calculate all statistics for a database
	 * @return 0 if error, 1 if success
	 */
	int generate_stats();


	/**
	 * @brief Print all calculated statistics
	 */
	void print_stats();



private:
	std::string filename;


	// Filters

	int hiderare = 0; 				// Hide low statistics
	int top = 10;					// Show only a top of statistics
	std::wregex current_regex;		// Regex for the interesting passwords
	bool use_regex = false;			// Know if we use a regex or not
	bool withcount = false;			// Know if the database is at the format withcount or not
	int limitSimplemask = 12;		// Limit the size of Simple Mask
	int limitAdvancedmask = 12;		// Limit the size of Advanced Mask
	int nbThread = 1;				// Number of usable threads, default 1
	std::string outfile_name;		// File where to write masks
	bool is_stdin = false;				// If filename is stdin


	// Dictionary

	std::unordered_map<int, uint64_t> stats_length;					// Passwords' length linked to their occurrence
	std::unordered_map<std::wstring, uint64_t> stats_simplemasks;	// Passwords' simple mask linked to their occurrence
	std::unordered_map<std::wstring, uint64_t> stats_advancedmasks;	// Passwords' advanced mask linked to their occurrence
	std::unordered_map<std::wstring, uint64_t> stats_charactersets;	// Passwords' characterset linked to their occurrence


	// Counters

	uint64_t total_counter = 0;		// number of analysed passwords
	uint64_t total_filter = 0;		// number of analysed passwords after a filter

	minMax minMaxValue;				// save all genral data from passwords


	// Security policy

	uint64_t nbSecurePassword = 0;	// all passwords that respect the security rules
	int minLength = 8;
	int minSpecial = 0;
	int minDigit = 1;
	int minLower = 1;
	int minUpper = 1;
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
void analyse_letter(const char & letter, char & last_simplemask, std::wstring & simplemask_string, std::wstring & advancedmask_string, Policy & policy, int & sizeAdvancedMask, int & sizeSimpleMask);

/**
 * @brief Analyse the characterset of the current password
 * @param charset: characterset of the current password
 * @param policy: current number of digit, lower, upper and special for the current password
 */
void analyse_charset(std::wstring & charset, const Policy & policy);

/**
 * @brief Analyse a password
 * @param password: current password
 * @param c: container of all useful data of the password
 * @param sr: get the actual security rules and count the number of password respecting them
 * @param limitAdvancedmask: define the limit for the size of advanced masks
 * @param limitSimplemask: define the limit for the size of simple masks
 */
void analyze_password(const std::wstring & password, Container & c, SecurityRules & sr, const int & limitAdvancedmask, const int & limitSimplemask);

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
