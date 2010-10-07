//
// File:       iTunesPlugIn.c
//
// Abstract:   visual plug-in for iTunes
//
// Version:    1.2
//
// Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc. ( "Apple" )
//             in consideration of your agreement to the following terms, and your use,
//             installation, modification or redistribution of this Apple software
//             constitutes acceptance of these terms.  If you do not agree with these
//             terms, please do not use, install, modify or redistribute this Apple
//             software.
//
//             In consideration of your agreement to abide by the following terms, and
//             subject to these terms, Apple grants you a personal, non - exclusive
//             license, under Apple's copyrights in this original Apple software ( the
//             "Apple Software" ), to use, reproduce, modify and redistribute the Apple
//             Software, with or without modifications, in source and / or binary forms;
//             provided that if you redistribute the Apple Software in its entirety and
//             without modifications, you must retain this notice and the following text
//             and disclaimers in all such redistributions of the Apple Software. Neither
//             the name, trademarks, service marks or logos of Apple Inc. may be used to
//             endorse or promote products derived from the Apple Software without specific
//             prior written permission from Apple.  Except as expressly stated in this
//             notice, no other rights or licenses, express or implied, are granted by
//             Apple herein, including but not limited to any patent rights that may be
//             infringed by your derivative works or by other works in which the Apple
//             Software may be incorporated.
//
//             The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
//             WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
//             WARRANTIES OF NON - INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
//             PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION
//             ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
//
//             IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
//             CONSEQUENTIAL DAMAGES ( INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//             SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             INTERRUPTION ) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
//             AND / OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
//             UNDER THEORY OF CONTRACT, TORT ( INCLUDING NEGLIGENCE ), STRICT LIABILITY OR
//             OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright ( C ) 2001-2007 Apple Inc. All Rights Reserved.
//

//########################################
//	includes
//########################################
#include "stdafx.h"
#undef DOMDocument
#include "iTunesVisualAPI.h"
#include "iTunesPluginUtils.hpp"
#include "guicon.h"


#include <commctrl.h>

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

    ITTrackInfo			trackInfo;
    ITStreamInfo			streamInfo;

    Boolean				playing;
    Boolean				padding[3];

    //	Plugin-specific data
    UInt8				minLevel[kVisualMaxDataChannels];		// 0-128
    UInt8				maxLevel[kVisualMaxDataChannels];		// 0-128

    UInt8				min, max;
};
typedef struct VisualPluginData VisualPluginData;

//########################################
//	local ( static ) globals
//########################################

static Boolean	gColorFlag = TRUE;



extern OSStatus iTunesPluginMainMachO( OSType inMessage, PluginMessageInfo *inMessageInfoPtr, void *refCon );


//########################################
// ProcessRenderData
//########################################

static void ProcessRenderData( VisualPluginData *visualPluginDataPtr, const RenderVisualData *renderData )
{
    SInt16		index;
    SInt32		channel;

    if (renderData == nil)
    {
        ZeroMemory(&visualPluginDataPtr->renderData,sizeof(visualPluginDataPtr->renderData));
        return;
    }

    visualPluginDataPtr->renderData = *renderData;

    for (channel = 0;channel < renderData->numSpectrumChannels;channel++)
    {
        visualPluginDataPtr->minLevel[channel] = 
            visualPluginDataPtr->maxLevel[channel] = 
            renderData->spectrumData[channel][0];

        for (index = 1; index < kVisualNumSpectrumEntries; index++)
        {
            UInt8		value;

            value = renderData->spectrumData[channel][index];

            if (value < visualPluginDataPtr->minLevel[channel])
                visualPluginDataPtr->minLevel[channel] = value;
            else if (value > visualPluginDataPtr->maxLevel[channel])
                visualPluginDataPtr->maxLevel[channel] = value;
        }
    }
}


//########################################
//	RenderVisualPort
//########################################

