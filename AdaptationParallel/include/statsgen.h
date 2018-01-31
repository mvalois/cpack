/**
 * @file statsgen.h
 * @brief Statsgen class and all needed structures
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 *Copyright © 31/01/2018, Yannick BASS Jean-Baptiste JORAND
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”),
 *to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *The Software is provided “as is”, without warranty of any kind, express or implied, including but not limited to the warranties of merchantability,
 *fitness for a particular purpose and noninfringement. In no event shall the authors or copyright holders X be liable for any claim, damages or other liability,
 *whether in an action of contract, tort or otherwise, arising from, out of or in connection with the software or the use or other dealings in the Software.
 *Except as contained in this notice, the name of Yannick BASS and Jean-Baptiste JORAND shall not be used in advertising or otherwise to promote the sale,
 *use or other dealings in this Software without prior written authorization from Yannick BASS and Jean-Baptiste JORAND. 
 */


#include <string>
#include <unordered_map>
#include <iostream>
#include <regex>



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
	double nbSecurePassword;
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
	int lineBegin;
	int lineEnd;

	double total_counter = 0;
	double total_filter = 0;

	std::unordered_map<int, double> length;
	std::unordered_map<std::wstring, double> simplemasks;
	std::unordered_map<std::wstring, double> advancedmasks;
	std::unordered_map<std::wstring, double> charactersets;

	minMax minMaxValue;

	std::wregex current_regex;
	bool use_regex = false;
	bool withcount = false;

	SecurityRules sr;
};



/**
 * @brief Calcul and save of all analysed statistics
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
	 * @param : new value for "filename"
	 */
	void setFilename(std::string);


	/**
	 * @brief Modify the attribute "hiderare", defining if 
	 * user wants to see statistics below 1%
	 * @param : new value for "hiderare"
	 */
	void setHiderare(int);

	/**
	 * @brief Modify the attribute "top", defining the number
	 * of best results the user wants to see
	 * @param : new value for "top"
	 */
	void setTop(int);

	/**
	 * @brief Modify the attribute "regex", defining the regular 
	 * expression of the interesting passwords
	 * @param : new value for "regex"
	 */
	void setRegex(std::string);

	/**
	 * @brief Modify the attribute "withcount", useful to know
	 * if the database uses the format withcount
	 * @param : new value for "withcount"
	 */
	void setWithcount(bool);

	/**
	 * @brief Modify the attribute "limitSimplemask", a filter
	 * for the size of the simple masks
	 * @param : new value for "limitSimplemask"
	 */
	void setLimitSimplemask(int);

	/**
	 * @brief Modify the attribute "limitAdvancedmask", a filter
	 * for the size of the advanced masks
	 * @param : new value for "limitAdvancedmask"
	 */
	void setLimitAdvancedmask(int);

	/**
	 * @brief Modify the attribute "nbThread", defining the number
	 * of threads the user wants to use
	 * @param : new value for "nbThread"
	 */
	void setNbThread(int);


	/**
	 * @brief Defining all security rules
	 */
	void setSecurityRules();


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
	int top = -1;					// Show only a top of statistics
	std::wregex current_regex;		// Regex for the interesting passwords
	bool use_regex = false;			// Know if we use a regex or not
	bool withcount = false;			// Know if the database is at the format withcount or not
	int limitSimplemask = 0;		// Limit the size of Simple Mask
	int limitAdvancedmask = 0;		// Limit the size of Advanced Mask
	int nbThread = 1;				// Number of usable threads, default 1


	// Dictionary

	std::unordered_map<int, double> stats_length;					// Passwords' length linked to their occurrence
	std::unordered_map<std::wstring, double> stats_simplemasks;	// Passwords' simple mask linked to their occurrence
	std::unordered_map<std::wstring, double> stats_advancedmasks;	// Passwords' advanced mask linked to their occurrence
	std::unordered_map<std::wstring, double> stats_charactersets;	// Passwords' characterset linked to their occurrence


	// Counters

	double total_counter = 0;		// number of analysed passwords
	double total_filter = 0;		// number of analysed passwords after a filter

	minMax minMaxValue;				// save all genral data from passwords 


	// Security policy

	double nbSecurePassword = 0;	// all passwords that respect the security rules
	int minLength = 8;
	int minSpecial = 0;
	int minDigit = 1;
	int minLower = 1;
	int minUpper = 1;
};



/**
 * @brief analyse a letter in a password
 * @param letter : current letter
 * @param last_simplemask : last analyzed part of the simple masks 
 * @param simplemask_string : current simple mask
 * @param advancedmask_string : current advanced mask
 * @param policy : current number of digit, lower, upper and special for the current password
 * @param sizeAdvancedMask : size of the current advanced mask 
 * @param sizeSimpleMask : size of the current simple mask
 */
void analyse_letter(const char & letter, char & last_simplemask, std::wstring & simplemask_string, std::wstring & advancedmask_string, Policy & policy, int & sizeAdvancedMask, int & sizeSimpleMask);

/**
 * @brief analyse the characterset of the current password
 * @param charset : characterset of the current password
 * @param policy : current number of digit, lower, upper and special for the current password
 */
void analyse_charset(std::wstring & charset, const Policy & policy);

/**
 * @brief analyse a password
 * @param password : current password
 * @param c : container of all useful data of the password
 */
void analyze_password(const std::wstring & password, Container & c);

/**
 * @brief update minima and maxima of all general data from analysed passwords 
 * with the analysed passwords
 * @param minMaxValue : save all minima and maxima
 * @param pol : policy of the last analyzed password
 */
void updateMinMax(minMax & minMaxValue, const Policy & pol);

/**
* @brief action of all threads
* @param threadarg : all useful argument for the thread
*/
void * generate_stats_thread(void * threadarg);
