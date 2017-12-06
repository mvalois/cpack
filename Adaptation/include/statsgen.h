
#include <string>
#include <unordered_map>
#include <iostream>
#include <regex>


struct Policy{
	int digit;
	int lower;
	int upper;
	int special;
};


class Statsgen {

public:

	Statsgen() {};
	
	void showHelp();
	void setHiderare(int);
	void setTop(int);
	void setRegex(std::string);
	void setWithcount(bool);
	void setLimitSimplemask(int);
	void setLimitAdvancedmask(int);

	void analyze_password(const std::wstring&, int &, std::wstring &, std::wstring &, std::wstring &,Policy &);
	int generate_stats(const std::string &);
	void print_stats();

private:
	// functions

	void analyse_letter(const char &, char &, std::wstring &, std::wstring &, Policy &, int &, int &);
	void analyse_charset(std::wstring &, const Policy &);

	void updateMinMax(const Policy &);

	// Filters

	int hiderare = 0;
	int top = -1;
	std::wregex current_regex;
	bool use_regex = false;
	bool withcount = false;
	int limitSimplemask = 0;
	int limitAdvancedmask = 0;


	// dico

	std::unordered_map<int, int> stats_length;
	std::unordered_map<std::wstring, int> stats_simplemasks;
	std::unordered_map<std::wstring, int> stats_advancedmasks;
	std::unordered_map<std::wstring, int> stats_charactersets;


	double total_counter = 0;
	double total_filter = 0;


	// Minimum password complexity counters

	int mindigit = -1;
	int maxdigit = -1;
	int minlower = -1;
	int maxlower = -1;
	int minupper = -1;
	int maxupper = -1;
	int minspecial = -1;
	int maxspecial = -1;
};