static void RenderVisualPort(VisualPluginData *visualPluginData, GRAPHICS_DEVICE destPort,const Rect *destRect,Boolean onlyUpdate)
{

    (void) visualPluginData;
    (void) onlyUpdate;

    if (destPort == nil)
        return;

#if TARGET_OS_MAC	
    {	
        GrafPtr		oldPort;
        GDHandle	oldDevice;
        Rect		srcRect;
        RGBColor	foreColor;

        srcRect		= *destRect;

        GetGWorld(&oldPort, &oldDevice);
        SetGWorld(destPort, nil);

        foreColor.blue = ((UInt16)visualPluginData->maxLevel[0] << 9);
        if (gColorFlag)
            foreColor.red = foreColor.green = ((UInt16)visualPluginData->maxLevel[1] << 9);
        else
            foreColor.red = foreColor.green = foreColor.blue;

        RGBForeColor(&foreColor);
        PaintRect(destRect);

        SetGWorld(oldPort, oldDevice);
    }
#else
    {
        RECT	srcRect;
        HBRUSH	hBrush;
        HDC		hdc;

        srcRect.left = destRect->left;
        srcRect.top = destRect->top;
        srcRect.right = destRect->right;
        srcRect.bottom = destRect->bottom;

        hdc = GetDC(destPort);		
        if (gColorFlag)
            hBrush = CreateSolidBrush(RGB((UInt16)visualPluginData->maxLevel[1]<<1, (UInt16)visualPluginData->maxLevel[1]<<1, (UInt16)visualPluginData->maxLevel[0]<<1));
        else
            hBrush = CreateSolidBrush(RGB((UInt16)visualPluginData->maxLevel[1]<<1, (UInt16)visualPluginData->maxLevel[1]<<1, (UInt16)visualPluginData->maxLevel[1]<<1));
        FillRect(hdc, &srcRect, hBrush);
        DeleteObject(hBrush);
        ReleaseDC(destPort, hdc);
    }
#endif
}


// ChangeVisualPort
//
static OSStatus ChangeVisualPort(VisualPluginData *visualPluginData,GRAPHICS_DEVICE destPort,const Rect *destRect)
{
    OSStatus		status;

    status = noErr;

    visualPluginData->destPort = destPort;
    if (destRect != nil)
        visualPluginData->destRect = *destRect;

    return status;
}

/*
ResetRenderData
*/
static void ResetRenderData(VisualPluginData *visualPluginData)
{
    ZeroMemory(&visualPluginData->renderData,sizeof(visualPluginData->renderData));

    visualPluginData->minLevel[0] = 
        visualPluginData->minLevel[1] =
        visualPluginData->maxLevel[0] =
        visualPluginData->maxLevel[1] = 0;
}

#if TARGET_OS_MAC
/* 
settingsControlHandler
*/
pascal OSStatus settingsControlHandler(EventHandlerCallRef inRef,EventRef inEvent, void* userData)
{
    WindowRef wind=NULL;
    ControlID controlID;
    ControlRef control=NULL;
    //get control hit by event
    GetEventParameter(inEvent,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&control);
    wind=GetControlOwner(control);
    GetControlID(control,&controlID);
    switch(controlID.id){
case kColorSettingID:
    gColorFlag=GetControlValue(control);
    break;
case kOKSettingID:
    HideWindow(wind);
    break;
    }
    return noErr;
}
#endif

//
//string getFont(const string & folder)
//{
//    WIN32_FIND_DATAA wfd;
//    DWORD dwError;
//    HANDLE hFind = FindFirstFileA(folder.c_str(), &wfd);
//
//    vector<string> xx;
//    int counter = 0;
//
//    if (hFind == INVALID_HANDLE_VALUE) 
//    {
//    } 
//    else 
//    {
//        do 
//        {
//            string xxx = wfd.cFileName;
//            if(xxx.length() > 4)
//            {
//                string subby = xxx.substr(xxx.length() - 4);
//                if(subby == ".ttf")
//                {
//                    cout << __FUNCTION__
//                        << ": " << ("C:\\" + string(wfd.cFileName)).c_str();
//
//                    xx.push_back("C:\\" + string(wfd.cFileName));
//                    counter++;
//                }
//            }
//        } 
//        while(FindNextFileA(hFind, &wfd) != 0);
//
//        dwError = GetLastError();
//        FindClose(hFind);
//
//        if (dwError != ERROR_NO_MORE_FILES) 
//        {
//        }
//    }
//
//    string ss;
//
//    if(counter)
//        ss =  xx[rand() % counter];
//
//    cout << __FUNCTION__
//        << ": " << ss.c_str();
//
//    return ss;
//}


/*
VisualPluginHandler
*/

