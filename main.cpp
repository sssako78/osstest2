#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <regex>
#include <sys/stat.h>
#include <map>
#include <fstream>
#include <thread>
#include <condition_variable>

#if defined(WIN32)
#pragma warning(disable:4996)
//#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <direct.h> 
#include <io.h>
#else
#define OS_CENTOS
#include <unistd.h>
#include <dlfcn.h>
#include <stdbool.h>
#ifdef OS_CENTOS
#include <cstring>      //centos7
#include <sys/time.h>
#define NOT_SUPPORT_REGEX
#endif
#endif


#include "utils/singleton.h"
#define GET_INST(X) Singleton<X>::GetInstance()
#define DEST_INST(X) Singleton<X>::DestroyInstance()

#include "utils/utilbox.h"
#include "ConfigInfo.h"

//#define SUPPORT_SIGNAL
//#define USE_BUFTOFILE
#define TARGET_ENGINE
#ifdef TARGET_ENGINE
#include "include/engine.h"
#define ENGINE_LANGUAGE "ENG"
#define ENGINE_SPEKAERID 105    //ashley
#endif

//#define USE_BUFTOFILE
#define MAKE_LOG_FILE



//#define WAIT_FOR_THREAD_CREATION
//#define USE_STARTLINE_FOR_TEST
#define STARTLINE_NUM   1000

typedef struct SYNTHTIME_INFO
{
    size_t nThreadID = 0;
    size_t nSynthID = 0;
    clock_t tBase = 0;                        //CLOCKS_PER_SEC
    clock_t tFirstBuf = 0;                    //CLOCKS_PER_SEC
    clock_t tEndSynth = 0;                  //CLOCKS_PER_SEC
    uint32_t nSizeofBuf = 0;                 //byte
    uint32_t nTimeofPlayback = 0;            //milli-sec.

} ST_SYNTHTIME_INFO;

typedef struct RESULT_INFO
{
    size_t nThreadCnt = 0;
    size_t nInitialThreadCnt = 0;
    clock_t tStart = 0;                        //CLOCKS_PER_SEC
    clock_t tFinish = 0;                    //CLOCKS_PER_SEC
    std::vector<ST_SYNTHTIME_INFO> vecSynthTime;
} ST_RESULT_INFO;


#define CHANNEL_SIZE 1
#define BYTE_PER_SAMPLE 2
#define MILLI_SEC (CLOCKS_PER_SEC / 1000)
//#define MICRO_IN_MILLI 1000
#define MILLI_IN_SEC 1000


#if defined(WIN32)
#define SEP "\\"
#define SYNTHTEXT_PATH "inputtext" SEP
#define PCMOUTPUT_NAME "output"

#ifdef TARGET_ENGINE
#define LOADSYM GetProcAddress
#define H_MOD HMODULE
#define LIBPATH "C:\\Program Files\\VW\\VT\\Ashley\\G22\\bin\\vt_eng.dll"
#define DBPATH "C:\\Program Files\\VW\\VT\\Ashley\\G22"
#endif
#else
#define SEP "/"
#define SYNTHTEXT_PATH "inputtext" SEP
#define PCMOUTPUT_NAME "output"

#ifdef TARGET_ENGINE
#define LOADSYM dlsym
#define H_MOD void *
#define LIBPATH "/usr/vt/ashley/G22/libvt_eng.so"
#define DBPATH "/usr/vt/ashley/G22"
#endif
#endif
char mTimeInfo[1024];
char mResultPCMPath[1024];
#define EXCEPTIONLIST_TEXT "exceptionlist.txt"

std::vector<std::string> m_VecExceptionList;
std::vector<std::string> m_VecInputText;
int m_curTextIdx = 0;
std::string m_speaker;
std::string m_type;
std::string m_text;
std::ifstream mTextin;

#define MAX_THREAD_NUM 30
#define MAX_SYNTH_TEXT 2048
#define MAX_LOGSIZE 2048
#define WAIT_FOR_THREAD_CREATION_SECOND 1
int mThreadCnt = 0;
ST_RESULT_INFO mVecResult;

char msgLog[MAX_LOGSIZE*4];
#ifdef MAKE_LOG_FILE
FILE *fpLog = NULL;
char logName[1024];
std::mutex log_mutex;
#endif
void makeLogFile(int mode, char* msg);

bool doSynthesize(int threadNum);
bool printResult(int nThreadnum);


#ifdef WAIT_FOR_THREAD_CREATION
#include <iostream>
#include <chrono>

std::condition_variable cv;
std::mutex cv_m;
#endif
std::mutex result_mutex;



#if defined(SUPPORT_SIGNAL)
#include <signal.h>

static void sighandler(int sig)
{
    printf("sighandler triggered:%d\n", sig);
    sprintf(msgLog, "sighandler triggered:%d\r\n", sig);
    makeLogFile(2, msgLog);
    exit(1);
}
#endif


#define CPU_MONITOR
#if !defined(WIN32) && defined(CPU_MONITOR)
#define CPU_MONITOR_USE_TOP
#define CPU_MONITOR_INTERVAL 30000  //1000  //300
std::vector<double> mVecCpurate;
std::vector<int> mVecMem;

pid_t mPid;
char pidfile[512 + 1];
unsigned long long m_iUser, m_iNice, m_iSystem, m_iIdle;
bool m_bProcStart;

