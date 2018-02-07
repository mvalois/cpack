/**
 * @file statsgen.cpp
 * @brief Creation of all methods from Statsgen
 *
 * @author Jean-Baptiste Jorand
 * @author Yannick Bass
 */


#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <thread>
#include <pthread.h>

#include "statsgen.h"
#include "utils.h"

#include <QFile>
#include <QString>


using namespace std;





Statsgen::Statsgen() {}



void Statsgen::setFilename(QString name) {
    filename = name;
}


void Statsgen::setHiderare(int val) {
    hiderare = val;
}



void Statsgen::setTop(int val) {
    top = val;
}


void Statsgen::setRegex(string expr) {
    wstring tmp(expr.length(),L' ');
    copy(expr.begin(), expr.end(), tmp.begin());

    wregex tmp_reg(tmp);
    current_regex = tmp_reg;

    use_regex = true;
}


void Statsgen::setWithcount(bool val) {
    withcount = val;
}


void Statsgen::setLimitSimplemask(int val) {
    limitSimplemask = val;
}


void Statsgen::setLimitAdvancedmask(int val) {
    limitAdvancedmask = val;
}


void Statsgen::setNbThread(int nb) {
    int max = thread::hardware_concurrency();
    if (nb > max) {
        nbThread = max;
    } else {
        nbThread = nb;
    }
}


void Statsgen::setSecurityRules() {
    wcout << "\nMinimal length of a password:" << endl;
    cin >> minLength;

    wcout << "\nMinimum of special characters in a password:" << endl;
    cin >> minSpecial;

    wcout << "\nMinimum of digits in a password:" << endl;
    cin >> minDigit;

    wcout << "\nMinimum of lower characters in a password:" << endl;
    cin >> minLower;

    wcout << "\nMinimum of upper characters in a password:" << endl;
    cin >> minUpper;
}


void Statsgen::setSecurityRules(int length,int special,int digit,int upper,int lower) {
    minLength=length;
    minSpecial=special;
    minDigit=digit;
    minUpper=upper;
    minLower=lower;
}



void Statsgen::generate_stats() {
    int nbline = nbline_file(filename);

    int rc;
    int i;
    pthread_t threads[nbThread];
    struct thread_data td[nbThread];

    pthread_attr_t attr;
    void *status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for( i = 0; i < nbThread; i++ ) {
        td[i].filename = filename;
        td[i].thread_id = i + 1;
        td[i].current_regex = current_regex;
        td[i].use_regex = use_regex;
        td[i].withcount = withcount;

        td[i].lineBegin = i*(nbline/nbThread) + 1;
        td[i].lineEnd = (i+1)*nbline/nbThread;
        if (i > 0) {
            while (td[i].lineBegin <= td[i-1].lineEnd) {
                td[i].lineBegin++;
            }
        }
        

        td[i].sr.nbSecurePassword = 0;
        td[i].sr.minLength = minLength;
        td[i].sr.minSpecial = minSpecial;
        td[i].sr.minDigit = minDigit;
        td[i].sr.minLower = minLower;
        td[i].sr.minUpper = minUpper;

        wcout << "Thread " << td[i].thread_id << " analyse : " << td[i].lineBegin << " --> " << td[i].lineEnd << endl;
        rc = pthread_create(&threads[i], NULL, generate_stats_thread, (void *)&td[i] );
      
        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);
    for( i = 0; i < nbThread; i++ ) {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            cout << "Error:unable to join," << rc << endl;
            exit(-1);
        }
    }


    Policy policyMin;
    Policy policyMax;

	for(i=0;i<nbThread;i++)	{
		total_counter+=td[i].total_counter;
		total_filter+=td[i].total_filter;

        policyMin.digit = td[i].minMaxValue.mindigit;
        policyMin.lower = td[i].minMaxValue.minlower;
        policyMin.upper = td[i].minMaxValue.minupper;
        policyMin.special = td[i].minMaxValue.minspecial;

        policyMax.digit = td[i].minMaxValue.maxdigit;
        policyMax.lower = td[i].minMaxValue.maxlower;
        policyMax.upper = td[i].minMaxValue.maxupper;
        policyMax.special = td[i].minMaxValue.maxspecial;

        nbSecurePassword += td[i].sr.nbSecurePassword;

        updateMinMax(minMaxValue, policyMin);
        updateMinMax(minMaxValue, policyMax);

		
		for(auto it=td[i].length.begin();it!=td[i].length.end();it++)
		{
			stats_length[it->first]+=it->second;
		}
		td[i].length.clear();
		for(auto it=td[i].charactersets.begin();it!=td[i].charactersets.end();it++)
		{
			stats_charactersets[it->first]+=it->second;
		}
		td[i].charactersets.clear();
		for(auto it=td[i].simplemasks.begin();it!=td[i].simplemasks.end();it++)
		{
			stats_simplemasks[it->first]+=it->second;
		}
		td[i].simplemasks.clear();
		for(auto it=td[i].advancedmasks.begin();it!=td[i].advancedmasks.end();it++)
		{
			stats_advancedmasks[it->first]+=it->second;
		}
		td[i].advancedmasks.clear();
	}



    if (!total_counter) {
        wcerr << "Empty file or not existing file" << endl;
    }
}




