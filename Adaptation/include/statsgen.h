/**Copyright © 31/01/2018, Yannick BASS Jean-Baptiste JORAND
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