#define MAX_READ_LINE 3
#define MAX_READ_LINE_SIZE 1024
bool cpu_getcmd(double& CPUrate, int& MEMres)
{
    bool ret = true;
#ifdef CPU_MONITOR_USE_TOP   //top
    std::string data;
    FILE* stream = NULL;
    char buffer[MAX_READ_LINE_SIZE];

    char garbage[16], user[16];
    int pid = 0, pr = 0, ni = 0, virt = 0, res = 0, shr = 0;
    char s = 0;
    double cpurate = 0.0;

    memset(buffer, 0x00, sizeof(char) * MAX_READ_LINE_SIZE);
    stream = popen(pidfile, "r");
    if (stream) {
        fgets(buffer, MAX_READ_LINE_SIZE, stream);
        pclose(stream);
#if 0   //debug
        for (int i = 0; i < strlen(buffer); i++) {
            printf("%d ", buffer[i]);
        }

        printf("%s\n", buffer);
#endif
        if (*buffer != NULL) {
            int n = sscanf(&buffer[6], "%d %s %d %d %d %d %d %c %lf", &pid, user, &pr, &ni, &virt, &res, &shr, &s, &cpurate);
            if (n == -1) {
                printf("reading top command error!\n");
                ret = false;
            } else {
                CPUrate = cpurate;
                MEMres = res;
            }
        }
        else {
            //printf("reading top command buffer is null!\n");
            ret = false;
        }
    }
    else {
        printf("top coommand pipe open error!\n");
        ret = false;
    }
#else   //ps reference
    std::string data;
    FILE* stream;

    char buffer[MAX_READ_LINE][MAX_READ_LINE_SIZE];
    int idxLine = 0;
    double cpurate = 0.0;
    memset(buffer, 0x00, sizeof(char) * MAX_READ_LINE * MAX_READ_LINE_SIZE);
    stream = popen(pidfile, "r");
    if (stream) {
        while (!feof(stream)) {
            fgets(buffer[idxLine], MAX_READ_LINE_SIZE, stream);
            idxLine++;
        }
        pclose(stream);

        int n = sscanf(buffer[1], "%lf", &cpurate);
        if (n == -1) {
            printf("reading ps command error!\n");
        }
        else {
            CPUrate = cpurate;
            MEMres = -1;
        }
    }
#endif
    return ret;
}
#if 0   //calc by proc
bool cpu_getbyproc(double& dbValue)
{
    dbValue = 0.0;

    // read cpu usages
    FILE* fd = fopen("/proc/stat", "r");
    if (fd == NULL) {
        std::cerr << "/proc/stat open error!" << "\n";
        return false;
    }

    unsigned long long iUser, iNice, iSystem, iIdle;

    int n = fscanf(fd, "cpu %Ld %Ld %Ld %Ld", &iUser, &iNice, &iSystem, &iIdle);
    fclose(fd);

    if (n != 4) {
        std::cerr << "fscanf returns is error" << "\n";
        return false;
    }

    if (m_bProcStart) {
        if (iUser >= m_iUser && iNice >= m_iNice && iSystem >= m_iSystem && iIdle >= m_iIdle) {
            unsigned long long iUsage, iTotal;

            iUsage = (iUser - m_iUser) + (iNice - m_iNice) + (iSystem - m_iSystem);
            iTotal = iUsage + (iIdle - m_iIdle);

            if (iTotal > 0) {
                dbValue = iUsage * 100.0 / iTotal;
            }
        }
    }
    else {
        m_bProcStart = true;
    }

    m_iUser = iUser;
    m_iNice = iNice;
    m_iSystem = iSystem;
    m_iIdle = iIdle;



    // read process usages
    fd = fopen(pidfile, "r");
    if (fd == NULL) {
        std::cerr << pidfile << " open error!" << "\n";
        return false;
    }

    int           pid;                      /** The process id. **/
    char          exName[128]; /** The filename of the executable **/
    char          state; /** 1 **/          /** R is running, S is sleeping,
                 D is sleeping in an uninterruptible wait,
                 Z is zombie, T is traced or stopped **/
    unsigned      euid,                      /** effective user id **/
        egid;                      /** effective group id */
    int           ppid;                     /** The pid of the parent. **/
    int           pgrp;                     /** The pgrp of the process. **/
    int           session;                  /** The session id of the process. **/
    int           tty;                      /** The tty the process uses **/
    int           tpgid;                    /** (too long) **/
    unsigned int  flags;                    /** The flags of the process. **/
    unsigned int  minflt;                   /** The number of minor faults **/
    unsigned int  cminflt;                  /** The number of minor faults with childs **/
    unsigned int  majflt;                   /** The number of major faults **/
    unsigned int  cmajflt;                  /** The number of major faults with childs **/
    int           utime;                    /** user mode jiffies **/
    int           stime;                    /** kernel mode jiffies **/

#if 1
    n = fscanf(fd, "%d %s %c %d %d %d %d %d %d %d %x %x %x %x %x %d %d", &pid, exName, &state, &euid, &egid, &ppid, &pgrp, &session, &tty, &tpgid, &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime);
#else
    n = fscanf(fd, "%*d %*s %*c %*d" //pid,command,state,ppid
        "%*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu"
        "%lu %lu" //usertime,systemtime
        "%*ld %*ld %*ld %*ld %*ld %*ld %*llu"
        "%*lu", &pid, exName, &state, &euid, &egid, &ppid, &pgrp, &session, &tty, &tpgid, &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime
    );
#endif

    fclose(fd);

    if (n != 17) {
        std::cerr << pidfile << "fscanf returns is error! " << n << "\n";
        std::cerr << pid << " " << state << "\n";
        return false;
    }

    return true;
}
#endif

