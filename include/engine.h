/***********************************************
 * tts sample program
 * made by ReadSpeaker
 ***********************************************/
#ifndef _ENGINE_H_
#define _ENGINE_H_
#include <string>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(VT_BASIC_DEFINE)
#if defined(WIN32)
#if !defined(_DllMode)
#define _DllMode(_type_)		__declspec( dllimport ) _type_
#endif
#elif defined (__WINRT__)
#define		_DllMode(_type_)		_type_
#else
#if !defined(_DllMode)
#define		_DllMode(_type_)		extern _type_
#endif
    typedef		int						HWND;
#endif
#endif

#if 0
    /* Return Value for user dic*/
#define		UDIC_SUCCESS				(1)
#define		UDIC_ERR_RANGE          	(-1)
#define		UDIC_ERR_ALREADY_LOADED     (-2)
#define		UDIC_ERR_NOT_FOUND			(-3)
#define		UDIC_ERR_ETC				(-4)


/* Return Value */
#define		QA_SUCCESS						(1)
#define		QA_ERROR_INVALID_FORMAT			(-1)
#define		QA_ERROR_CREATE_THREAD			(-2)
#define		QA_ERROR_NULL_TEXT				(-3)
#define		QA_ERROR_EMPTY_TEXT				(-4)
#define		QA_ERROR_DB_NOT_LOADED			(-5)
#define		QA_ERROR_OUT_FILE_OPEN			(-6)
#define		QA_ERROR_UNKNOWN				(-7)
#endif

#define ENGINE_INTERNAL
#ifdef ENGINE_INTERNAL
/* new syncinfo */
    typedef struct _phonesync_new
    {
        int			m_nLength;
        short		m_nId;
#if 1 // sjchang 2020-06-02 Modify with VTEngine v3.12.5.x
        short		m_nId_sampa;
        short		m_nId_viseme;
#else
#if 1 // sjchang 2019-09-25 All VTEngine from 2019-09-25(v3.12.3.0) with upper 4.2.1.12 / 4.2.2.2
        short		m_nSampaId;
#else
        //short		m_nId_sampa;	// sampa id // sjchang 2018-11-09 Comment for not-exsit engine (/CHI/JPN/SWE/TWN/YUE/)
#endif
#endif
    } PHONESYNC_NEW;

    typedef struct _wordsync_new
    {
        short		m_nNumberOfPhones;
        PHONESYNC_NEW* m_pPhone;
        int			m_nLength;
        int			m_nWordStartPosInText;
        int			m_nWordEndPosInText;
        int			m_nSentStartPosInText;
        int			m_nSentEndPosInText;
        int			m_nNumberOfWordsInSent;
        int			m_nIndexOfWordsInSent;
    } WORDSYNC_NEW;

    typedef struct _buffersync
    {
        int			m_nOffsetInStream;
        int			m_nIndexOfWordSync;
        int			m_nOffsetInWord;
        int			m_nIndexOfPhoneSync;
        int			m_nOffsetInPhone;
    } BUFFERSYNC;

    typedef struct _syncinfo_new
    {
        WORDSYNC_NEW* m_pWord;
        int			m_nMaxWordForSync;
        int			m_nMaxPhoneForSync;
        int			m_nNumberOfValidWordSync;
        BUFFERSYNC	m_BufferStart;
        BUFFERSYNC	m_BufferEnd;
    } SYNCINFO_NEW, * PSYNCINFO_NEW;


//#define NEW_MARKINFO    // mark 정보를 직접 계산하는 방식.(엔진정보와 차이를 보일 수 있음)
    typedef struct QASyncInfo
    {
        int			offsetwordstart;
        int			offsetwordend;
        int			wordlength_sample;
#ifdef NEW_MARKINFO
        int			offsetinbuffer_sample = 0;
        int			endoffsetinstream = 0;
        int			wordlength_sample_before = 0;
        int			touch = 0;
        int			lastoffsetword = 0;
#endif
    } QASYNCINFO;