void Statsgen::print_stats() {
    int count;
    float perc = (float) 100 * (total_filter / total_counter);

    wofstream writeFile("result.txt");

    writeFile << "\n\tSelected " << total_filter << " on " << total_counter << " passwords\t("
        << perc << " %)" << endl;


    writeFile << "\nSecurity rules : " << endl;
    writeFile << "\tMinimal length of a password: " << minLength << endl;
    writeFile << "\tMinimum of special characters in a password: " << minSpecial << endl;
    writeFile << "\tMinimum of digits in a password: " << minDigit << endl;
    writeFile << "\tMinimum of lower characters in a password: " << minLower << endl;
    writeFile << "\tMinimum of upper characters in a password: " << minUpper << endl;

    float perce = (float) 100 * (nbSecurePassword / total_counter);
    writeFile << "\n\t\t--> " << nbSecurePassword << " passwords\t(" << perce << " %) respect the security rules\n" << endl;


    writeFile << "\nmin - max\n" << endl;
    writeFile << setw(43) << right << "digit:  "
            << setw(2) << right << minMaxValue.mindigit << " - " << minMaxValue.maxdigit << endl;
    writeFile << setw(43) << right << "lower:  "
            << setw(2) << right << minMaxValue.minlower << " - " << minMaxValue.maxlower << endl;
    writeFile << setw(43) << right << "upper:  "
            << setw(2) << right << minMaxValue.minupper << " - " << minMaxValue.maxupper << endl;
    writeFile << setw(43) << right << "special:  "
            << setw(2) << right << minMaxValue.minspecial << " - " << minMaxValue.maxspecial << endl;       



    writeFile << "\nStatistics relative to length: \n" << endl;
    showMap(writeFile, stats_length, top, total_counter, hiderare, count);

    writeFile << "\nStatistics relative to charsets: \n" << endl;
    showMap(writeFile, stats_charactersets, -1, total_counter, hiderare, count);


    writeFile << "\nStatistics relative to simplemasks: \n" << endl;
    showMap(writeFile, stats_simplemasks, top, total_counter, hiderare, count);

    if (limitSimplemask > 0) {
        wcout << endl;
        readResult(writeFile, stats_simplemasks[L"othermasks"], L"othermasks", count, total_counter, hiderare);
    }


    writeFile << "\nStatistics relative to advancedmask: \n" << endl;
    showMap(writeFile, stats_advancedmasks, top, total_counter, hiderare, count);
    
    if (limitAdvancedmask > 0) {
        wcout << endl;
        readResult(writeFile, stats_advancedmasks[L"othermasks"], L"othermasks", count, total_counter, hiderare);
    }
}




