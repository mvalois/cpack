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

#include <iostream>
#include <fstream>
#include "statsgen.h"
using namespace std;



int main(int argc,char* argv[]) {
	if(argc < 2){
		wcerr << "\nMissing arguments\n\t--help for information\n" << endl;
		return -1;
	}

	locale::global(locale(""));
	Statsgen test;
	string filename;


	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (string(argv[i]) == "--hiderare" || string(argv[i]) == "-hr") {
				test.setHiderare(1);
			}

			else if (string(argv[i]) == "--top" || string(argv[i]) == "-t") {
				if (argc == i+1) {
					wcerr << "Missing argument --top [value]" << endl;
					return -1;
				}
				test.setTop(atoi(argv[i+1]));
				i++;
			}

			else if (string(argv[i]) == "--regex" || string(argv[i]) == "-r") {
				if (argc == i+1) {
					wcerr << "Missing argument --top [value]" << endl;
					return -1;
				}
				test.setRegex(argv[i+1]);
				i++;
			}

			else if (string(argv[i]) == "--limitsimplemasks" || string(argv[i]) == "-lsm") {
				if (argc == i+1) {
					wcerr << "Missing argument --limitsimplemasks [value]" << endl;
					return -1;
				}
				test.setLimitSimplemask(atoi(argv[i+1]));
				i++;
			}

			else if (string(argv[i]) == "--limitadvancedmasks" || string(argv[i]) == "-lam") {
				if (argc == i+1) {
					wcerr << "Missing argument --limitadvancedmasks [value]" << endl;
					return -1;
				}
				test.setLimitAdvancedmask(atoi(argv[i+1]));
				i++;
			}

			else if (string(argv[i]) == "--help" || string(argv[i]) == "-h") {
				test.showHelp();
				return 0;
			}

			else if (string(argv[i]) == "--withcount" || string(argv[i]) == "-w") {
				test.setWithcount(true);
			}

			else {
				filename = argv[i];
			}
		}
	}



	if (filename == "") {
		wcerr << "Missing filename" << endl;
		return -1;
	}




	if (test.generate_stats(filename)) {
		test.print_stats();
	}
	
	
	return 0;
}
