#include <iostream>
#include <fstream>
#include "statsgen.h"
using namespace std;



int main() {
	locale::global(locale(""));

	Statsgen test;

	string filename = "/home/jorand/Documents/Projet_3A/BDD/rockyou.txt";

	test.generate_stats(filename);
	test.print_stats();
	
	return 0;
}
