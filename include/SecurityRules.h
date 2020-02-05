#ifndef SECURITY_RULES_H
#define SECURITY_RULES_H


struct SecurityRules {
	uint64_t nbSecurePassword;
	uint minLength;
	uint minSpecial;
	uint minDigit;
	uint minLower;
	uint minUpper;
	bool operator==(const SecurityRules& o) const;
};


#endif