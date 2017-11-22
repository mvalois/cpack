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
	/*wstring cleanKey  = wstring(L"^\\s+");
	wstring repl = wstring(L"");

	wstring parseKey  = wstring(L"\\s*(\\d+) (.+)");*/
	double sum=0;
	while(readfile.good())
	{
  		getline(readfile, line);
		if (line.size() == 0) 
		{
    	continue;
		}

		/*wstring cleanLine = regex_replace(line, std::wregex(cleanKey),  repl);
		wsmatch parsedPassword;
		regex_match(line,parsedPassword,std::wregex(parseKey));
		if(parsedPassword.size()>2)
		{
			int nbPasswords=stoi(parsedPassword[1]);
			
			for(int j=0;j<nbPasswords;j++)
			{
				writefile << parsedPassword[2] << endl;
			}
		}*/
		bool number=false;
		int i=0;
		for(i=0;i<line.length();i++)
		{
			if(iswdigit(line.at(i)))
			{
				number=true;
			}
			else if (!iswdigit(line.at(i)) && number)
			{
				break;
			}
		}
		wstring password=line.substr(i+1,line.length());
		int nbPasswords=stoi(line.substr(0,i));
		sum+=nbPasswords;
		for(int j=0;j<nbPasswords;j++)
		{
			writefile << password << endl;
		}
  }
  wcout << L"Numbers of passwords :"<<sum<<endl;
}