#if 0 // sjchang 2020-06-18 Modify with VTEngine v3.12.5.x
    /* new syncinfo */
    typedef struct _phonesync_old
    {
        int			m_nLength;
        short		m_nId;
        short		m_nId_sampa; // sjchang 2019-09-25 All VTEngine from 2019-09-25(v3.12.3.0) with upper 4.2.1.12 / 4.2.2.2
    } PHONESYNC_OLD;

    typedef struct _wordsync_old
    {
        short		m_nNumberOfPhones;
        PHONESYNC_OLD* m_pPhone;
        int			m_nLength;
        int			m_nWordStartPosInText;
        int			m_nWordEndPosInText;
        int			m_nSentStartPosInText;
        int			m_nSentEndPosInText;
        int			m_nNumberOfWordsInSent;
        int			m_nIndexOfWordsInSent;
    } WORDSYNC_OLD;

    typedef struct _syncinfo_old
    {
        WORDSYNC_OLD* m_pWord;
        int			m_nMaxWordForSync;
        int			m_nMaxPhoneForSync;
        int			m_nNumberOfValidWordSync;
        BUFFERSYNC	m_BufferStart;
        BUFFERSYNC	m_BufferEnd;
    } SYNCINFO_OLD, * PSYNCINFO_OLD;
#endif

    /* mark array structure for mark tag */
#define		MAX_MARK_NAME				(512)
#define		MARK_STATUS_GENERAL			(1)
#define		MARK_STATUS_EMPTY_STRING	(2)
#define		MARK_STATUS_TRIMMED_STRING	(3)
    typedef struct
    {
        int		m_nOffsetInStream;
        int		m_nOffsetInBuffer;
        int		m_nPosInText;
        char	m_sMarkName[MAX_MARK_NAME];
        char	m_bMarkStatus;
    } TTSMark;

    typedef struct
    {
        int		m_nSize;
        TTSMark* m_Marks;
        int		m_nStartIndexOfMarks;
        int		m_nEndIndexOfMarks;
    } TTSMarkArray;

    typedef struct QAMarkInfo
    {
        int		m_nOffsetInStream;
        int		m_nOffsetInBuffer;
        int		m_nPosInText;
        char	m_sMarkName[MAX_MARK_NAME];
        char	m_bMarkStatus;
    } QAMARKINFO;
