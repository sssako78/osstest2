/*
* Copyright (c) 2000-2019 ReadSpeaker
* All Rights Reserved.
*/

#ifndef VTAPI_EXTEND_H
#define VTAPI_EXTEND_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "vtapi.h"


/* Handle for getting synchronized information */
typedef void* VTAPI_SYNC_HANDLE;

/* Structure of infomation of word in synchronized information */
typedef struct vtapi_wordinfo
{
    int                 nIndex;
    int                 nLength;
    int                 nStartPosInText;
    int                 nEndPosInText;
    int                 nSentStartPosInText;
    int                 nSentEndPosInText;
} VTAPI_WORDINFO;

/* Mark array structure for mark tag */
#define     MAX_MARK_NAME               (512)
typedef struct vtapi_markinfo
{
    int                 nIndex;
    int                 nPosInText;
    int                 nOffsetInStream;
    int                 nOffsetInBuffer;
    char                szMarkName[MAX_MARK_NAME];
} VTAPI_MARKINFO;

/* Get buffer with synchronized information */
VTAPI_decl int VTAPI_GetBuffer_Sync(VTAPI_HANDLE hVTAPI, char** pFrameBuffer, VTAPI_SYNC_HANDLE *pSyncHandle);

/* Get information of word in synchronized information */
VTAPI_decl int VTAPI_GetWordCount(VTAPI_SYNC_HANDLE hSync);
VTAPI_decl int VTAPI_GetWordInfo(VTAPI_SYNC_HANDLE hSync, int nWordIndex, VTAPI_WORDINFO *pWord);

/* Get information of mark */
VTAPI_decl int VTAPI_GetMarkCount(VTAPI_HANDLE hVTAPI);
VTAPI_decl int VTAPI_GetMarkInfo(VTAPI_HANDLE hVTAPI, int nMarkIndex, VTAPI_MARKINFO *pMark);

/* Type of filter for log */
#ifndef VTAPI_LOG_FN_DEFINE
#define VW_LOG_VERBOSE_INFO     0
#define VW_LOG_VERBOSE_TRACE    1
#define VW_LOG_VERBOSE_DEBUG    2
#define VW_LOG_VERBOSE_WARN     3
#define VW_LOG_VERBOSE_ERROR    4
#define VW_LOG_VERBOSE_FATAL    5
#define VW_LOG_NONE             6

typedef void (*VTAPI_LOG_FN)(const char *header, const char *msg, int level, void *user_data);
#define VTAPI_LOG_FN_DEFINE
#endif

/* Set callback for log */
VTAPI_decl void VTAPI_SetCallback4Log(VTAPI_HANDLE hVTAPI, void *LogProc);

/* Set callback for engine log */
VTAPI_decl void VTAPI_SetCallback4Engine(void* LogProc);

/* Set filter for log */
VTAPI_decl void VTAPI_SetCallback4LogFilter(VTAPI_HANDLE hVTAPI, int nFilter);


/* Type of requesting informations of engine */
typedef enum ENGINE_GETTTSINFO
{
    ENGINE_BUILD_DATE          =  0,
    ENGINE_VERIFY_CODE         =  1,
    ENGINE_MAX_CHANNEL         =  2,
    ENGINE_DB_DIRECTORY        =  3,
    ENGINE_LOAD_SUCCESS_CODE   =  4,
    ENGINE_MAX_SPEAKER         =  5,
    ENGINE_DEF_SPEAKER         =  6,
    ENGINE_CODEPAGE            =  7,
    ENGINE_DB_ACCESS_MODE      =  8,
    ENGINE_FIXED_POINT_SUPPORT =  9,
    ENGINE_SAMPLING_FREQUENCY  = 10,
    ENGINE_MAX_PITCH_RATE      = 11,
    ENGINE_DEF_PITCH_RATE      = 12,
    ENGINE_MIN_PITCH_RATE      = 13,
    ENGINE_MAX_SPEED_RATE      = 14,
    ENGINE_DEF_SPEED_RATE      = 15,
    ENGINE_MIN_SPEED_RATE      = 16,
    ENGINE_MAX_VOLUME          = 17,
    ENGINE_DEF_VOLUME          = 18,
    ENGINE_MIN_VOLUME          = 19,
    ENGINE_MAX_SENT_PAUSE      = 20,
    ENGINE_DEF_SENT_PAUSE      = 21,
    ENGINE_MIN_SENT_PAUSE      = 22,
    ENGINE_DB_BUILD_DATE       = 23,
    ENGINE_MAX_COMMA_PAUSE     = 24,
    ENGINE_DEF_COMMA_PAUSE     = 25,
    ENGINE_MIN_COMMA_PAUSE     = 26,

    ENGINE_VERSION             = 99
} EngineInfo_Request;

/* Get information of engine with specified license file*/
VTAPI_decl int VTAPI_GetTTSInfo(VTAPI_ENGINE_HANDLE hEngine, int request, char *licensefile, void *value, int valuesize);


/* Check specified license per engine */
VTAPI_decl int VTAPI_CheckLicense(VTAPI_ENGINE_HANDLE hEngine, char *licensefile, void *pLicensePtr, int nLicenseSize);

/* Set(Specify) license per engine */
VTAPI_decl int VTAPI_SetLicense(VTAPI_ENGINE_HANDLE hEngine, int nFlag, char *pszLicenseinfo);