bool bSynthFinished = false;
void cpu_monitor()
{
    printf("cpu_monitor\n");
    
    int logintv = 10 * 60 * 1000;   //10min
    int elapsed = 0;
    while (bSynthFinished == false) {
        std::this_thread::sleep_for(std::chrono::milliseconds(CPU_MONITOR_INTERVAL));
        double cpuRate = 0.0;
        int memRes = 0;
        if (cpu_getcmd(cpuRate, memRes)) {
            //            printf("%.2f\n", dbValue);
            mVecCpurate.push_back(cpuRate);
            mVecMem.push_back(memRes);
            elapsed += CPU_MONITOR_INTERVAL;
            if (elapsed > logintv) {
                sprintf(msgLog, "memRes = %d\r\n", memRes);
                makeLogFile(1, msgLog);
                elapsed = 0;
            }
        }
    }
}
#endif

#ifdef TARGET_ENGINE
LIB_SPEC_ST stSpec;
bool loadLib(void)
{
    char funcname[1024];

#if defined(WIN32) /* Windows only! */
    stSpec.handle = LoadLibrary(LIBPATH);
#else
    stSpec.handle = dlopen(LIBPATH, RTLD_LAZY);
#endif
    if (!stSpec.handle) {
        printf("Load library open error in (%s)\n", LIBPATH);
        //stSpec.report.errorcode = checkArch(stSpec.engninfo.libpath);
        return false;
    }
    //stSpec.report.errorcode = checkArch(stSpec.engninfo.libpath);

    //std::string strLang = ENGINE_LANGUAGE;
    sprintf(funcname, "VT_LOADTTS_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_LOADTTS = (short(*)(int hWnd, int nSpeakerID, char* db_path, char* licensefile))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_LOADTTS == NULL) {
        printf("VT_LOADTTS load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }

    sprintf(funcname, "VT_UNLOADTTS_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_UNLOADTTS = (void(*)(int nSpeakerID))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_UNLOADTTS == NULL) {
        printf("VT_UNLOADTTS load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }

    sprintf(funcname, "VT_LOAD_UserDict_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_LOAD_UserDict = (short(*)(int dictidx, char* filename))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_LOAD_UserDict == NULL) {
        printf("VT_LOAD_UserDict load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }

    sprintf(funcname, "VT_UNLOAD_UserDict_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_UNLOAD_UserDict = (short(*)(int dictidx))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_UNLOAD_UserDict == NULL) {
        printf("VT_UNLOAD_UserDict load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }

    sprintf(funcname, "VT_TextToFile_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_TextToFile = (short(*)(int fmt, char* tts_text, char* filename, int nSpeakerID, int pitch, int speed, int volume, int pause, int dictidx, int texttype))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_TextToFile == NULL) {
        printf("VT_TextToFile load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }

    sprintf(funcname, "VT_TextToBuffer_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_TextToBuffer = (int(*)(int fmt, char* tts_text, char* output_buff, int* output_len, int flag, int nThreadID, int nSpeakerID, int pitch, int speed, int volume, int pause, int dictidx, int texttype))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_TextToBuffer == NULL) {
        printf("VT_TextToBuffer load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }

    sprintf(funcname, "VT_SetPitchSpeedVolumePause_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_SetPitchSpeedVolumePause = (void(*)(int pitch, int speed, int volume, int pause, int nSpeakerID))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_SetPitchSpeedVolumePause == NULL) {
        printf("VT_SetPitchSpeedVolumePause load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }

    sprintf(funcname, "VT_SetCommaPause_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_SetCommaPause = (void(*)(int pause, int nSpeakerID))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_SetCommaPause == NULL) {
        printf("VT_SetCommaPause load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }

    sprintf(funcname, "VT_SetParenthesisCharNumber_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_SetParenthesisCharNumber = (void(*)(int nByte))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_SetParenthesisCharNumber == NULL) {
        printf("VT_SetParenthesisCharNumber load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }

    sprintf(funcname, "VT_GetTTSInfo_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_GetTTSInfo = (int(*)(int request, char* licensefile, void* value, int valuesize))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_GetTTSInfo == NULL) {
        printf("VT_GetTTSInfo load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }


#ifdef ENGINE_INTERNAL
    sprintf(funcname, "VT_TextToBufferEX_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_TextToBufferEX = (int(*)(int fmt, char* tts_text, char* output_buff, int* output_len, int flag, int nThreadID, int nSpeakerID, SYNCINFO_NEW * *ppSyncInfoNew, TTSMarkArray * *ppMarkArray, int pitch, int speed, int volume, int pause, int dictidx, int texttype))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_TextToBufferEX == NULL) {
        printf("VT_TextToBufferEX load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        return false;
    }

#if 0
    sprintf(funcname, "VT_GetLicenseInfo_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_GetLicenseInfo = (int(*)(int request, char* licensefile, void* pLicensePtr, int nLicenseSize, void* value, int valuesize))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_GetLicenseInfo == NULL) {
        printf("VT_GetLicenseInfo load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_GETLICENSEINFO;
        return false;
    }

    sprintf(funcname, "VT_AllocSyncInfo_New_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_AllocSyncInfo_New = (PSYNCINFO_NEW(*)(void))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_AllocSyncInfo_New == NULL) {
        printf("VT_AllocSyncInfo_New load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_ALLOCSYNCINFO_NEW;
        return false;
    }

    sprintf(funcname, "VT_FreeSyncInfo_New_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_FreeSyncInfo_New = (void(*)(PSYNCINFO_NEW pSyncInfo))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_FreeSyncInfo_New == NULL) {
        printf("VT_FreeSyncInfo_New load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_FREESYNCINFO_NEW;
        return false;
    }

    sprintf(funcname, "VT_InitSyncInfo_New_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_InitSyncInfo_New = (void(*)(PSYNCINFO_NEW pSyncInfo))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_InitSyncInfo_New == NULL) {
        printf("VT_InitSyncInfo_New load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_INITSYNCINFO_NEW;
        return false;
    }

    sprintf(funcname, "VT_TextToPhoneme_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_TextToPhoneme = (int(*)(char* tts_text, char* phoneme, int* phoneme_len, int flag, int pitch, int speed, int volume, int pause, int dictidx, int texttype))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_TextToPhoneme == NULL) {
        printf("VT_TextToPhoneme load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_TEXTTOPHONEME;
        return false;
    }

    sprintf(funcname, "VT_TextToPhonemeFile_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_TextToPhonemeFile = (short(*)(char* tts_text, char* filename, int pitch, int speed, int volume, int pause, int dictidx, int texttype))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_TextToPhonemeFile == NULL) {	// 일부 엔진에서는 미지원하는것 같음.
        printf("VT_TextToPhonemeFile load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_TEXTTOPHONEMEFILE;
        //return false;
    }

    sprintf(funcname, "VT_TextToTranscription_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_TextToTranscription = (int(*)(char* tts_text, char* phoneme, int* phoneme_len, int flag, int pitch, int speed, int volume, int pause, int dictidx, int texttype))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_TextToTranscription == NULL) {
        printf("VT_TextToTranscription load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_TEXTTOTRANSCRIPTION;
        return false;
    }

    sprintf(funcname, "VT_TextToPron_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_TextToPron = (short(*)(char* tts_text, char* outPron, int* pszSize_len, int flag, int mode, int dictidx, int texttype))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_TextToPron == NULL) {
        printf("VT_TextToPron load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_TEXTTOPRON;
        //return false;
    }

    sprintf(funcname, "VT_SetFirstLastPause_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_SetFirstLastPause = (void(*)(int first_pause, int last_pause))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_SetFirstLastPause == NULL) {	// 일부 엔진에서는 미지원하는것 같음.
        printf("VT_SetFirstLastPause load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_SETFIRSTLASTPAUSE;
        //return false;
    }

    sprintf(funcname, "VT_GetFirstLastPause_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_GetFirstLastPause = (int(*)(int* first_pause, int* last_pause))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_GetFirstLastPause == NULL) {	// 일부 엔진에서는 미지원하는것 같음.
        printf("VT_GetFirstLastPause load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_SETFIRSTLASTPAUSE;
        //return false;
    }

    sprintf(funcname, "VT_LOAD_UserConfig_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_LOAD_UserConfig = (short(*)(int dictidx, char* filename))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_LOAD_UserConfig == NULL) {	// 일부 엔진에서는 미지원하는것 같음.
        printf("VT_LOAD_UserConfig load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_TEXTTOPHONEMEFILE;
        //return false;
    }

    sprintf(funcname, "VT_UNLOAD_UserConfig_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_UNLOAD_UserConfig = (short(*)(int dictidx))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_UNLOAD_UserConfig == NULL) {	// 일부 엔진에서는 미지원하는것 같음.
        printf("VT_UNLOAD_UserConfig load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_SETFIRSTLASTPAUSE;
        //return false;
    }

    sprintf(funcname, "VT_TextToBufferEX_UserConfig_%s", ENGINE_LANGUAGE);
    stSpec.func.VT_TextToBufferEX_UserConfig = (int(*)(int fmt, char* tts_text, char* output_buff, int* output_len, int flag, int nThreadID, int nSpeakerID, SYNCINFO_NEW * *ppSyncInfoNew, TTSMarkArray * *ppMarkArray, int pitch, int speed, int volume, int pause, int dictidx, int texttype, int configidx))LOADSYM((H_MOD)stSpec.handle, funcname);
    if (stSpec.func.VT_TextToBufferEX_UserConfig == NULL) {	// 일부 엔진에서는 미지원하는것 같음.
        printf("VT_TextToBufferEX_UserConfig load symbol error in (%s)\n", stSpec.engninfo.libpath.c_str());
        stSpec.report.errorcode = Q_LIB_ERR_LOAD_SETFIRSTLASTPAUSE;
        //return false;
    }
#endif
#endif
    return true;
}


void unloadLib(void* handle)
{
    if (handle)
    {
#if defined(WIN32) /* Windows only! */
        FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
        handle = nullptr;
    }
}

#endif

void deinit_Ext()
{
#ifdef TARGET_ENGINE
    (*stSpec.func.VT_UNLOADTTS)(ENGINE_SPEKAERID%100);

    unloadLib(stSpec.handle);
#endif
}

bool init_Ext(int argc, char** argv)
{
    int ret = -1;
#ifdef TARGET_ENGINE
    loadLib();

    char cwd[1024] = { 0, };
    char LICENSE[1024] = { 0, };
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("[Error] Can't get the current path!\n");
        //        makeLogFile(2, "[Error] Can't get the current path!\r\n");
        return false;
    }
    printf("cwd:%s\n", cwd);
    sprintf(LICENSE, "%s%sverification.txt", cwd, SEP);
    // check userdictionary
    if ((ret = (stSpec.func.VT_LOADTTS)((int)NULL, ENGINE_SPEKAERID%100, DBPATH, LICENSE)) != 0) {
        printf("VT_LOADTTS ERROR in speaker(%d). error(%d)\n", ENGINE_SPEKAERID, ret);
        return true;
    }

#endif
#ifdef TARGET_VTAPI
    VTAPI_ENGINE_HANDLE engine = NULL;
    char cwd[1024] = { 0, };


    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("[Error] Can't get the current path!\n");
        //        makeLogFile(2, "[Error] Can't get the current path!\r\n");
        return false;
    }
    printf("cwd:%s\n", cwd);
#ifdef CALLBACK_LOG
    VTAPI_SetCallback4Log(NULL, (void*)log_callback);
    VTAPI_SetCallback4LogFilter(NULL, 0);
#endif

    ret = VTAPI_Init(cwd);
    VTAPI_SetLicenseFolder(cwd);



    if (argc < 3) {
        printf("[Error] Invalid param counts(%d)!\n", argc);
        //        sprintf(msgLog, "[Error] Invalid param counts(%d)!\r\n", argc);
        //        makeLogFile(1, msgLog);
        return false;
    }
    m_speaker = argv[1]; m_type = argv[2]; m_threadnum = atoi(argv[3]);
    if (argc > 3)   m_text = argv[3];

    engine = VTAPI_GetEngine((char*)m_speaker.c_str(), (char*)m_type.c_str());
    if (engine <= 0) {
        printf("[Error] GetEngine : 0x % x, % s\n", engine, VTAPI_GetLastErrorInfo(0)->szMsg);
        //        sprintf(msgLog, "[Error] GetEngine : 0x%x, %s\r\n", engine, VTAPI_GetLastErrorInfo(0)->szMsg);
        //        makeLogFile(1, msgLog);
        return false;
    }
#endif

    return true;
}

int main(int argc, char ** argv)
{
    char cwd[1024] = { 0, };
    int ret = -1;

#if defined(SUPPORT_SIGNAL)
#if defined(WIN32)
    signal(SIGTERM, sighandler);
    signal(SIGSEGV, sighandler);
    signal(SIGABRT, sighandler);
#else
#if 1
    struct sigaction sa;
    sa.sa_handler = sighandler;
    sigaction(SIGKILL, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
#endif
#endif
#endif

    // set time info
    time_t t;
    struct tm tm;
    t = time(NULL);
    tm = *localtime(&t);
    memset(mTimeInfo, 0x00, sizeof(char) * 1024);
    sprintf(mTimeInfo, "%.4d%.2d%.2d_%.2d%.2d%.2d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);


    // log start
    makeLogFile(0, NULL);
 
#if 0
    if(getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("[Error] Can't get the current path!\n");
//        makeLogFile(2, "[Error] Can't get the current path!\r\n");
        return EXIT_FAILURE;
    }
    printf("cwd:%s\n", cwd);
#endif

    if (argc < 3) {
        printf("[Error] Invalid param counts(%d)!\n", argc);
//        sprintf(msgLog, "[Error] Invalid param counts(%d)!\r\n", argc);
//        makeLogFile(1, msgLog);
        return EXIT_FAILURE;
    }
    m_speaker = argv[1]; m_type = argv[2];
    if (argc > 3) m_text = argv[3];


    //memset(mResultPath, 0x00, sizeof(char) * 1024);
    //sprintf(mResultPath, "%s%sRESULT_%.4d%.2d%.2d_%.2d%.2d%.2d_%s_%s", PCMOUTPUT_NAME, SEP, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, m_speaker.c_str(), m_type.c_str());
    memset(mResultPCMPath, 0x00, sizeof(char) * 1024);
    sprintf(mResultPCMPath, "%s%s%s_%s_%s", GET_INST(ConfigInfo)->getOutDir().c_str(), SEP, mTimeInfo, m_speaker.c_str(), m_type.c_str());

    struct stat info;
    if (stat(mResultPCMPath, &info) != 0) {
#if defined(WIN32)
        mkdir(mResultPCMPath);
#else
        mkdir(mResultPCMPath, 0755);
#endif
    }

    // prepare exception texts
    if (readFileLine(EXCEPTIONLIST_TEXT, m_VecExceptionList) == false) {
        makeLogFile(1, "Exception list is empty\r\n");
    }

    // check synthesize text
    int length = 0;
    if (m_text.empty()) {
        m_text = GET_INST(ConfigInfo)->getScript();
    }

    mTextin.open(m_text);
    if (mTextin.is_open()) {
        mTextin.seekg(0, mTextin.end);
        length = mTextin.tellg();
        mTextin.seekg(0, mTextin.beg);
    } else {
        printf("[Error] Cannot open text file:%s\n", m_text.c_str());
        return EXIT_FAILURE;
    }

#if 0   // check inputtext
    if (mTextin.is_open()) {
        int cnt = 0;
        while (!mTextin.eof()) {
            std::string strtext;
            getline(mTextin, strtext);
            cnt++;
        }

        printf("cnt = %d\n", cnt);
        return EXIT_SUCCESS;
    }
#endif

    //prepare engine

    // init TTS
    if (init_Ext(argc, argv) == false) {
        return EXIT_FAILURE;
    }

    if (length > 0) {
#ifdef USE_STARTLINE_FOR_TEST   //skip lines
        std::string tempstr;
        while (!mTextin.eof()) {
            getline(mTextin, tempstr);
            ++m_curTextIdx;
            if (m_curTextIdx > STARTLINE_NUM)
            {
                break;
            }
#if 0
            if (tempstr.size() > MAX_LOGSIZE - 50)
                sprintf(msgLog, "[%d]\r\n", m_curTextIdx);
            else
                sprintf(msgLog, "[%d] %s\r\n", m_curTextIdx, tempstr.c_str());
            makeLogFile(1, msgLog);
#endif
        }
#endif
#if 0
        // prepare input texts
        if (readFileLine(m_text.c_str(), m_VecInputText) == false) {
            sprintf(msgLog, "[Error] Cannot found input file(%s)\r\n", m_text.c_str());
            makeLogFile(1, msgLog);
            return EXIT_FAILURE;
        }

        // start threads
        int maxthreadcnt = m_VecInputText.size() / 100;
        if (maxthreadcnt > 30) maxthreadcnt = 30;
        else if (maxthreadcnt < 1) maxthreadcnt = 1;
#else
        getTime(mVecResult.tStart);

        // start threads
        int maxthreadcnt = GET_INST(ConfigInfo)->getThreadNum();
        //int maxthreadcnt = 1;
        if (maxthreadcnt > MAX_THREAD_NUM) maxthreadcnt = MAX_THREAD_NUM;
        else if (maxthreadcnt < 1) maxthreadcnt = 1;
#endif
        std::vector<std::thread> threads(maxthreadcnt);
        printf("Synthesis %s, %s, %s, threads : %d\n", m_speaker.c_str(), m_type.c_str(), m_text.c_str(), maxthreadcnt);
        for (int thread_count = 0; thread_count < maxthreadcnt; thread_count++) {
            threads[thread_count] = std::thread(doSynthesize, thread_count);
        }

#ifdef WAIT_FOR_THREAD_CREATION
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_FOR_THREAD_CREATION_SECOND));
        // store the number of threads created
        {   // do not remove this brace
            std::unique_lock<std::mutex> lk(cv_m);
            mVecResult.nInitialThreadCnt = mVecResult.nThreadCnt;
        }
        printf("notify_all\n");
        cv.notify_all();
#endif


#if !defined(WIN32) && defined(CPU_MONITOR)
#ifdef CPU_MONITOR_USE_TOP
        sprintf(pidfile, "top n1 | grep %lu", getpid());
#else
        sprintf(pidfile, "ps -p %lu -o %%cpu", getpid());
#endif
        std::thread tCPU = std::thread(cpu_monitor);
#endif

        // Wait for all threads to finish
        for (auto& th : threads)
            th.join();

#if !defined(WIN32) && defined(CPU_MONITOR)
        bSynthFinished = true;
        tCPU.join();
#endif

        getTime(mVecResult.tFinish);
        printResult(maxthreadcnt);
    }

    // deinit TTS
    deinit_Ext();
    
    makeLogFile(2, "\nFinished!!\r\n");

    return EXIT_SUCCESS;
}

#define MAX_LOG_SIZE 200 * 1024 * 1024    //200MB
void makeLogFile(int mode, char* msg)
{
#if 0
    if(msg != NULL && strlen(msg) > 0) {
        printf(msg);
    }
#endif

#ifdef MAKE_LOG_FILE
    static char logname[1024+64];
    static int sizecheckcnt = 0;

    result_mutex.lock();
    if (sizecheckcnt % 20 == 0) {
        std::ifstream in(logname, std::ifstream::ate | std::ifstream::binary);
        size_t size = in.tellg();
        if ((mode != 2) && (size > MAX_LOG_SIZE)) {
            if (fpLog != NULL) {
                fclose(fpLog);
                fpLog = NULL;
                mode = 0;
            }
        }
    }

    if(fpLog == NULL && mode == 0) {
        static int idxlog = 0;
        sprintf(logname, "ENGINE_%s_%d_log.txt", mTimeInfo, idxlog++);

        fpLog = fopen(logname, "wb+");
        if(fpLog == NULL) {
            printf("[Error] Log File Open\r\n");
            return;
        }
    }

    if(fpLog) {
        if(msg != NULL && strlen(msg) > 0) {
            fwrite(msg, strlen(msg), 1, fpLog);
        }

        if(mode == 2) {
            if(fpLog != NULL) {
                fclose(fpLog);
                fpLog = NULL;
            }
        }
    }
    result_mutex.unlock();
#endif
}

bool isExceptionalText(std::string text)
{
    bool ret = false;

    for(std::string exceptiontext : m_VecExceptionList) {
        if (text.find(exceptiontext) != std::string::npos) {
            ret = true;
            break;
        }
    }

    return ret;
}
bool doSynthesize(int threadNum)
{
    int ret = -1;
    int mThreadID = 0;


#ifdef WAIT_FOR_THREAD_CREATION
#if 0
    result_mutex.lock();
    mThreadID = mThreadCnt++;
    printf("Waiting... thread : %d\n", threadNum);
    result_mutex.unlock();

    while (mThreadCnt < MAX_THREAD_NUM) {
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_FOR_THREAD_CREATION_SECOND));
    }
    printf("Restarting... thread : %d\n", threadNum);
    
#else
    {   // do not remove this brace
        std::unique_lock<std::mutex> lk(cv_m);
        mThreadID = mVecResult.nThreadCnt++;
        mThreadCnt++;
        printf("Waiting... thread : %d\n",threadNum);
        cv.wait(lk);
    }
    printf("Restarting... thread : %d\n", threadNum);
#endif
#else
    result_mutex.lock();
    mThreadCnt++;
    result_mutex.unlock();
#endif

    if (mTextin.is_open()) {
        while (!mTextin.eof()) {
            std::string strFileName(mResultPCMPath);
            std::string strSynthText;
            int curIdx;
#if 0
            result_mutex.lock();
            if (m_curTextIdx < m_VecInputText.size()) {
                strSynthText = m_VecInputText.at(m_curTextIdx);
                curIdx = m_curTextIdx++;
                result_mutex.unlock();
            } else {
                result_mutex.unlock();
                break;
            }
#else
            result_mutex.lock();
            if (mTextin.eof()) {
                result_mutex.unlock();
                break;
            }
            getline(mTextin, strSynthText);
            curIdx = m_curTextIdx++;
            if (isExceptionalText(strSynthText) == true) {
                result_mutex.unlock();
                continue;
            }
            result_mutex.unlock();

            //skip empty text
            strSynthText = trim(strSynthText);
            if (strSynthText.empty()) continue;

            replaceText(strSynthText, "\r", "");
            if(strSynthText.size() > MAX_LOGSIZE - 50)
                sprintf(msgLog, "[%d,%d]\r\n", threadNum, curIdx);
            else
                sprintf(msgLog, "[%d,%d] %s\r\n", threadNum, curIdx, strSynthText.c_str());
            makeLogFile(1, msgLog);
#endif

            if ((curIdx % 500) == 0) printf(".");
#ifdef TARGET_ENGINE
#if defined(USE_BUFTOFILE)
            FILE* fp = NULL;
#endif
            int nFramesize, nLastValidFrame = 0;
            int total = 0;
            char* framebuf;
            int frameflag, rc;
            //int nThreadID = 0;
            int texttype = -1; //default

#if defined(USE_BUFTOFILE)
            strFileName.append(SEP).append(m_speaker).append("_").append(m_type).append("_").append(std::to_string(threadNum)).append("_").append(std::to_string(curIdx)).append(".pcm");
            //char PCMFile[1024];
            //sprintf(PCMFile, "%s%s%d in thread %d Thread.pcm", strFileName.c_str(), SEP, curIdx, threadNum);
            fp = fopen(strFileName.c_str(), "wb+");
#endif

            /****************************************/
            /* TTS Buffering API                    */
            /* call VT_TextToBufferEX_XXX ()        */
            /****************************************/
            rc = frameflag = -1;
            if ((*stSpec.func.VT_TextToBufferEX)(S16PCM, (char*)NULL, (char*)NULL, &nFramesize, frameflag, threadNum, ENGINE_SPEKAERID % 100, NULL, NULL, -1, -1, -1, -1, -1, texttype) < 0) {
                printf("VT_TextToBufferEX_init ERROR [%d]curIdx=%d\n", threadNum, curIdx);
                //(*stSpec.func.VT_UNLOADTTS)(stSpec.engninfo.speakerid);
                return false;
            }

            framebuf = (char*)malloc(nFramesize);
            frameflag++;

            do {
                if ((rc = (stSpec.func.VT_TextToBufferEX)(S16PCM, (char*)strSynthText.c_str(), framebuf, &nFramesize, frameflag, threadNum, ENGINE_SPEKAERID % 100, NULL, NULL, -1, -1, -1, -1, -1, texttype)) < 0) {
//                if ((rc = (stSpec.func.VT_TextToBufferEX)(S16PCM, (char*)strSynthText.c_str(), framebuf, &nFramesize, frameflag, threadNum, ENGINE_SPEKAERID % 100, &pSyncInfoNew, &ppMarkArray, -1, -1, -1, -1, -1, texttype)) < 0) {
                    //printf("VT_TextToBufferEX ERROR [%d]curIdx=%d\n", threadNum, curIdx);
                    sprintf(msgLog, "VT_TextToBufferEX ERROR[%d]curIdx = %d\n", threadNum, curIdx);
                    printf("%s", msgLog);
                    makeLogFile(1, msgLog);
                    break;
                }

                //            printf("===========size:%d(samples:%d), total:%d(samples:%d)==========\n", nFramesize, nFramesize/2, total, total/2);
                //            printf("===========size:%d, m_nNumberOfValidWordSync:%d, m_nMaxWordForSync:%d==========\n", nFramesize, pSyncInfoNew->m_nNumberOfValidWordSync, pSyncInfoNew->m_nMaxWordForSync);
                total += nFramesize;

                if (frameflag == 0) frameflag++;
#if defined(USE_BUFTOFILE)
                if (fp != NULL) { fwrite(framebuf, nFramesize, 1, fp); }
#endif
                if (nFramesize > 0) nLastValidFrame = nFramesize;
            } while (rc == 0);

#if defined(USE_BUFTOFILE)
            if (fp != NULL) { fclose(fp); }
#endif
            free(framebuf);
#endif
        }
    }

    //printf("End... thread : %d\n", threadNum);
    return true;
}

bool printResult(int nThreadnum)
{
    FILE* fpResult = NULL;
    FILE* fpResultThread = NULL;

    std::string strPrefix;
    char report[4096];

    std::string strReport(mResultPCMPath);
    std::string strReportThread(mResultPCMPath);
    strReport.append(".txt");
    strReportThread.append("_ThreadData.txt");

    fpResult = fopen(strReport.c_str(), "wt+");
    if (fpResult == NULL) {
        sprintf(msgLog, "[Error] Result File Open\r\n");
        makeLogFile(1, msgLog);
        return false;
    }

#if 0
    fpResultThread = fopen(strReportThread.c_str(), "wt+");
    if (fpResultThread == NULL) {
        sprintf(msgLog, "[Error] ResultThread File Open\r\n");
        makeLogFile(1, msgLog);
        return false;
    }
#endif

    // engine info
    int nRes = 0;
    char szSpeaker[64] = { 0, }, szType[64] = { 0, };
    char szDllPath[1024] = { 0, }, szDBPath[1024] = { 0, }, szLang[64] = { 0, }, szGender[64] = { 0, }, szVersion[64] = { 0, }, szISOCode[64] = { 0, };
    int nSampling, nChannel;
//    nRes = VTAPI_GetEngineInfoFieldEx(hEngine, szSpeaker, szType, szDllPath, szDBPath, szLang, szGender, szVersion, &nSampling, &nChannel, szISOCode);
    sprintf(report, "Engine Info\t%s, %s, %s, %s, %s\n", szSpeaker, szType, szLang, szGender, szVersion);
    fwrite(report, strlen(report), 1, fpResult);

    // total elapsed time
    sprintf(report, "Total Elpased Time(sec)\t%.2f\n", (double)(mVecResult.tFinish - mVecResult.tStart) / CLOCKS_PER_SEC);
    fwrite(report, strlen(report), 1, fpResult);
    makeLogFile(1, report);

    // The number of threads at the start of synthesis
#ifdef WAIT_FOR_THREAD_CREATION
    sprintf(report, "Threads Num\t(%d/%d)\n", mVecResult.nInitialThreadCnt, mVecResult.nThreadCnt);
#else
    sprintf(report, "Threads Num\t(%d)\n", mThreadCnt);
#endif
    fwrite(report, strlen(report), 1, fpResult);
    makeLogFile(1, report);

#if 0
    // Loop number for thread
    sprintf(report, "Loop Number\t%d\n\n", MAX_SYNTH_NUM);
    fwrite(report, strlen(report), 1, fpResult);
    makeLogFile(1, report);
#endif

    //prepare summary result
    double sumfirstbuf = 0, sumsynth = 0;
    double sumrto = 0;
    double minsynth = 0, maxsynth = 0;
    uint32_t nRefBufSize;
    size_t nSynthNum = 0;

#if 0
    std::vector<ST_SYNTHTIME_INFO>::iterator iter = mVecResult.vecSynthTime.begin();
    if (iter != mVecResult.vecSynthTime.end()) {
        maxsynth = minsynth = iter->tEndSynth - iter->tBase;
        nRefBufSize = iter->nSizeofBuf;

        // report thread results
        char reportThread[4096];
        sprintf(reportThread, "THREAD\tFIRST Buf(ms)\tSYNTH Duration(ms)\tPLAYBACK Time(ms)\tSize(byte)\tREAL-TIME Ratio\n");
        fwrite(reportThread, strlen(reportThread), 1, fpResultThread);
        while (iter != mVecResult.vecSynthTime.end()) {
            double cur_firstbuf = 0, cur_synth = 0, cur_playbacktime = 0, cur_rto = 0;
            cur_firstbuf = iter->tFirstBuf - iter->tBase;
            cur_synth = iter->tEndSynth - iter->tBase;
            cur_playbacktime = iter->nSizeofBuf / (((double)nSampling * BYTE_PER_SAMPLE / MILLI_IN_SEC) * CHANNEL_SIZE);
            cur_rto = (double)cur_playbacktime / (cur_synth / MILLI_SEC);

            // thread results
            sprintf(reportThread, "%d\t%.2f\t%.2f\t%.2f\t%d\t%.2f\n", iter->nThreadID, cur_firstbuf / MILLI_SEC, cur_synth / MILLI_SEC, cur_playbacktime, iter->nSizeofBuf, cur_rto);
            fwrite(reportThread, strlen(reportThread), 1, fpResultThread);

            if (cur_synth > maxsynth) {
                maxsynth = cur_synth;
            }
            if (cur_synth < minsynth) {
                minsynth = cur_synth;
            }

            sumfirstbuf += cur_firstbuf;
            sumsynth += cur_synth;
            sumrto += cur_rto;

            if (nRefBufSize != iter->nSizeofBuf) {
                sprintf(msgLog, "[Error] Different synth file is found!\r\n");
                makeLogFile(1, msgLog);
                break;
            }

            nSynthNum++;
            iter++;
        }

        // summary synthesis
        sprintf(report, "MIN SYNTH(ms)\tMAX SYNTH(ms)\tAVG. First Buf(ms)\tAVG. SYNTH(ms)\tAVG. Realtime ratio\n");
        fwrite(report, strlen(report), 1, fpResult);

        sprintf(report, "%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n", minsynth / MILLI_SEC, maxsynth / MILLI_SEC, sumfirstbuf / MILLI_SEC / nSynthNum, sumsynth / MILLI_SEC / nSynthNum, sumrto / nSynthNum);
        fwrite(report, strlen(report), 1, fpResult);
    }
#endif

    // summary cpu ratio
#if !defined(WIN32) && defined(CPU_MONITOR)
    double maxcpu = 0.0, mincpu = 0.0, avgcpu = 0.0;
    size_t ncpurate = 0;
    std::vector<double>::iterator itercpu = mVecCpurate.begin();
    if (itercpu != mVecCpurate.end()) {
        maxcpu = mincpu = *itercpu;

        printf("mVecCpurate=%d", mVecCpurate.size());
        while (itercpu != mVecCpurate.end()) {
            if (*itercpu > maxcpu) { maxcpu = *itercpu; }
            if (*itercpu < mincpu) { mincpu = *itercpu; }
            avgcpu += *itercpu;
            ncpurate++; itercpu++;
        }
        avgcpu /= ncpurate;
    }

    // write cpu
    sprintf(report, "\nMIN CPU\tMAX CPU\tAVG. CPU\n");
    fwrite(report, strlen(report), 1, fpResult);

    sprintf(report, "%.2f\t%.2f\t%.2f\n", mincpu, maxcpu, avgcpu);
    fwrite(report, strlen(report), 1, fpResult);

    // write res
    sprintf(report, "\nRES[interval:%d(sec)]\n", CPU_MONITOR_INTERVAL/1000);
    fwrite(report, strlen(report), 1, fpResult);

    size_t nMemSize = mVecMem.size();
    size_t nStepCnt;
    if(nMemSize >= 20) nStepCnt = nMemSize / 10;
    else nStepCnt = 1;
    printf("nStepCnt=%d, %d\n", nStepCnt, nMemSize);
    sprintf(report, "nStepCnt=%d, %d\n", nStepCnt, nMemSize);
    fwrite(report, strlen(report), 1, fpResult);
    for (size_t nIdx = 0; nIdx < nMemSize; ) {
        sprintf(report, "[%06d] %d\n", nIdx, mVecMem.at(nIdx));
        fwrite(report, strlen(report), 1, fpResult);
        nIdx += nStepCnt;
    }
#endif


    return true;
}