#endif


    typedef enum LICENSEINFO
    {
        LICENSE_CHANNEL_NO = 0,
        LICENSE_EXPIRE_DATE = 1,
        LICENSE_HOSTID = 2,
        LICENSE_DETAILED_ALL = 3,
        LICENSE_DETAILED_OS = 4,
        LICENSE_DETAILED_LANG = 5,
        LICENSE_DETAILED_SPEAKER = 6,
        LICENSE_DETAILED_VERSION = 7,
        LICENSE_DETAILED_DBACCESS = 8,
        LICENSE_DETAILED_SAMPLING = 9,
        LICENSE_DETAILED_APP = 10,
        LICENSE_DETAILED_WAVSAVE = 11,
        LICENSE_DETAILED_SAVETIME = 12,
        LICENSE_DETAILED_BGAUDIO = 13,
        LICENSE_DETAILED_DBSIZE = 14,
        LICENSE_DETAILED_REALTIME = 15,
        LICENSE_MAX
    } LICENSEINFO;

    typedef enum LANGIDX {
        KOR = 0,
        ENG = 1,
        CHI = 2,
        JPN = 3,
        SPA = 4,
        BRE = 5,
        CFR = 6,
        TWN = 7,
        YUE = 8,
        THA = 9,
        BPT = 10,
        GER = 11,
        ESP = 12,
        ITA = 13,
        FRE = 14,
        POR = 15,
        SWE = 16,
        DUT = 17,
        RUS = 18,
        AUE = 19,
        HUN = 20,
        IND = 21,
        RON = 22,
        SLK = 23,
        NOR = 24,
        ASP = 25,
        HIN = 26,
        CES = 27,
        INE = 28,
        ARB = 29,
        POL = 30,
        FIN = 32,
        DAN = 33,
        LAV = 34,
        CYM = 39,
        VIE = 40,
        ELL = 46,
        TAM = 50,
        BEN = 51,
        MAX
    } LANGIDX_E;

    /* engine info */
    enum
    {
        ENGN_SPEAKERID = 0,
        //INFO_CODEPAGE,
        //INFO_LANGUAGE,
        ENGN_TYPE,
        ENGN_LIBPATH,
        ENGN_DBPATH,
        ENGN_USERDICPATH,
        ENGN_MAX
    };

    /* speaker info */
    enum
    {
        SPK_LANG = 0,
        SPK_CODEPAGE,
        SPK_ISO,
        SPK_SPEAKER,
        SPK_GENDER,
        SPK_SPEAKERID,
        SPK_MAX
    };

    /* Request */
    enum
    {
        QA_BUILD_DATE		   =  0,
        QA_VERIFY_CODE         =  1,
        QA_MAX_CHANNEL         =  2,
        QA_DB_DIRECTORY        =  3,
        QA_LOAD_SUCCESS_CODE   =  4,
        QA_MAX_SPEAKER         =  5,
        QA_DEF_SPEAKER         =  6,
        QA_CODEPAGE            =  7,
        QA_DB_ACCESS_MODE      =  8,
        QA_FIXED_POINT_SUPPORT =  9,
        QA_SAMPLING_FREQUENCY  = 10,
        QA_MAX_PITCH_RATE      = 11,
        QA_DEF_PITCH_RATE      = 12,
        QA_MIN_PITCH_RATE      = 13,
        QA_MAX_SPEED_RATE      = 14,
        QA_DEF_SPEED_RATE      = 15,
        QA_MIN_SPEED_RATE      = 16,
        QA_MAX_VOLUME          = 17,
        QA_DEF_VOLUME          = 18,
        QA_MIN_VOLUME          = 19,
        QA_MAX_SENT_PAUSE	   = 20,
        QA_DEF_SENT_PAUSE	   = 21,
        QA_MIN_SENT_PAUSE      = 22,
        QA_DB_BUILD_DATE       = 23,
        QA_MAX_COMMA_PAUSE	   = 24,
        QA_DEF_COMMA_PAUSE	   = 25,
        QA_MIN_COMMA_PAUSE	   = 26,
    };

    /* Audio Format */
    typedef enum AUDIOFORMAT {
        S16PCM      = 0,
        ALAW        = 1,
        MULAW       = 2,
        DADPCM      = 3,
        S16PCM_WAVE = 4,
        U08PCM_WAVE = 5,
    //	IMA_WAVE	= 6, /* not supported! */
        ALAW_WAVE   = 7,
        MULAW_WAVE  = 8,
        MULAW_AU    = 9,
    } AUDIOFORMAT_E ;

    typedef struct LANG_INFO {
        LANGIDX lang_idx;
        char* lang_code;
    } LANG_INFO_ST;

    typedef struct QA_RESULT {
        std::string result;
        int errorcode = -1;
        std::string errorreason;
    } QA_RESULT_ST;

    typedef struct DLFUNC {
        short (*VT_LOADTTS)(int hWnd, int nSpeakerID, char *db_path, char *licensefile);
        void (*VT_UNLOADTTS)(int nSpeakerID);
        short (*VT_LOAD_UserDict)(int dictidx, char *filename);
        short (*VT_UNLOAD_UserDict)(int dictidx);
        short (*VT_PLAYTTS)(int hcaller, uint32_t umsg, char *text_buff, int nSpeakerID, int pitch, int speed, int volume, int pause, int dictidx, int texttype);
        void (*VT_STOPTTS)(void);
        void (*VT_RESTARTTTS)(void);
        void (*VT_PAUSETTS)(void);
        short (*VT_TextToFile)(int fmt, char *tts_text, char *filename, int nSpeakerID, int pitch, int speed, int volume, int pause, int dictidx, int texttype);
        int (*VT_TextToBuffer)(int fmt, char *tts_text, char *output_buff, int *output_len, int flag, int nThreadID, int nSpeakerID, int pitch, int speed, int volume, int pause, int dictidx, int texttype);
        void (*VT_SetPitchSpeedVolumePause)(int pitch, int speed, int volume, int pause, int nSpeakerID);
        void (*VT_SetCommaPause)(int pause, int nSpeakerID);
        void (*VT_SetParenthesisCharNumber)(int nByte);
        int (*VT_GetTTSInfo)(int request, char *licensefile, void *value, int valuesize);

#ifdef ENGINE_INTERNAL
        int (*VT_TextToBufferEX)(int fmt, char* tts_text, char* output_buff, int* output_len, int flag, int nThreadID, int nSpeakerID, SYNCINFO_NEW** ppSyncInfoNew, TTSMarkArray** ppMarkArray, int pitch, int speed, int volume, int pause, int dictidx, int texttype);
#if 0
        int (*VT_GetLicenseInfo)(int request, char* licensefile, void* pLicensePtr, int nLicenseSize, void* value, int valuesize);
        PSYNCINFO_NEW (*VT_AllocSyncInfo_New)(void);
        void (*VT_FreeSyncInfo_New)(PSYNCINFO_NEW pSyncInfo);
        void (*VT_InitSyncInfo_New)(PSYNCINFO_NEW pSyncInfo);
        int (*VT_TextToPhoneme)(char* tts_text, char* phoneme, int* phoneme_len, int flag, int pitch, int speed, int volume, int pause, int dictidx, int texttype);
        short (*VT_TextToPhonemeFile)(char* tts_text, char* filename, int pitch, int speed, int volume, int pause, int dictidx, int texttype);
        int (*VT_TextToTranscription)(char* tts_text, char* phoneme, int* phoneme_len, int flag, int pitch, int speed, int volume, int pause, int dictidx, int texttype);
        short (*VT_TextToPron)(char* tts_text, char* outPron, int* pszSize_len, int flag, int mode, int dictidx, int texttype);
        void (*VT_SetFirstLastPause)(int first_pause, int last_pause);
        int (*VT_GetFirstLastPause)(int* first_pause, int* last_pause);
        short (*VT_LOAD_UserConfig)(int dictidx, char *filename);
        short (*VT_UNLOAD_UserConfig)(int dictidx);
        int (*VT_TextToBufferEX_UserConfig)(int fmt, char *tts_text, char *output_buff, int *output_len, int flag, int nThreadID, int nSpeakerID, SYNCINFO_NEW **ppSyncInfoNew, TTSMarkArray **ppMarkArray, int pitch, int speed, int volume, int pause, int dictidx, int texttype, int configidx);
#endif
#endif
    } DLFUNC_ST;

    typedef struct ENGN_INFO {
        int speakerid = -1;
        //std::string codpage;
        //std::string lang;
        std::string type;
        std::string libpath;
        std::string dbpath;
        std::string userdic;
    } ENGN_INFO_ST;

    typedef struct LIB_SPEC {
        DLFUNC func;
        void* handle = nullptr;
        bool loadedlib = false;
        ENGN_INFO_ST engninfo;
        std::string builddate;
        int verifycode = -1;
        int maxchannel = -1;
        std::string dbdir;
        int loadsuccesscode = -1;
        int maxspeaker = -1;
        int defspeaker = -1;
        int codpage = -1;
        int dbaccessmode = -1;
        int fixedpoint = -1;
        int samplerate = -1;
        int maxpitchrate = -1;
        int defpitchrate = -1;
        int minpitchrate = -1;
        int maxspeed = -1;
        int defspeed = -1;
        int minspeed = -1;
        int maxvolume = -1;
        int defvolume = -1;
        int minvolume = -1;
        int maxsentpause = -1;
        int defsentpause = -1;
        int minsentpause = -1;
        std::string dbbuilddate;
        int maxcommapause = -1;
        int defcommapause = -1;
        int mincommapause = -1;
        QA_RESULT_ST report;
    } LIB_SPEC_ST;

    typedef struct SPEAKER_INFO {
        std::string lang;
        int codepage = -1;
        std::string iso;
        std::string speaker;
        std::string gender;
        int speakerid = -1;
    } SPEAKER_INFO_ST;

    /* TC script */

