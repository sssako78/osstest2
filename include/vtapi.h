/*
 * Copyright (c) 2000-2019 ReadSpeaker
 * All Rights Reserved.
 */

#ifndef _VTAPI_H_
#define _VTAPI_H_

#if defined(__cplusplus)
extern "C" 
{
#endif

#if !defined(VTAPI_decl)
#	if defined(WIN32) || defined(_WIN32) || defined(WINCE)
#		define VTAPI_decl       __declspec( dllexport )
#		define _CRTDBG_MAP_ALLOC 
#		include <stdlib.h>
#		if defined(WINCE)
#		else
#		include <crtdbg.h>
#		endif
#	else
#		define VTAPI_decl       extern
#	endif
#endif


#if defined(WIN32) || defined(_WIN32) || defined(WINCE)
#pragma warning(disable:4996)
#include <windows.h>
#else
#if !defined(HWND_DEFINE)
#define HWND_DEFINE
typedef int HWND;
#endif
#if !defined(DWORD_DEFINE)
#define DWORD_DEFINE
typedef unsigned long       DWORD;
#endif
#if !defined(UINT_DEFINE)
#define UINT_DEFINE
typedef unsigned int        UINT;
#endif
#endif


// Sync with vt_ssml_ft.h

//#define FOR_VTAPI_VER_43
//#define FOR_VTAPI_VER_44
#define FOR_VTAPI_VER_45
//#define FOR_VTAPI_VER_46 // For E2E

#if defined(FOR_VTAPI_VER_46)
#define FOR_VTAPI_VER_44
#define FOR_VTAPI_VER_45
#define VTAPI_VERSION	"4.6.0.6"
#elif defined(FOR_VTAPI_VER_45)
#define FOR_VTAPI_VER_44
#define VTAPI_VERSION	"4.5.0.6"
#elif defined(FOR_VTAPI_VER_44)
#define FOR_VTAPI_VER_44
#define VTAPI_VERSION	"4.4.0.8"
#else
#define FOR_VTAPI_VER_43
#define VTAPI_VERSION	"4.3.0.12"
#endif

typedef struct VOICE_INFO* VTAPI_HANDLE;
typedef struct ENGINE_INFO* VTAPI_ENGINE_HANDLE;
#if 1 //defined(USE_NEW_USERDICT_MANAGER)
typedef struct USERDICT_INFO* VTAPI_USERDICT_HANDLE;
#endif
#if 1 //defined(FOR_VTAPI_VER_44)
#if 1 //defined(USE_RELEXICON_DICT)
typedef struct LEXICON_INFO* VTAPI_LEXICON_HANDLE;
#endif
#endif


typedef enum OUTPUTFORMAT
{
	FORMAT_16PCM = 0,
	FORMAT_8PCM = 1,
	FORMAT_ALAW_PCM = 2,
	FORMAT_MULAW_PCM = 3,
	FORMAT_ADPCM_PCM = 4,
	FORMAT_16PCM_WAV = 5,
	FORMAT_8PCM_WAV = 6,
	FORMAT_ALAW_WAV = 7,
	FORMAT_MULAW_WAV = 8,

	FORMAT_MAX
} Output_Format;

typedef enum TEXTTYPES
{
	TEXT_FORMAT_DEFAULT = 0, // multibyte
	TEXT_FORMAT_UNICODE = 1,
	TEXT_FORMAT_UTF8 = 2,
	TEXT_FORMAT_JEITA = 4,
	TEXT_FORMAT_JEITA_PLUS = 8,
	TEXT_FORMAT_BIG5 = 16,	

	TEXT_CONTENT_NORMAL = 0,
	TEXT_CONTENT_SSML = 128,

	TEXT_INPUTTYPE_STRING = 0, // string input
	TEXT_INPUTTYPE_FILE = 1024, // filename input
} Text_Types;

typedef enum AUDIOEVENTTYPES
{
	AUDIO_EVENT_NO_EVENTS = 0,
	AUDIO_EVENT_START_INPUT_STREAM = 1,
    AUDIO_EVENT_END_INPUT_STREAM = 2,
    AUDIO_EVENT_VOICE_CHANGE = 4,
    AUDIO_EVENT_TTS_MARK = 8,
    AUDIO_EVENT_WORD_BOUNDARY = 16,
	AUDIO_EVENT_SENTENCE_BOUNDARY = 32,
	AUDIO_EVENT_BUFFER = 64,
	AUDIO_EVENT_BUFFER_END = 128,
	AUDIO_EVENT_FAILURE = 256,
	AUDIO_EVENT_READY_PLAY = 512,
	AUDIO_EVENT_ALL = 1023
} Audio_Event_Types;

typedef enum ATTRFLAGS
{
    ATTR_PITCH = 0,          
    ATTR_SPEED = 1,            
    ATTR_VOLUME = 2, 
	ATTR_PAUSE = 3,
	ATTR_DICTIDX = 4,
#if 1 //0 //!defined(USE_NEW_ENGINE_SETTING) //maintain for compatibility
	ATTR_COMMAPAUSE = 5,
	ATTR_PARENTHESISNUM = 6,
#if 0 // sjchang 2021-04-01 Remove
	ATTR_EMPHASISFACTOR = 7,
#endif
#endif
	ATTR_MODE_SSML = 8,
	
#if 1 //defined(USE_OUTPUT_MARGIN_EACH_REQUEST) // with // sjchang 2021-05-13 Add for v3.12.7.x
	ATTR_FRONT_MARGIN = 9,
	ATTR_REAR_MARGIN = 10,
	ATTR_MIDDLE_MARGIN = 11,
#endif

#if 1 // defined(USE_USERCONFIG) // sjchang 2022-08-05 Add for UserConfig
	ATTR_CONFIGIDX = 12, 
#endif

	ATTR_MAX
} Attr_Flags;

typedef enum PRIORITYTYPES
{
	PRIOR_DEFAULT = 0,
	PRIOR_OVERLAP = 1,
	PRIOR_ALERT = 2,
} Priority_Types;

#define MAX_STR_LEN				256
#define MAX_ERR_MSG				512

typedef struct _audio_format
{
	int					nChannel;
	int					nBits;
	int					nSampling;
} VTAPI_AUDIO_FORMAT;

typedef struct _audio_info
{
	VTAPI_AUDIO_FORMAT	audiofmt;
	int					nDevNo;
} VTAPI_AUDIO_INFO;

typedef struct
{
	int					nId;
	int					nTextStartOffset;			// Text position of Word or Sentence or Mark
	int					nTextEndOffset;				// Text position of Word or Sentence or Mark
	char				szStrValue[MAX_STR_LEN];	// Name of Mark

	int					nOffsetInStream;			// Buffer position of Mark
	int					nOffsetInBuffer;			// Buffer position of Mark

	int					nError;						// Error // VTAPI4 Version over 4.2.2.x

	int					nBufferSize;				// Buffer size of Word or FrameBuffer
	char*				pFrameBuffer;				// FrameBuffer
} VTAPI_AUDIO_EVENT;


typedef struct _vtapi_errs
{
	int nErr;
	char szMsg[MAX_ERR_MSG];
} VTAPI_ERRS_INFO;


#define		VTAPI_SUCCESS						 0

#define		VTAPI_INVALID_PARAM_ERROR			-1		
#define		VTAPI_INVALID_VALUE_ERROR			-2
#define		VTAPI_INCORRECT_SET_ERROR			-3
#define		VTAPI_INVALID_ID_OR_THREAD_ERROR	-4
#define	    VTAPI_USERDICT_ERROR				-5
#define		VTAPI_SSML_SYNTAX_ERROR				-6
#define		VTAPI_MEMORY_ALLOC_ERROR			-7
#define		VTAPI_NOT_OPENBUFFER_ERROR			-8
#define		VTAPI_VTSSML_INVALID_ERROR			-9
#define		VTAPI_INVALID_ENGINE_ERROR			-10
#define		VTAPI_OVER_CHANNEL_ERROR			-11
#define		VTAPI_NODATA_ERROR					-12
#define		VTAPI_NOT_SUPPORTED_ERROR			-13

#define		VTAPI_DLL_VTPLAY_NOT_LINKING		-21
#define		VTAPI_DLL_VTSSML_NOT_LINKING		-22
#define		VTAPI_DLL_VTSAVE_NOT_LINKING		-23
#define		VTAPI_DLL_VTCONV_NOT_LINKING		-24
#define		VTAPI_DLL_VTEFFECT_NOT_LINKING		-25



// API for VTAPI

// VTAPI_INIT Return Value : You can check by XOR the lower values.
//                           If all Library links succeed, 0
#define VTAPI_LIB_VTPLAY_NOT_LINKING 0x1
#define VTAPI_LIB_VTSAVE_NOT_LINKING 0x10
#define VTAPI_LIB_VTCONV_NOT_LINKING 0x100
#define VTAPI_LIB_VTSSML_NOT_LINKING 0x1000
#define VTAPI_LIB_VTEFFECT_NOT_LINKING 0x10000

VTAPI_decl int VTAPI_Init(char *szWorkingDir); // It is automatically Initiated when using other functions.
VTAPI_decl void VTAPI_Exit();
VTAPI_decl void VTAPI_SetLicenseFolder(char *szPath);

// API for VTAPI-Voice
VTAPI_decl VTAPI_HANDLE VTAPI_CreateHandle();
VTAPI_decl void VTAPI_ReleaseHandle(VTAPI_HANDLE hVTAPI);


VTAPI_decl int VTAPI_SetEngineHandle(VTAPI_HANDLE hVTAPI, VTAPI_ENGINE_HANDLE hEngine);
VTAPI_decl VTAPI_ENGINE_HANDLE VTAPI_GetEngineHandle(VTAPI_HANDLE hVTAPI);

VTAPI_decl int VTAPI_SetOutputAudio(VTAPI_HANDLE hVTAPI, VTAPI_AUDIO_INFO stAudioInfo);
VTAPI_decl int VTAPI_SetOutputFile(VTAPI_HANDLE hVTAPI, char *pszFileName, int nOutputFormat);
VTAPI_decl int VTAPI_SetOutputFile_UTF8(VTAPI_HANDLE hVTAPI, char *pszFileName, int fileNameLen, int nOutputFormat);
VTAPI_decl int VTAPI_SetOutputBuffer(VTAPI_HANDLE hVTAPI, int nOutputFormat);

VTAPI_decl int VTAPI_SetAttr(VTAPI_HANDLE hVTAPI, int nFlag, int nValue);
VTAPI_decl int VTAPI_GetAttr(VTAPI_HANDLE hVTAPI, int nFlag, int *nValue);


#if !defined(__VTAPI_DEFINED_CALLBACK_FUNCTION__)
#define __VTAPI_DEFINED_CALLBACK_FUNCTION__
typedef void  (*LPPEventProc)(VTAPI_HANDLE hVTAPI, int nEventType, void *pParam);
#endif

VTAPI_decl int VTAPI_SetEvent(VTAPI_HANDLE hVTAPI, void *pEventProc, int nEventTypes);

#if defined(WIN32) || defined(_WIN32) || defined(WINCE)
// API for Audio Output (VTAPI_Speak - File Output included)
VTAPI_decl int VTAPI_Speak(VTAPI_HANDLE hVTAPI, void *pszText, int nTextLength, int nTextType);
VTAPI_decl int VTAPI_Stop(VTAPI_HANDLE hVTAPI);
VTAPI_decl int VTAPI_Pause(VTAPI_HANDLE hVTAPI);
VTAPI_decl int VTAPI_Resume(VTAPI_HANDLE hVTAPI);
VTAPI_decl int VTAPI_SetPriority(VTAPI_HANDLE hVTAPI, int nPrior);
#endif

// API for FileSave
VTAPI_decl int VTAPI_TextToFile(VTAPI_HANDLE hVTAPI, void *pszText, int nTextLength, int nTextType);

// API for Buffer
typedef enum BUFSIZE_VALUE
{
    VAL_DEFAULT = -1,
    VAL_ONEBUF = 0,
} BUFSIZE_VALUE;
VTAPI_decl int VTAPI_BufferOpen(VTAPI_HANDLE hVTAPI, void *pszText, int nTextLength, int nTextType, int nBufferSize);
VTAPI_decl int VTAPI_GetBuffer(VTAPI_HANDLE hVTAPI, char** pFrameBuffer);
VTAPI_decl void VTAPI_FreeBuffer(char* ptr);
VTAPI_decl int VTAPI_BufferClose(VTAPI_HANDLE hVTAPI);

VTAPI_decl VTAPI_ERRS_INFO *VTAPI_GetLastErrorInfo(VTAPI_HANDLE hVTAPI);

VTAPI_decl int VTAPI_SetUserData(VTAPI_HANDLE hVTAPI, void *ptr);
VTAPI_decl void *VTAPI_GetUserData(VTAPI_HANDLE hVTAPI);

VTAPI_decl void VTAPI_GetVersion(char output[100]);
VTAPI_decl int VTAPI_GetSSMLVersion(char output[100]);

// API for tts engine
VTAPI_decl VTAPI_ENGINE_HANDLE VTAPI_GetFirstInstalledEngine();
VTAPI_decl VTAPI_ENGINE_HANDLE VTAPI_GetNextInstalledEngine(VTAPI_ENGINE_HANDLE hEngine);
VTAPI_decl int VTAPI_UpdateInstalledEngine();
VTAPI_decl VTAPI_ENGINE_HANDLE VTAPI_GetEngine(char *pszSpeaker, char *pszType);
VTAPI_decl VTAPI_ENGINE_HANDLE VTAPI_GetEngineEx(char *pszSpeaker, char *pszType, char *pszDllPath, char *pszDBPath);
VTAPI_decl VTAPI_ENGINE_HANDLE VTAPI_GetEngineEx2(char *pszLang, char *pszGender);

VTAPI_decl int VTAPI_GetEngineInfoField(VTAPI_ENGINE_HANDLE hEngine, char *pszSpeaker, char *pszType);
VTAPI_decl int VTAPI_GetEngineInfoFieldEx(VTAPI_ENGINE_HANDLE hEngine, char *pszSpeaker, char *pszType,char *pszDllPath, char *pszDBPath, char *pszLang, char *pszGender, char *pszVersion, int *nSampling, int *nChannel, char *pszISOCode);

VTAPI_decl int VTAPI_LoadEngine(VTAPI_ENGINE_HANDLE hEngine);
VTAPI_decl int VTAPI_LoadEngineEx(VTAPI_ENGINE_HANDLE hEngine, HWND hWnd);
VTAPI_decl int VTAPI_UnloadEngine(VTAPI_ENGINE_HANDLE hEngine);
VTAPI_decl int VTAPI_LoadUserDict(VTAPI_ENGINE_HANDLE hEngine, int nDictIdx, char *pszDicFile);
VTAPI_decl int VTAPI_UnloadUserDict(VTAPI_ENGINE_HANDLE hEngine, int nDictIdx);
VTAPI_decl int VTAPI_GetEngineVersion(VTAPI_ENGINE_HANDLE hEngine, char output[100]);

#if 1 //defined(USE_NEW_USERDICT_MANAGER)
// API for dictionary manager
VTAPI_decl VTAPI_USERDICT_HANDLE VTAPI_CreateUserDictHandle(char* pszDicFile);
VTAPI_decl void VTAPI_ReleaseUserDictHandle(VTAPI_USERDICT_HANDLE hUSERDICT);
VTAPI_decl int VTAPI_SetUserDictHandle(VTAPI_HANDLE hVTAPI, VTAPI_USERDICT_HANDLE hUSERDICT);
VTAPI_decl VTAPI_USERDICT_HANDLE VTAPI_GetUserDictHandle(VTAPI_HANDLE hVTAPI);
VTAPI_decl int VTAPI_GetUserDictInfo(VTAPI_USERDICT_HANDLE hUSERDICT, char* pszDicFile);
#endif

#if 1 //defined(FOR_VTAPI_VER_44)
#if 1 //defined(USE_RELEXICON_DICT)
// Support lexicon
VTAPI_decl int VTAPI_LoadLexiconFromFile(VTAPI_HANDLE hVTAPI, char *pszLexFile);
VTAPI_decl int VTAPI_UnloadLexicon(VTAPI_HANDLE hVTAPI);

VTAPI_decl VTAPI_LEXICON_HANDLE VTAPI_CreateLexiconHandle(char* pszLexFile);
VTAPI_decl void VTAPI_ReleaseLexiconHandle(VTAPI_LEXICON_HANDLE hLEXICON);
VTAPI_decl int VTAPI_SetLexiconHandle(VTAPI_HANDLE hVTAPI, VTAPI_LEXICON_HANDLE hLEXICON);
VTAPI_decl VTAPI_LEXICON_HANDLE VTAPI_GetLexiconHandle(VTAPI_HANDLE hVTAPI);
VTAPI_decl int VTAPI_GetLexiconInfo(VTAPI_LEXICON_HANDLE hLEXICON, char* pszLexFile);
#endif
#endif

#if defined(__cplusplus)
}
#endif

#endif /* _VTAPI_H_ */
