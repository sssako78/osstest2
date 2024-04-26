#if defined(WIN32) || defined(WINCE)
#pragma warning(disable:4996)
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "utilbox.h"
#include <string.h>
//#include <iostream>
//#include <fstream>

#undef max
#include <limits>
#include <mutex>
std::mutex g_mutex;

using namespace std;

int read_file(const char* pFileName, char** ppBuff, int& size, bool unicode)
{
    FILE* fp = nullptr;
    bool isBE = false;

    if (pFileName == nullptr) return -1;
    if (unicode) {
        FILE* tfp = fopen(pFileName, "r");
        if (tfp == nullptr) return -1;
        int c = fgetc(tfp);
        fclose(tfp);
        // only support little endian
        if (c != 0xFF) isBE = true;
    }
    
    fp = fopen(pFileName, "rb");
    if (fp == nullptr) return -1;

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    if (size <= 0) {
        fclose(fp);
        return -1;
    }

    fseek(fp, 0L, SEEK_SET);
    *ppBuff = (char*)calloc(size + 1, sizeof(char));
    memset(*ppBuff, 0x00, sizeof(char) * (size + 1));
    fread(*ppBuff, 1, size, fp);
    fclose(fp);

    //swap for unicode BE..
    return size;
}

int read_file(const char* pFileName, char** ppBuff)
{
    int size;
    return read_file(pFileName, ppBuff, size, false);
}

bool readFileLine(const char* filename, std::vector<std::string>& listLine)
{
    bool ret = false;
    std::ifstream in(filename);

    if (in.is_open()) {
        while (!in.eof()) {
            std::string strLine("");
            getline(in, strLine);
            replaceText(strLine, "\r", "");
            listLine.push_back(strLine);
        }
        in.close();
        ret = true;
    }

    return ret;
}

bool readFileSpecificLine(const char* filename, int num, std::string& line)
{
    std::ifstream in(filename);

    if (in.is_open()) {
        in.seekg(std::ios::beg);
        for (int i = 0; i < num; ++i) {
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        //std::string strLine("");
        getline(in, line);
        replaceText(line, "\r", "");    // exception for window's text
        in.close();
    }

    return true;
}

bool split(std::string strPhrase, std::vector<std::string>& listItems, std::string strDiv, int MaxSplitCount)
{
    size_t index=0, curCnt=0;
    std::string strNext = strPhrase;
    while ((index = strNext.find(strDiv)) != std::string::npos) {
        std::string strPre = strNext.substr(0, index);
        strNext = strNext.substr(index + strDiv.size());
        listItems.push_back(strPre);
        curCnt++;
        if ((MaxSplitCount > 1)
            && (curCnt >= MaxSplitCount - 1)) {
            break;
        }
    }
    listItems.push_back(strNext);

    return true;
}

bool splitList(std::string strPhrase, std::vector<std::string>& listItems, std::string strDivList)
{
    if (strDivList.size() == 0) return false;

    size_t index = 0;
    std::string strNext = strPhrase;
    while ((index = strNext.find(strDivList.at(0))) != std::string::npos) {
        std::string strPre;
        strPre = strNext.substr(0, index);
        std::vector<std::string> preListItems;
        if (splitList(strPre, preListItems, strDivList.substr(1))) {
            int sizeofList = (int)listItems.size();
            listItems.resize(sizeofList + (int)(preListItems.size()));
            std::copy(preListItems.begin(), preListItems.end(), listItems.begin() + sizeofList);
        }
        preListItems.clear();
        strNext = strNext.substr(index + 1);
    }
    listItems.push_back(strNext);

    return true;
}

bool splitWith(std::string strPhrase, std::vector<std::string>& listItems, std::string strDiv)
{
    size_t index = 0;
    std::string strNext = strPhrase;
    while ((index = strNext.find(strDiv)) != std::string::npos) {
        std::string strPre, strWith;
        strPre = strNext.substr(0, index);
        if (strPre.size() > 0) listItems.push_back(strPre);
        strWith = strNext.substr(index, strDiv.size());
        listItems.push_back(strWith);
        strNext = strNext.substr(index + strDiv.size());
    }
    if (strNext.size() > 0) listItems.push_back(strNext);

    return true;
}

bool replaceText(std::string& text, std::string from, std::string to)
{
    size_t iPos = text.find(from);
    while (iPos < std::string::npos) {
        text.replace(iPos, from.size(), to);
        iPos = text.find(from, iPos + to.size());
    }

    return true;
}

bool simpleCSVtokenizer(std::string strLine, std::vector<std::string>& tokens)
{
    size_t index;
    std::string strNext = strLine;
    replaceText(strNext, "\"\"", "\"");

    while ((index = strNext.find(",")) != std::string::npos) {
        std::string strPre;
        strPre = strNext.substr(0, index);

        if (strPre.find_first_of("\"") == 0) {
            size_t indexEnd;
            if ((indexEnd = strNext.find("\",")) > 0) {
                strPre = strNext.substr(1, indexEnd - 1);
                strNext = strNext.substr(indexEnd + 2);
                tokens.push_back(strPre);
            } else {
                strNext = strNext.substr(index + 1);
                tokens.push_back(strPre);
            }
        } else {
            strNext = strNext.substr(index + 1);
            tokens.push_back(strPre);
        }
    }

    tokens.push_back(strNext);
    return true;
}

std::string trim(std::string str)
{
    std::string whitespaces(" \t\f\v\n\r");
    int found = (int)str.find_last_not_of(whitespaces);
    if (found != std::string::npos) str.erase(found + 1);
    found = (int)str.find_first_not_of(whitespaces);
    if (found != std::string::npos) str.erase(0, found);

    return str;
}

bool getTime(clock_t& time)
{
#ifdef USE_CLOCK_GETTIME
#define MICRO_PER_SEC ((__clock_t) 1000000)
#define NANO_PER_MICRO ((__clock_t) 1000)
    struct timespec tsTime;
    if (clock_gettime(CLOCK_MONOTONIC, &tsTime) == -1)
    {
        return false;
    }

    //printf("tsTime.tv_sec=%d", tsTime.tv_sec);
    time = tsTime.tv_sec * MICRO_PER_SEC + tsTime.tv_nsec / NANO_PER_MICRO;
    return true;
#else
    time = clock();
    return true;
#endif
}
