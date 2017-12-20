#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;



void multiplication(int coeff, const string & input, const string & output) {
  wifstream readfile(input);
  wofstream outfile(output);
  wstring line;
  wstring add;

  while(readfile.good()) {
    getline(readfile, line);
    if (line.size() == 0) {
      continue;
    }

    for (int i = 0; i<coeff; i++) {
      add = line;
      for (int j = 0; j<i; j++) {
        add += to_wstring(j);
      }
      outfile << add << endl;
    }
  }
}



void division(int coeff, const string &input, const string & output) {
  wifstream readfile(input);
  wofstream outfile(output);
  wstring line;
  int nbline = 0;
  
  while(readfile.good()) {
    getline(readfile, line);
    if (line.size() == 0) {
      continue;
    }
    nbline++;
    if (nbline%coeff == 0) {
      outfile << line << endl;
    }
  }
}


int main(int argc,char* argv[]) {
  locale::global(locale(""));
  
  if(argc < 3){
    wcerr << "\nNeed two arguments\n\t- name of the input database\n\t- name of the output database\n" << endl;
    return -1;
  }

  string input = argv[1];
  string output = argv[2];
  float val;
  
  wcout << "Coefficient (>1 pour multiplication  /  <1 pour division) : " << endl;
  cin >> val;

  
  if (val >= 1) {
    multiplication((int) val, input, output);
  } else {
    division((int) 1/val, input, output);
  }
    


  return 0;
}
