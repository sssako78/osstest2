#ifndef CONFIGINFO_H
#define CONFIGINFO_H

#include <string>
#include <vector>
//#include "logutil.h"

#define MAX_DIR_LENGTH 2048

class ConfigInfo
{
public:
    explicit ConfigInfo();
    ~ConfigInfo();

    std::string getScript();
    std::string getOutDir();
    int getThreadNum();
private:
    std::string script_path;
    std::string output_path;
    int thread_num = 1;
#if defined(WIN32)
#define SEP "\\"
#define CONFIG_PATH "config" SEP "info.config"
#else
#define SEP "/"
#define CONFIG_PATH "config" SEP "info.config"
#endif
#define DEFAULT_OUTPUT_DIR "." SEP "output"
#define DEFAULT_SCRIPT_NAME "." SEP "test.txt"
#define CFG_LOG_LEVEL "log_level"
#define CFG_SCRIPT_PATH "script_path"
#define CFG_OUTPUT_PATH "output_path"
#define CFG_THREADNUM "thread_num"

    bool prepareConfig();
};
#endif // CONFIGINFO_H
