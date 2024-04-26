#ifndef UTILBOX_H
#define UTILBOX_H

#include <string>
#include <vector>

#if defined(WIN32)
#include <time.h>
#else
//#define OS_CENTOS
#ifdef OS_CENTOS
#include <sys/time.h>
#endif
#define USE_CLOCK_GETTIME
#endif

#include <iostream>
#include <fstream>

int read_file(const char* pFileName, char** ppBuff, int& size, bool unicode);
int read_file(const char* pFileName, char** ppBuff);
bool readFileLine(const char* filename, std::vector<std::string>& listLine);
bool readFileSpecificLine(const char* filename, int num, std::string& line);
bool split(std::string strPhrase, std::vector<std::string>& listItems, std::string strDiv, int MaxSplitCount = 1);
bool splitList(std::string strPhrase, std::vector<std::string>& listItems, std::string strDivList);
bool splitWith(std::string strPhrase, std::vector<std::string>& listItems, std::string strDiv);
bool replaceText(std::string& text, std::string from, std::string to);
bool simpleCSVtokenizer(std::string strLine, std::vector<std::string>& tokens);
std::string trim(std::string str);
bool getTime(clock_t& time);

#endif // UTILBOX_H
