#include "Policy.h"

using namespace std;

static const map<uint, string> charset_names = Policy::charsetNames();


Policy::operator string() const {
	uint charset = 0;
	charset ^= (bool) digit << 0;
	charset ^= (bool) lower << 1;
	charset ^= (bool) upper << 2;
	charset ^= (bool) special << 3;

	map<uint, string>::const_iterator it = charset_names.find(charset);
	return it->second;
}

Policy::operator minMax() const {
	minMax m;
	m.maxlower = lower;
	m.maxupper = upper;
	m.maxdigit = digit;
	m.maxspecial = special;
	return m;
}

bool Policy::satisfies(const SecurityRules& sr, const uint& pwd_size) const {
	return 	pwd_size >= sr.minLength
			&& digit >= sr.minDigit
			&& lower >= sr.minLower
			&& upper >= sr.minUpper
			&& special >= sr.minSpecial;
}