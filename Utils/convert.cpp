#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

int main(int argc,char* argv[]) {
	if(argc < 3)
	{
		wcerr << "\nMissing arguments\n" << endl;
		return -1;
	}

	locale::global(locale(""));
	string filename=argv[1];
	string outputfile=argv[2];

	wifstream readfile(filename);
	wofstream writefile(outputfile.c_str());
  wstring line;
	wstring key  = wstring(L"^\\s+");
  wstring repl = wstring(L"");

	wstring key2  = wstring(L"(\\d+) (.+)");
  double sum=0;
	while(readfile.good())
	{
  		getline(readfile, line);
		if (line.size() == 0) 
		{
    	continue;
    }

		wstring cleanLine = regex_replace(line, std::wregex(key),  repl);
		wsmatch m;
		regex_match(cleanLine,m,std::wregex(key2));
		if(m.size()>2)
		{
			int i=stoi(m[1]);
			sum+=i;
			for(int j=0;j<i;j++)
			{
				writefile << m[2] << endl;
			}
		}
  }
  wcout << L"Numbers of passwords :"<<sum<<endl;
}