//#define USE_SYNTH_BY_VTAPI_ENGINETEXT	//synthsize with engine-textfiles that are generated by vtapi
#ifdef USE_SYNTH_BY_VTAPI_ENGINETEXT
    enum //QATool enginetext format
    {
        SC_INDEX = 0,
        SC_UCID,
        SC_SPEAKER,
        SC_TYPE,
        SC_LANG,
        SC_GENDER,
        SC_USERDIC,
        SC_TEXTLEN,
        SC_PITCH,
        SC_SPEED,
        SC_VOLUME,
        SC_SENT_PAUSE,
        SC_COMMA_PAUSE,
        SC_BUFSIZE,
        SC_OUTPUTFORMAT,
        SC_MAX
    };
#else
    enum
    {
        SC_INDEX = 0,
        SC_SPEAKERID,
        //SC_LANGUAGE,
        SC_TYPE,
        SC_AUDIOFORMAT,
        SC_PITCH,
        SC_SPEED,
        SC_VOLUME,
        SC_SENT_PAUSE,
        SC_COMMA_PAUSE,
        SC_TEXT,
        SC_USERDIC,
        SC_MAX
    };
#endif

    typedef struct TEST_SCRIPT {
        std::string index;
        int speakerid = -1;
        //std::string lang;
        std::string type;
        std::string audioformat;
        int pitchrate = -1;
        int speed = -1;
        int volume = -1;
        int sentpause = -1;
        int commapause = -1;
        std::string ttstext;
        std::string userdic;
        QA_RESULT_ST report;
        ENGN_INFO_ST* usedengninfo;
    } TEST_STCRIPT_ST;

#if defined(__cplusplus)
    }
#endif

#endif  /*  _ENGINE_H_ */
