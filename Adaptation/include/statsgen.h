
#include <string>
#include <map>
#include <iostream>

struct Policy{
	int digit;
	int lower;
	int upper;
	int special;
};


class Statsgen {

public:

	Statsgen();

	void analyze_password(const std::wstring&, int &, std::wstring &, std::wstring &, std::wstring &,Policy &);
	int generate_stats(const std::string &);
	void print_stats();

private:

	// Filters

	int minlength = 0;
	int maxlength = 0;
	std::map<std::wstring, int> charsets;
	std::map<std::wstring, int> simplemasks;
	bool quiet = false;
	bool debug = true;


	// dico

	std::map<int, int> stats_length;
	std::map<std::wstring, int> stats_simplemasks;
	std::map<std::wstring, int> stats_advancedmasks;
	std::map<std::wstring, int> stats_charactersets;


	// Ignore stats with less than 1% coverage

	bool hiderare = false;
	int filter_counter = 0;
	int total_counter = 0;


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