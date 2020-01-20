#include "Policy.h"

using namespace std;

static const map<int, string> charset_names = Policy::charsetNames();


Policy::operator string() const {
	int charset = 0;
	charset ^= (bool) digit << 0;
	charset ^= (bool) lower << 1;
	charset ^= (bool) upper << 2;
	charset ^= (bool) special << 3;

	map<int, string>::const_iterator it = charset_names.find(charset);
	return it->second;
}

bool Policy::satisfies(const SecurityRules& sr, const int& pwd_size) const {
	return 	pwd_size >= sr.minLength
			&& digit >= sr.minDigit
			&& lower >= sr.minLower
			&& upper >= sr.minUpper
			&& special >= sr.minSpecial;
}