void Statsgen::initGraphicalStats(QBarSeries * barLength, QPieSeries * pieCharset, double & percentageTotal, double & percentageSecurity, double & total, double & filter) {
    total = total_counter;
    filter = total_filter;
    percentageTotal = (double) 100 * (total_filter / total_counter);
    percentageSecurity = (double) 100 * (nbSecurePassword / total_counter);

    /* LENGTH HISTOGRAM */
    multimap<double, int> reverseL = flip_map<int>(stats_length);
    double percentageL;
    double nbHideL = 0;

    MapIterator<double, int> itL;
    for(itL = reverseL.end(); itL != reverseL.begin(); itL--) {
        if (itL == reverseL.end()) continue;

        percentageL = (double) (100*itL->first) / total_counter;
        if (percentageL >= 2) {
            QBarSet *set = new QBarSet(QString::number(itL->second));
            *set << itL->first;
            barLength->append(set);
        } else {
            nbHideL += itL->first;
        }
    }

    QBarSet *set = new QBarSet("Other lengths");
    *set << nbHideL;
    barLength->append(set);


    /* CHARSET PIECHART */
    multimap<double, wstring> reverseC = flip_map<wstring>(stats_charactersets);
    int top = 0;
    double nbHideC = 0;

    MapIterator<double, wstring> itC;
    for(itC = reverseC.end(); itC != reverseC.begin(); itC--) {
        if (itC == reverseC.end()) continue;
        top++;
        if (top <= 5) {
            pieCharset->append(QString::fromStdWString(itC->second), itC->first);
        } else {
            nbHideC += itC->first;
        }
    }
    pieCharset->append("Other charsets", nbHideC);
}




void analyse_letter(const char & letter, char & last_simplemask, wstring & simplemask_string, wstring & advancedmask_string, Policy & policy, int & sizeAdvancedMask, int & sizeSimpleMask) {
    sizeAdvancedMask++;

    if (letter >= L'0' && letter <= L'9') {
        policy.digit++;
        advancedmask_string += L"?d";
        if (last_simplemask != 'd') {
            sizeSimpleMask++;
            simplemask_string += L"digit";
            last_simplemask = 'd';
        }
    }
    else if(letter >= L'a' && letter <= L'z') {
        policy.lower++;
        advancedmask_string += L"?l";
        if (last_simplemask != 'l') {
            sizeSimpleMask++;
            simplemask_string += L"lower";
            last_simplemask = 'l';
        }
    }
    else if(letter >= L'A' && letter <= L'Z') {
        policy.upper++;
        advancedmask_string += L"?u";
        if (last_simplemask != 'u') {
            sizeSimpleMask++;
            simplemask_string += L"upper";
            last_simplemask = 'u';
        }
    }
    else {
        policy.special++;
        advancedmask_string += L"?s";

        if (last_simplemask != 's') {
            sizeSimpleMask++;
            simplemask_string += L"special";
            last_simplemask = 's';
        }
    }
}


void analyse_charset(wstring & charset, const Policy & policy) {
    if (policy.digit && !policy.lower && !policy.upper && !policy.special) {
        charset = L"numeric";
    }
    else if (!policy.digit && policy.lower && !policy.upper && !policy.special) {
        charset = L"loweralpha";
    }
    else if (!policy.digit && !policy.lower && policy.upper && !policy.special) {
        charset = L"upperalpha";
    }
    else if (!policy.digit && !policy.lower && !policy.upper && policy.special) {
        charset = L"special";
    }
    else if (!policy.digit && policy.lower && policy.upper && !policy.special) {
        charset = L"mixedalpha";
    }
    else if (policy.digit && policy.lower && !policy.upper && !policy.special) {
        charset = L"loweralphanum";
    }
    else if (policy.digit && !policy.lower && policy.upper && !policy.special) {
        charset = L"upperalphanum";
    }
    else if (!policy.digit && policy.lower && !policy.upper && policy.special) {
        charset = L"loweralphaspecial";
    }
    else if (!policy.digit && !policy.lower && policy.upper && policy.special) {
        charset = L"upperalphaspecial";
    }
    else if (policy.digit && !policy.lower && !policy.upper && policy.special) {
        charset = L"specialnum";
    }

    else if (!policy.digit && policy.lower && policy.upper && policy.special) {
        charset = L"mixedalphaspecial";
    }
    else if (policy.digit && !policy.lower && policy.upper && policy.special) {
        charset = L"upperalphaspecialnum";
    }
    else if (policy.digit && policy.lower && !policy.upper && policy.special) {
        charset = L"loweralphaspecialnum";
    }
    else if (policy.digit && policy.lower && policy.upper && !policy.special) {
        charset = L"mixedalphanum";
    }

    else {
        charset = L"all";
    }
}


