#ifndef SECURITY_RULES_H
#define SECURITY_RULES_H


struct SecurityRules {
	uint64_t nbSecurePassword;
	int minLength;
	int minSpecial;
	int minDigit;
	int minLower;
	int minUpper;
};


#endif