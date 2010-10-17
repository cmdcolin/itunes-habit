#pragma once



#if TARGET_OS_WIN32
#define	MAIN iTunesPluginMain
#define IMPEXP	__declspec(dllexport)
#else
#define IMPEXP
#define	MAIN main
#endif

//########################################
//	typedef's, struct's, enum's, etc.
//########################################
#define kTVisualPluginName              "\014iTunesPlugIn"
#define	kTVisualPluginCreator			'hook'

#define	kTVisualPluginMajorVersion		1
#define	kTVisualPluginMinorVersion		2
#define	kTVisualPluginReleaseStage		developStage
#define	kTVisualPluginNonFinalRelease	0



enum
{
    kColorSettingID = 3, 
    kOKSettingID = 5
};


struct VisualPluginData {

    
    VisualPluginData()
    {
        cout << __FUNCTION__ << "\n";
    }

    ~VisualPluginData()
    {
        cout << __FUNCTION__ << "\n";
    }


    void *				appCookie;
    ITAppProcPtr			appProc;

#if TARGET_OS_MAC
    CGrafPtr			destPort;
#else
    HWND				destPort;
#endif
    Rect				destRect;
    OptionBits			destOptions;
    UInt32				destBitDepth;

    RenderVisualData		renderData;
    UInt32				renderTimeStampID;

    ITTrackInfoV1	trackInfo;
    ITStreamInfoV1	streamInfo;
    ITTrackInfo     trackUniInfo;
    ITStreamInfo    streamUniInfo;

    Boolean				playing;
    Boolean				padding[3];

    //	Plugin-specific data
    UInt8				minLevel[kVisualMaxDataChannels];		// 0-128
    UInt8				maxLevel[kVisualMaxDataChannels];		// 0-128

    UInt8				min, max;
};
typedef struct VisualPluginData VisualPluginData;