static OSStatus VisualPluginHandler(
                                    OSType message, 
                                    VisualPluginMessageInfo * messageInfo, 
                                    void * refCon
                                    )
{
    OSStatus			status;
    VisualPluginDataz *  vpd     = static_cast<VisualPluginDataz *>(refCon);

    status = noErr;

    switch (message)
    {    

    case 0x4337:
        {
            cout << __FUNCTION__
                << ": 0x4337";
        }
        break;

        /*
        Sent when the visual plugin is registered.  The plugin should do minimal
        memory allocations here.  The resource fork of the plugin is still available.
        */		
    case kVisualPluginInitMessage:
        {
            cout << __FUNCTION__
                << ": Init";

            INITCOMMONCONTROLSEX icx;

            icx.dwSize  = sizeof(INITCOMMONCONTROLSEX);
            icx.dwICC   = ICC_TAB_CLASSES;

            RedirectIOToConsole();
            InitCommonControlsEx(&icx);
            NormalizeCurrentDirectory();
            srand(seed());

            //try
            //{
            //    XMLPlatformUtils::Initialize();
            //}

            //catch(XMLException & e)
            //{
            //    MessageBox(HWND_DESKTOP, TEXT("Xerces XML API failed to initialize"),
            //        TEXT("Fatal Error"), MB_OK | MB_ICONEXCLAMATION);

            //    MessageBox(0, e.getMessage(), TEXT("Fatal Error"), MB_OK);

            //    status = unimpErr;
            //}

            vpd = new VisualPluginDataz(TEXT("C:/testestubertest.xml"), TEXT("C:/img.jpg"), TEXT("C:/log.dtd"), TEXT("C:/errors.log"));

            vpd->cookie	= messageInfo->u.initMessage.appCookie;
            vpd->proc = messageInfo->u.initMessage.appProc;

            messageInfo->u.initMessage.refCon = vpd;
        }
        break;

        /*
        Sent when the visual plugin is unloaded
        */		
    case kVisualPluginCleanupMessage:
        {
            cout << __FUNCTION__
                << ": Cleanup";

            delete vpd;

            XMLPlatformUtils::Terminate();
        }
        break;

        /*
        Sent when the visual plugin is enabled.  iTunes currently enables all
        loaded visual plugins.  The plugin should not do anything here.
        */
    case kVisualPluginEnableMessage:
    case kVisualPluginDisableMessage:
        break;

        /*
        Sent if the plugin requests idle messages.  Do this by setting the kVisualWantsIdleMessages
        option in the RegisterVisualMessage.options field.
        */
    case kVisualPluginIdleMessage:
        break;

        /*
        Sent if the plugin requests the ability for the user to configure it.  Do this by setting
        the kVisualWantsConfigure option in the RegisterVisualMessage.options field.
        */
#if TARGET_OS_MAC					
    case kVisualPluginConfigureMessage:
        {
            static EventTypeSpec controlEvent={kEventClassControl,kEventControlHit};
            static const ControlID kColorSettingControlID={'cbox',kColorSettingID};

            static WindowRef settingsDialog;
            static ControlRef color=NULL;

            IBNibRef 		nibRef;
            //we have to find our bundle to load the nib inside of it
            CFBundleRef iTunesPlugin;

            iTunesPlugin=CFBundleGetBundleWithIdentifier(CFSTR("com.apple.example.iTunes Visualizer"));
            if (iTunesPlugin == NULL) 
            {
                SysBeep(2);
            }
            else
            {
                CreateNibReferenceWithCFBundle(iTunesPlugin,CFSTR("SettingsDialog"), &nibRef);

                if (nibRef != nil)
                {
                    CreateWindowFromNib(nibRef, CFSTR("PluginSettings"), &settingsDialog);
                    DisposeNibReference(nibRef);

                    if (settingsDialog)
                    {
                        InstallWindowEventHandler(settingsDialog,NewEventHandlerUPP(settingsControlHandler),
                            1,&controlEvent,0,NULL);
                        GetControlByID(settingsDialog,&kColorSettingControlID,&color);

                        SetControlValue(color,gColorFlag);
                        ShowWindow(settingsDialog);
                    }
                }
            }
        }
        break;
#endif // TARGET_OS_MAC

        /*
        Sent when iTunes is going to show the visual plugin in a port.  At
        this point,the plugin should allocate any large buffers it needs.
        */
    case kVisualPluginShowWindowMessage:
        break;

        /*
        Sent when iTunes is no longer displayed.
        */
    case kVisualPluginHideWindowMessage:
        break;

        /*
        Sent when iTunes needs to change the port or rectangle of the currently
        displayed visual.
        */
    case kVisualPluginSetWindowMessage:
        break;

        /*
        Sent for the visual plugin to render a frame.
        */
    case kVisualPluginRenderMessage:
        break;
#if 0			
        /*
        Sent for the visual plugin to render directly into a port.  Not necessary for normal
        visual plugins.
        */
    case kVisualPluginRenderToPortMessage:
        status = unimpErr;
        break;
#endif 0
        /*
        Sent in response to an update event.  The visual plugin should update
        into its remembered port.  This will only be sent if the plugin has been
        previously given a ShowWindow message.
        */	
    case kVisualPluginUpdateMessage:
        break;

        /*
        Sent when the player starts.
        */
    case kVisualPluginPlayMessage:
        {
            cout << __FUNCTION__ << ": Play Song";

            vpd->trackInfo = *messageInfo->u.playMessage.trackInfo;
            vpd->streamInfo = *messageInfo->u.playMessage.streamInfo;

            vpd->trackUniInfo = *messageInfo->u.playMessage.trackInfoUnicode;
            vpd->streamUniInfo = *messageInfo->u.playMessage.streamInfoUnicode;

            vpd->playing = true;

            try
            {
                vpd->loggy->log(vpd->trackUniInfo);

                vpd->ic.createLastPlayedChart(
                    vpd->loggy->lastPlayedSongs(5), 
                    vpd->img_file.c_str(), 
                    "C:\\at.ttf",
                    24);

                //
                //#if defined(_UNICODE)
                //                if(!vpd->up.uploadFile(SX(vpd->img_file.c_str())))
                //#else
                //                if(!vpd->up.uploadFile(vpd->img_file.c_str()))
                //#endif

                {
                    cout << "Upload failed";
                }

                vpd->loggy->serialize(vpd->music_log_file);
            }
            catch(DOMException & e)
            {
                cout << "DOM Exception: " << e.getMessage();
            }
        }
        break;


    case kVisualPluginUnpauseMessage:
        {
            vpd->playing = true;
        }
        break;

        /*
        Sent when the player changes the current track information.  This
        is used when the information about a track changes,or when the CD
        moves onto the next track.  The visual plugin should update any displayed
        information about the currently playing song.
        */
    case kVisualPluginChangeTrackMessage:
        {
            cout << __FUNCTION__ << ": Change Track";

            vpd->trackInfo = *messageInfo->u.changeTrackMessage.trackInfo;
            vpd->streamInfo = *messageInfo->u.changeTrackMessage.streamInfo;

            vpd->trackUniInfo = *messageInfo->u.changeTrackMessage.trackInfoUnicode;
            vpd->streamUniInfo = *messageInfo->u.changeTrackMessage.streamInfoUnicode;

            vpd->playing = true;

            try
            {
                vpd->loggy->log(vpd->trackUniInfo);

                vpd->ic.createLastPlayedChart(
                    vpd->loggy->lastPlayedSongs(1), 
                    vpd->img_file.c_str(), 
                    "C:\\at.ttf",
                    72);


                vpd->loggy->serialize(vpd->music_log_file);
            }
            catch(DOMException &)
            {
                cout << "DOM Exception";
            }
        }
        break;

        /*
        Sent when the player stops.
        */
    case kVisualPluginStopMessage:
        break;

        /*
        Sent when the player changes position.
        */
    case kVisualPluginSetPositionMessage:
        break;


        /*
        Sent to the plugin in response to a MacOS event.  The plugin should return noErr
        for any event it handles completely,or an error (unimpErr) if iTunes should handle it.
        */
#if TARGET_OS_MAC
    case kVisualPluginEventMessage:
        {
            EventRecord* tEventPtr = messageInfo->u.eventMessage.event;
            if ((tEventPtr->what == keyDown) || (tEventPtr->what == autoKey))
            {    // charCodeMask,keyCodeMask;
                char theChar = tEventPtr->message & charCodeMask;

                switch (theChar)
                {
                case	'c':
                case	'C':
                    gColorFlag = !gColorFlag;
                    status = noErr;
                    break;

                default:
                    status = unimpErr;
                    break;
                }
            }
            else
                status = unimpErr;
        }
        break;
#endif // TARGET_OS_MAC

    default:
        status = unimpErr;
        break;
    }
    return status;	
}

