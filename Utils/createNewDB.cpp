#include <fstream>
#include <string>
#include <iostream>
using namespace std;




void multiplication(int coeff) {
  wifstream readfile("../../BDD/rockyou.txt");
  wofstream outfile("../../BDD/rockyoucustom.txt");
  wstring line;

  while(readfile.good()) {
    getline(readfile, line);
    for (int i = 0; i<coeff; i++) {
      outfile << line << endl;
    }
  }
}



void division(int coeff) {
  wifstream readfile("../../BDD/rockyou.txt");
  wofstream outfile("../../BDD/rockyoucustom.txt");
  wstring line;
  int nbline = 0;
  
  while(readfile.good()) {
    getline(readfile, line);
    nbline++;
    if (nbline%coeff == 0) {
      outfile << line << endl;
    }
  }
}


int main(int argc,char* argv[]) {
  locale::global(locale(""));
  
  float val;
  
  wcout << "Coefficient (>1 pour multiplication  /  <1 pour division) : " << endl;
  cin >> val;

  
  if (val >= 1) {
    multiplication((int) val);
  } else {
    division((int) 1/val);
  }
    


  return 0;
}