void analyze_password(const wstring & password, Container & c, SecurityRules & sr) {
    c.pass_length = password.size();

    char last_simplemask = 'a';
    int sizeAdvancedMask = 0;
    int sizeSimpleMask = 0;
    wchar_t letter;

    for (int i=0; i < c.pass_length; i++) {
        letter = password[i];
        analyse_letter(letter, last_simplemask, c.simplemask_string, c.advancedmask_string, c.pol, sizeAdvancedMask, sizeSimpleMask);
    }

    analyse_charset(c.characterset, c.pol);

    if ( (c.pass_length >= sr.minLength) && 
         (c.pol.digit >= sr.minDigit) && 
         (c.pol.lower >= sr.minLower) && 
         (c.pol.upper >= sr.minUpper) && 
         (c.pol.special >= sr.minSpecial) ) {
        sr.nbSecurePassword++;
    }
}



void updateMinMax(minMax & minMaxValue, const Policy & pol) {
    if (minMaxValue.mindigit == -1 || minMaxValue.mindigit > pol.digit) {
        minMaxValue.mindigit = pol.digit;
    }
    if (minMaxValue.maxdigit == -1 || minMaxValue.maxdigit < pol.digit) {
        minMaxValue.maxdigit = pol.digit;
    }

    if (minMaxValue.minlower == -1 || minMaxValue.minlower > pol.lower) {
        minMaxValue.minlower = pol.lower;
    }
    if (minMaxValue.maxlower == -1 || minMaxValue.maxlower < pol.lower) {
        minMaxValue.maxlower = pol.lower;
    }

    if (minMaxValue.minupper == -1 || minMaxValue.minupper > pol.upper) {
        minMaxValue.minupper = pol.upper;
    }
    if (minMaxValue.maxupper == -1 || minMaxValue.maxupper < pol.upper) {
        minMaxValue.maxupper = pol.upper;
    }

    if (minMaxValue.minspecial == -1 || minMaxValue.minspecial > pol.special) {
        minMaxValue.minspecial = pol.special;
    }
    if (minMaxValue.maxspecial == -1 || minMaxValue.maxspecial < pol.special) {
        minMaxValue.maxspecial = pol.special;
    }
}



void * generate_stats_thread(void * threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;

    QFile file(my_data->filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream flux(&file);

    QString qline;
    wstring line;
    int nbline = 0;

    while(! flux.atEnd()) {
        ++nbline;
        qline = flux.readLine();

        if (nbline < my_data->lineBegin) {
            continue;
        }
        if (nbline > my_data->lineEnd) {
            break;
        }


        line = qline.toStdWString();
        if (line.size() == 0) {
            continue;
        }

        Container c;

        if (my_data->withcount) {
            uint i = 0;
            bool number=false;
            for(i=0; i < line.length(); i++) {
                if(iswdigit(line.at(i))) {
                    number=true;
                }
                else if (!iswdigit(line.at(i)) && number) {
                    break;
                }
            }
            wstring password = line.substr(i+1,line.length());
            int nbPasswords = stoi(line.substr(0,i));
            
            my_data->total_counter += nbPasswords;

            if ( !my_data->use_regex || (my_data->use_regex && regex_match(password,my_data->current_regex)) ) {
                my_data->total_filter += nbPasswords;
                analyze_password(password, c, my_data->sr);

                my_data->length[ c.pass_length ] += nbPasswords;
                my_data->charactersets[ c.characterset ] += nbPasswords;
                my_data->simplemasks[ c.simplemask_string ] += nbPasswords;
                my_data->advancedmasks[ c.advancedmask_string ] += nbPasswords;
            }
        }
        else {
            my_data->total_counter++;
            if ( !my_data->use_regex || (my_data->use_regex && regex_match(line,my_data->current_regex)) ) {
                analyze_password(line, c, my_data->sr);
                
                my_data->total_filter++;

                my_data->length[ c.pass_length ] += 1;
                my_data->charactersets[ c.characterset ] += 1;
                my_data->simplemasks[ c.simplemask_string ] += 1;
                my_data->advancedmasks[ c.advancedmask_string ] += 1;
            }
        }
        updateMinMax(my_data->minMaxValue, c.pol);
    }

    pthread_exit(NULL);
}

