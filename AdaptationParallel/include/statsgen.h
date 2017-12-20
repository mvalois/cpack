/**
 * @file statsgen.h
 * @brief Statsgen class and all needed structures
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
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
 * @brief All needed variables to give to each thread
 */
struct thread_data {
	int thread_id;
	std::string filename;
	int lineBegin;
	int lineEnd;

	double total_counter = 0;
	double total_filter = 0;

	std::unordered_map<int, int> length;
	std::unordered_map<std::wstring, int> simplemasks;
	std::unordered_map<std::wstring, int> advancedmasks;
	std::unordered_map<std::wstring, int> charactersets;

	minMax minMaxValue;

	std::wregex current_regex;
	bool use_regex = false;
	bool withcount = false;
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
	 * @brief Calculate all statistics for a database which the
	 * name is given by argument
	 * @param : database's name
	 * @return 0 if error, 1 if success
	 */
	int generate_stats(const std::string &);
	
	/**
	 * @brief Print all calculated statistics
	 */
	void print_stats();

private:
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

	std::unordered_map<int, int> stats_length;					// Passwords' length linked to their occurrence
	std::unordered_map<std::wstring, int> stats_simplemasks;	// Passwords' simple mask linked to their occurrence
	std::unordered_map<std::wstring, int> stats_advancedmasks;	// Passwords' advanced mask linked to their occurrence
	std::unordered_map<std::wstring, int> stats_charactersets;	// Passwords' characterset linked to their occurrence


	// Counters

	double total_counter = 0;		// number of analysed passwords
	double total_filter = 0;		// number of analysed passwords after a filter

	minMax minMaxValue;				// save all genral data from passwords 
};