/*
RegisterVisualPlugin
*/

enum
{
    kPluginMajorVersion = 1,
    kPluginMinorVersion = 1,
    kPluginReleaseStage = 1,
    kPluginNonFinalRelease = 2
};

static OSStatus RegisterVisualPlugin(PluginMessageInfo *messageInfo)
{
    cout << __FUNCTION__ << "\n";

    PlayerMessageInfo pm = {0};

    //
    // Set plug-in version information
    //

    SetNumVersion(&pm.u.registerVisualPluginMessage.pluginVersion,kTVisualPluginMajorVersion,kTVisualPluginMinorVersion,kTVisualPluginReleaseStage,kTVisualPluginNonFinalRelease);


    pm.u.registerVisualPluginMessage.timeBetweenDataInMS		= 0xFFFFFFFF;
    pm.u.registerVisualPluginMessage.numWaveformChannels		= 2;
    pm.u.registerVisualPluginMessage.numSpectrumChannels		= 2;

    pm.u.registerVisualPluginMessage.minWidth				= 64;
    pm.u.registerVisualPluginMessage.minHeight				= 64;
    pm.u.registerVisualPluginMessage.maxWidth				= 32767;
    pm.u.registerVisualPluginMessage.maxHeight				= 32767;

    pm.u.registerVisualPluginMessage.minFullScreenBitDepth	= 0;
    pm.u.registerVisualPluginMessage.maxFullScreenBitDepth	= 0;
    pm.u.registerVisualPluginMessage.windowAlignmentInBytes	= 0;
    pm.u.registerVisualPluginMessage.registerRefCon	        = 0;


    //
    // Plug-in name (length prepended)
    //


    Str255				pluginName = kTVisualPluginName;
    memcpy(&pm.u.registerVisualPluginMessage.name[0], &pluginName[0], pluginName[0] + 1);


    SetNumVersion(&pm.u.registerVisualPluginMessage.pluginVersion,kTVisualPluginMajorVersion,kTVisualPluginMinorVersion,kTVisualPluginReleaseStage,kTVisualPluginNonFinalRelease);

    pm.u.registerVisualPluginMessage.options					=	kVisualWantsIdleMessages;

    pm.u.registerVisualPluginMessage.handler					= (VisualPluginProcPtr)VisualPluginHandler;
    pm.u.registerVisualPluginMessage.registerRefCon			= 0;
    pm.u.registerVisualPluginMessage.creator					= kTVisualPluginCreator;

    pm.u.registerVisualPluginMessage.timeBetweenDataInMS		= 0xFFFFFFFF; // 16 milliseconds = 1 Tick,0xFFFFFFFF = Often as possible.
    pm.u.registerVisualPluginMessage.numWaveformChannels		= 2;
    pm.u.registerVisualPluginMessage.numSpectrumChannels		= 2;

    pm.u.registerVisualPluginMessage.minWidth				= 64;
    pm.u.registerVisualPluginMessage.minHeight				= 64;
    pm.u.registerVisualPluginMessage.maxWidth				= 32767;
    pm.u.registerVisualPluginMessage.maxHeight				= 32767;
    pm.u.registerVisualPluginMessage.minFullScreenBitDepth	= 0;
    pm.u.registerVisualPluginMessage.maxFullScreenBitDepth	= 0;
    pm.u.registerVisualPluginMessage.windowAlignmentInBytes	= 0;

    //pm.u.registerVisualPluginMessage.name[0] =
    //    static_cast<UInt8>(strlen("Habit"));

    //strcpy(reinterpret_cast<char *>(&pm.u.registerVisualPluginMessage.name[1]), "Habit");

    //
    // Register it
    //

    return PlayerRegisterVisualPlugin(messageInfo->u.initMessage.appCookie,
        messageInfo->u.initMessage.appProc, &pm);
}

//########################################
//	main entrypoint
//########################################

extern "C"
{
    IMPEXP OSStatus MAIN(OSType message,PluginMessageInfo *messageInfo,void *refCon)
    {

        {
            OSStatus status;

            (void) refCon;

            cout << __FUNCTION__ << "\n";

            switch (message)
            {
            case kPluginInitMessage:
                status = RegisterVisualPlugin(messageInfo);
                break;

            case kPluginCleanupMessage:
                status = noErr;
                break;

            default:
                status = unimpErr;
                break;
            }

            return status;
        }
    }
}