/* Get filename of applied license per engine */
VTAPI_decl int VTAPI_GetEngineLicensePath(VTAPI_ENGINE_HANDLE hEngine, char *licensefile);


/* Type of requesting information of license */
typedef enum ENGINE_GETLICENSEINFO
{
    LICENSE_CHANNEL_NO            = 0,
    LICENSE_EXPIRE_DATE           = 1,
    LICENSE_HOSTID                = 2,
    LICENSE_DETAILED_ALL          = 3,
    LICENSE_DETAILED_OS           = 4,
    LICENSE_DETAILED_LANG         = 5,
    LICENSE_DETAILED_SPEAKER      = 6,
    LICENSE_DETAILED_VERSION      = 7,
    LICENSE_DETAILED_DBACCESS     = 8,
    LICENSE_DETAILED_SAMPLING     = 9,
    LICENSE_DETAILED_APP          = 10,
    LICENSE_DETAILED_WAVSAVE      = 11,
    LICENSE_DETAILED_SAVETIME     = 12,
    LICENSE_DETAILED_BGAUDIO      = 13,
    LICENSE_DETAILED_DBSIZE       = 14,
    LICENSE_DETAILED_REALTIME     = 15,

    LICENSE_DETAILED_RSVERSION    = 16,

    LICENSE_DETAILED_SERVER_CH_NO = 20,

#if 1 // defined(USE_VT_CACHE)
	LICENSE_DETAILED_NOTE_CACHE	  = 21,
#endif
#if 1 // defined(USE_AUDIO_WATERMARK)
	LICENSE_DETAILED_NOTE_AWMARK  = 22,
#endif

} LicenseInfo_Request;

/* Get information of specified license per engine */
VTAPI_decl int VTAPI_GetLicenseInfo(VTAPI_ENGINE_HANDLE hEngine, int request, char* licensefile, void* pLicensePtr, int nLicenseSize, void* value, int valuesize);


/* Support for not installed engines */
VTAPI_decl VTAPI_ENGINE_HANDLE VTAPI_AddNewEngineInfo(char *pszSpeaker, char *pszType, int nSpeakerID, char *pszDBPath, char *pszLang, char *pszGender, int nCodePage, char *pszISOCode, char *pszVendor, int nSamplingRate);


/* Support event-based synthesis API */
VTAPI_decl int VTAPI_SynthesizeAsync(VTAPI_HANDLE hVTAPI, void *pszText, int nTextLength, int nTextType, int nBufferSize);
VTAPI_decl int VTAPI_Synthesize(VTAPI_HANDLE hVTAPI, void *pszText, int nTextLength, int nTextType, int nBufferSize);
#if 1 // sjchang 2022-03-17 Add interruption for CPEN
VTAPI_decl int VTAPI_StopSynthesize(VTAPI_HANDLE hVTAPI);
#endif


#if defined(FOR_VTAPI_VER_44)
typedef enum ENGINEATTRFLAGS
{
	ENGINE_ATTR_PITCH = 0,
	ENGINE_ATTR_SPEED = 1,
	ENGINE_ATTR_VOLUME = 2,
	ENGINE_ATTR_PAUSE = 3,
	ENGINE_ATTR_DICTIDX = 4,
	ENGINE_ATTR_COMMAPAUSE = 5,
	ENGINE_ATTR_PARENTHESISNUM = 6,
#if 0 // sjchang 2021-04-01 Remove
	ENGINE_ATTR_EMPHASISFACTOR = 7,
#endif

#if 0 //defined(USE_OUTPUT_MARGIN_EACH_ENGINE)
	ENGINE_ATTR_FRONT_MARGIN = 9,
	ENGINE_ATTR_REAR_MARGIN = 10,
#endif

#if 1 // defined(USE_USERCONFIG) // sjchang 2022-08-05 Add for UserConfig
	ENGINE_ATTR_CONFIGIDX = 12,
#endif

	ENGINE_ATTR_MAX
} Engine_Attr_Flags;

/* Set/Get attribute of engine */
VTAPI_decl int VTAPI_SetEngineAttr(VTAPI_ENGINE_HANDLE hEngine, int nFlag, int nValue);
VTAPI_decl int VTAPI_GetEngineAttr(VTAPI_ENGINE_HANDLE hEngine, int nFlag, int *nValue);


/* Set(Specify) slicense */
VTAPI_decl int VTAPI_SetNewLicense(char *pszSeed, char *pszCDKey, char *pszPath);
#endif

#if defined(FOR_VTAPI_VER_44)
#if 1 //defined(USE_USERCONFIG)
VTAPI_decl int VTAPI_LoadUserConfig(VTAPI_ENGINE_HANDLE hEngine, int nConfigIdx, char *pszConfigFile);
//VTAPI_decl int VTAPI_LoadUserConfigExt(VTAPI_ENGINE_HANDLE hEngine, int nConfigIdx, char* pszConfigFile, void* pUserConfigPtr, int nUserConfigSize);
VTAPI_decl int VTAPI_UnloadUserConfig(VTAPI_ENGINE_HANDLE hEngine, int nConfigIdx);

VTAPI_decl int VTAPI_SetOutputSampling(VTAPI_HANDLE hVTAPI, int nNewSampling);
#endif
#endif

#if defined(__cplusplus)
}
#endif

#endif /* VTAPI_EXTEND_H */

