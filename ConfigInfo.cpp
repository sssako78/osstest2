#include "ConfigInfo.h"
#include "utils/utilbox.h"

#if defined(WIN32) || defined(WINCE)
#pragma warning(disable:4996)
#include <sys/stat.h>
#include <direct.h>
#else
#include <sys/stat.h>
#include <string.h>
#endif

#include <iostream>
#include <fstream>



using namespace std;

std::string ConfigInfo::getScript()
{
    return script_path;
}

std::string ConfigInfo::getOutDir()
{
    return output_path;
}

int ConfigInfo::getThreadNum()
{
    return thread_num;
}


bool ConfigInfo::prepareConfig()
{
    std::vector<std::string> listLine;
    readFileLine(CONFIG_PATH, listLine);

    if (listLine.size() > 0)
    {
        for (std::vector<std::string>::iterator iter = listLine.begin(); iter != listLine.end(); iter++)
        {
            std::vector<std::string> splitList;

            split((*iter), splitList, "=");
            if (splitList.size() == 2)
            {
                std::string strConfigName = trim(splitList.at(0));
                if (strConfigName.compare(CFG_SCRIPT_PATH) == 0)
                {
                    std::string strScriptPath = trim(splitList.at(1));
                    bool bValid = false;
                    
#if 1
                    script_path = strScriptPath.c_str();
#else
                    if (!strScriptPath.empty())
                    {
                        struct stat info;
                        if (stat(strScriptPath.c_str(), &info) == 0)
                        {
                            if (info.st_mode & S_IFREG)
                            {
                                bValid = true;
                            }
                        }
                    }

                    if(bValid) script_path = strScriptPath.c_str();
                    //else strcpy(script_path, DEFAULT_SCRIPT_NAME);
#endif
                }
                else if (strConfigName.compare(CFG_OUTPUT_PATH) == 0)
                {
                    std::string strConfigOutDir = trim(splitList.at(1));
                    std::string strOutDir = DEFAULT_OUTPUT_DIR;
                    if (!strConfigOutDir.empty())
                    {
                        strConfigOutDir.erase(strConfigOutDir.find_last_not_of("\\/") + 1);
                        struct stat info;
                        if (stat(strConfigOutDir.c_str(), &info) == 0)
                        {
                            if (info.st_mode & S_IFDIR)
                            {
                                output_path = strConfigOutDir;
                                continue;
                            }
                        }
                        strOutDir = strConfigOutDir;
                    }
#if defined(WIN32)
                    int ret = mkdir(strOutDir.c_str());
#else
                    int ret = mkdir(strOutDir.c_str(), 0777);
#endif
                    if (ret == -1)  //incorrect path name
                    {
#if defined(WIN32)
                        mkdir(DEFAULT_OUTPUT_DIR);
#else
                        mkdir(DEFAULT_OUTPUT_DIR, 0777);
#endif
                        output_path = DEFAULT_OUTPUT_DIR;
                    }
                    else
                    {
                        output_path = strOutDir.c_str();
                    }
                }
                else if (strConfigName.compare(CFG_THREADNUM) == 0)
                {
                    thread_num = std::stoi(splitList.at(1));
                }
            }
        }
    }

    listLine.clear();
    return true;
}

ConfigInfo::ConfigInfo()
{
    output_path = "output";
    prepareConfig();
}

ConfigInfo::~ConfigInfo()
{

}
