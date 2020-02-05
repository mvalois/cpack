#include <cassert>

#include "Statsgen.h"

#define TESTFILE "ressources/rockyou.txt"

using namespace std;

int main(){
	Statsgen s0(TESTFILE);
	s0.generate_stats();
	Statsgen s4(TESTFILE);
	s4.setNbThread(4);
	s4.generate_stats();
	assert(s0 == s4);
	return 0;
}