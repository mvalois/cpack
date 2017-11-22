
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

	void analyze_password(const std::wstring&, int &, std::wstring &, std::wstring &, std::wstring &,Policy &);
	int generate_stats(const std::string &);
	void print_stats();

private:
	// functions

	void analyse_letter(const char &, char &, std::wstring &, std::wstring &, Policy &, int &, int &);
	void analyse_charset(std::wstring &, const Policy &);

	void updateMinMax(const Policy &);
	void readResult(int, std::wstring, int &);
	void readResult(int, int, int &);

	// Filters

	int hiderare = 0;
	int top = -1;
	std::wregex current_regex;
	bool use_regex = false;
	bool withcount = false;


	// dico

	std::unordered_map<int, int> stats_length;
	std::unordered_map<std::wstring, int> stats_simplemasks;
	std::unordered_map<std::wstring, int> stats_advancedmasks;
	std::unordered_map<std::wstring, int> stats_charactersets;


	int total_counter = 0;
	int total_filter = 0;


	// Minimum password complexity counters

	int mindigit = -1;
	int maxdigit = -1;
	int minlower = -1;
	int maxlower = -1;
	int minupper = -1;
	int maxupper = -1;
	int minspecial = -1;
	int maxspecial = -1;


	// Time

	float timeLetterTotal = 0;
	int nbLetterCalc = 0;

	float timeCharsetTotal = 0;
	int nbCharsetCalc = 0;

	float timeMinMaxTotal = 0;
	int nbMinMaxCalc = 0;

	float timeGetlineTotal = 0;
	int nbGetlineCalc = 0;

	float timeTotal = 0;

};