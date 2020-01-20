#ifndef POLICY_H
#define POLICY_H

#include <string>
#include <map>

#include "SecurityRules.h"

class Policy {
public:
	int digit = 0;
	int lower = 0;
	int upper = 0;
	int special = 0;
	operator std::string() const;
	static std::map<int, std::string> charsetNames() {
		std::map<int, std::string> names;
		names[1] = "numeric";
		names[2] = "loweralpha";
		names[3] = "loweralphanum";
		names[4] = "upperalpha";
		names[5] = "upperalphanum";
		names[6] = "mixedalpha";
		names[7] = "mixedalphanum";
		names[8] = "special";
		names[9] = "specialnum";
		names[10]= "loweralphaspecial";
		names[11]= "loweralphaspecialnum";
		names[12]= "upperalphaspecial";
		names[13]= "upperalphaspecialnum";
		names[14]= "mixedalphaspecial";
		names[15]= "all";
		return names;
	}
	bool satisfies(const SecurityRules& sr, const int& pwd_size) const;
};

#endif