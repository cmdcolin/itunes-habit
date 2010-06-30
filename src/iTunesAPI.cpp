//
// File:       iTunesAPI.c
//
// Abstract:   part of iTunes Visual SDK
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
// Copyright ( C ) 2000-2007 Apple Inc. All Rights Reserved.
//
#include "iTunesAPI.h"
#include "iTunesVisualAPI.h"

// MyMemClear
//
static void MyMemClear (LogicalAddress dest, SInt32 length)
{
    register unsigned char	*ptr;

    ptr = (unsigned char *) dest;

    if( length > 16 )
    {
        register unsigned long	*longPtr;

        while( ((unsigned long) ptr & 3) != 0 )
        {
            *ptr++ = 0;
            --length;
        }

        longPtr = (unsigned long *) ptr;

        while( length >= 4 )
        {
            *longPtr++ 	= 0;
            length		-= 4;
        }

        ptr = (unsigned char *) longPtr;
    }

    while( --length >= 0 )
    {
        *ptr++ = 0;
    }
}


// SetNumVersion
//
void SetNumVersion (NumVersion *numVersion, UInt8 majorRev, UInt8 minorAndBugRev, UInt8 stage, UInt8 nonRelRev)
{
    numVersion->majorRev		= majorRev;
    numVersion->minorAndBugRev	= minorAndBugRev;
    numVersion->stage			= stage;
    numVersion->nonRelRev		= nonRelRev;
}


// ITCallApplication
//
static OSStatus ITCallApplicationInternal (void *appCookie, ITAppProcPtr handler, OSType message, UInt32 messageMajorVersion, UInt32 messageMinorVersion, PlayerMessageInfo *mi)
{
    PlayerMessageInfo	localmi;

    if (mi == nil)
    {
        MyMemClear(&localmi, sizeof(localmi));

        mi = &localmi;
    }

    mi->messageMajorVersion = messageMajorVersion;
    mi->messageMinorVersion = messageMinorVersion;
    mi->messageInfoSize	 = sizeof(PlayerMessageInfo);

    return handler(appCookie, message, mi);
}

// ITCallApplication
//
OSStatus ITCallApplication (void *appCookie, ITAppProcPtr handler, 
                            OSType message, PlayerMessageInfo *mi)
{
    return ITCallApplicationInternal(appCookie, handler, message, 
        kITPluginMajorMessageVersion, 
        kITPluginMinorMessageVersion, mi);
}


// PlayerSetFullScreen
//
OSStatus PlayerSetFullScreen (void *appCookie, ITAppProcPtr appProc, 
                              Boolean fullScreen)
{
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.setFullScreenMessage.fullScreen = fullScreen;

    return ITCallApplication(appCookie, appProc, 
        kPlayerSetFullScreenMessage, &mi);
}


// PlayerSetFullScreenOptions
//
OSStatus PlayerSetFullScreenOptions (void *appCookie, ITAppProcPtr appProc, 
                                     SInt16 minBitDepth, SInt16 maxBitDepth, 
                                     SInt16 preferredBitDepth, 
                                     SInt16 desiredWidth, SInt16 desiredHeight)
{
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.setFullScreenOptionsMessage.minBitDepth		= minBitDepth;
    mi.u.setFullScreenOptionsMessage.maxBitDepth		= maxBitDepth;
    mi.u.setFullScreenOptionsMessage.preferredBitDepth = preferredBitDepth;
    mi.u.setFullScreenOptionsMessage.desiredWidth		= desiredWidth;
    mi.u.setFullScreenOptionsMessage.desiredHeight		= desiredHeight;

    return ITCallApplication(appCookie, appProc, kPlayerSetFullScreenOptionsMessage, &mi);
}

// PlayerGetCurrentTrackCoverArt
//
OSStatus PlayerGetCurrentTrackCoverArt (void *appCookie, ITAppProcPtr appProc, Handle *coverArt, OSType *coverArtFormat)
{
    OSStatus			status;
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.getCurrentTrackCoverArtMessage.coverArt = nil;

    status = ITCallApplication(appCookie, appProc, kPlayerGetCurrentTrackCoverArtMessage, &mi);

    *coverArt = mi.u.getCurrentTrackCoverArtMessage.coverArt;
    if (coverArtFormat)
        *coverArtFormat = mi.u.getCurrentTrackCoverArtMessage.coverArtFormat;
    return status;
}

// PlayerGetPluginData
//
OSStatus PlayerGetPluginData (void *appCookie, ITAppProcPtr appProc, void *dataPtr, UInt32 dataBufferSize, UInt32 *dataSize)
{
    OSStatus			status;
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.getPluginDataMessage.dataPtr			= dataPtr;
    mi.u.getPluginDataMessage.dataBufferSize	= dataBufferSize;

    status = ITCallApplication(appCookie, appProc, kPlayerGetPluginDataMessage, &mi);

    if (dataSize != nil)
        *dataSize = mi.u.getPluginDataMessage.dataSize;

    return status;
}


// PlayerSetPluginData
//
OSStatus PlayerSetPluginData (void *appCookie, ITAppProcPtr appProc, void *dataPtr, UInt32 dataSize)
{
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.setPluginDataMessage.dataPtr	= dataPtr;
    mi.u.setPluginDataMessage.dataSize	= dataSize;

    return ITCallApplication(appCookie, appProc, kPlayerSetPluginDataMessage, &mi);
}


// PlayerGetPluginNamedData
//
OSStatus PlayerGetPluginNamedData (void *appCookie, ITAppProcPtr appProc, ConstStringPtr dataName, void *dataPtr, UInt32 dataBufferSize, UInt32 *dataSize)
{
    OSStatus			status;
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.getPluginNamedDataMessage.dataName		= dataName;
    mi.u.getPluginNamedDataMessage.dataPtr			= dataPtr;
    mi.u.getPluginNamedDataMessage.dataBufferSize	= dataBufferSize;

    status = ITCallApplication(appCookie, appProc, kPlayerGetPluginNamedDataMessage, &mi);

    if (dataSize != nil)
        *dataSize = mi.u.getPluginNamedDataMessage.dataSize;

    return status;
}


// PlayerSetPluginNamedData
//
OSStatus PlayerSetPluginNamedData (void *appCookie, ITAppProcPtr appProc, ConstStringPtr dataName, void *dataPtr, UInt32 dataSize)
{
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.setPluginNamedDataMessage.dataName	= dataName;
    mi.u.setPluginNamedDataMessage.dataPtr		= dataPtr;
    mi.u.setPluginNamedDataMessage.dataSize	= dataSize;

    return ITCallApplication(appCookie, appProc, kPlayerSetPluginNamedDataMessage, &mi);
}


// PlayerIdle
//
OSStatus PlayerIdle (void *appCookie, ITAppProcPtr appProc)
{
    return ITCallApplication(appCookie, appProc, kPlayerIdleMessage, nil);
}


// PlayerShowAbout
//
void PlayerShowAbout (void *appCookie, ITAppProcPtr appProc)
{
    ITCallApplication(appCookie, appProc, kPlayerShowAboutMessage, nil);
}


// PlayerOpenURL
//
void PlayerOpenURL (void *appCookie, ITAppProcPtr appProc, SInt8 *string, UInt32 length)
{
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.openURLMessage.url	= string;
    mi.u.openURLMessage.length	= length;

    ITCallApplication(appCookie, appProc, kPlayerOpenURLMessage, &mi);
}

// PlayerUnregisterPlugin
//
OSStatus PlayerUnregisterPlugin (void *appCookie, ITAppProcPtr appProc, PlayerMessageInfo *mi)
{
    return ITCallApplication(appCookie, appProc, kPlayerUnregisterPluginMessage, mi);
}


// PlayerRegisterVisualPlugin
//
OSStatus PlayerRegisterVisualPlugin (void *appCookie, ITAppProcPtr appProc, PlayerMessageInfo *mi)
{
    return ITCallApplicationInternal(appCookie, appProc, kPlayerRegisterVisualPluginMessage, kITVisualPluginMajorMessageVersion, kITVisualPluginMinorMessageVersion, mi);
}


// PlayerHandleMacOSEvent
//
OSStatus PlayerHandleMacOSEvent (void *appCookie, ITAppProcPtr appProc, const EventRecord *theEvent, Boolean *eventHandled)
{
    PlayerMessageInfo	mi;
    OSStatus			status;

    MyMemClear(&mi, sizeof(mi));

    mi.u.handleMacOSEventMessage.theEvent = theEvent;

    status = ITCallApplication(appCookie, appProc, kPlayerHandleMacOSEventMessage, &mi);

    if( eventHandled != nil )
        *eventHandled = mi.u.handleMacOSEventMessage.handled;

    return status;
}

// PlayerGetPluginFileSpec
//
#if TARGET_OS_MAC
OSStatus PlayerGetPluginFileSpec (void *appCookie, ITAppProcPtr appProc, FSSpec *pluginFileSpec)
{
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(messageInfo));

    messageInfo.u.getPluginFileSpecMessage.fileSpec = pluginFileSpec;

    return ITCallApplication(appCookie, appProc, kPlayerGetPluginFileSpecMessage, &messageInfo);
}
#endif	// TARGET_OS_MAC

// PlayerGetPluginITFileSpec
//
OSStatus PlayerGetPluginITFileSpec (void *appCookie, ITAppProcPtr appProc, ITFileSpec *pluginFileSpec)
{
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.getPluginITFileSpecMessage.fileSpec = pluginFileSpec;

    return ITCallApplication(appCookie, appProc, kPlayerGetPluginITFileSpecMessage, &mi);
}

// PlayerGetFileTrackInfo
//
OSStatus PlayerGetFileTrackInfo (void *appCookie, ITAppProcPtr appProc, const ITFileSpec *fileSpec, ITTrackInfo *trackInfo)
{
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.getFileTrackInfoMessage.fileSpec 	= fileSpec;
    mi.u.getFileTrackInfoMessage.trackInfo = trackInfo;

    return ITCallApplication(appCookie, appProc, kPlayerGetFileTrackInfoMessage, &mi);
}

// PlayerSetFileTrackInfo
//
OSStatus PlayerSetFileTrackInfo (void *appCookie, ITAppProcPtr appProc, const ITFileSpec *fileSpec, const ITTrackInfo *trackInfo)
{
    PlayerMessageInfo	mi;

    MyMemClear(&mi, sizeof(mi));

    mi.u.setFileTrackInfoMessage.fileSpec 	= fileSpec;
    mi.u.setFileTrackInfoMessage.trackInfo = trackInfo;

    return ITCallApplication(appCookie, appProc, kPlayerSetFileTrackInfoMessage, &mi);
}

// PlayerGetITTrackInfoSize
//
OSStatus PlayerGetITTrackInfoSize (void *appCookie, ITAppProcPtr appProc, UInt32 appPluginMajorVersion, UInt32 appPluginMinorVersion, UInt32 *itTrackInfoSize)
{
    PlayerMessageInfo	mi;
    OSStatus			status;

    /*
    Note: appPluginMajorVersion and appPluginMinorVersion are the versions given to the plugin by iTunes in the plugin's init message.
    These versions are *not* the version of the API used when the plugin was compiled.
    */

    *itTrackInfoSize = 0;

    MyMemClear(&mi, sizeof(mi));

    status = ITCallApplication(appCookie, appProc, kPlayerGetITTrackInfoSizeMessage, &mi);
    if( status == noErr )
    {
        *itTrackInfoSize = mi.u.getITTrackInfoSizeMessage.itTrackInfoSize;
    }
    else if( appPluginMajorVersion == 10 && appPluginMinorVersion == 2 )
    {
        // iTunes 2.0.x

        *itTrackInfoSize = ((UInt32) &((ITTrackInfo *) 0)->composer);

        status = noErr;
    }
    else if( appPluginMajorVersion == 10 && appPluginMinorVersion == 3 )
    {
        // iTunes 3.0.x

        *itTrackInfoSize = ((UInt32) &((ITTrackInfo *) 0)->beatsPerMinute);

        status = noErr;
    }
    else
    {
        // iTunes 4.0 and later implement the kPlayerGetITTrackInfoSizeMessage message. If you got here
        // then the appPluginMajorVersion or appPluginMinorVersion are incorrect.

        status = paramErr;
    }

    if( status == noErr && (*itTrackInfoSize) > sizeof(ITTrackInfo) )
    {
        // iTunes is using a larger ITTrackInfo than the one when this plugin was compiled. Pin *itTrackInfoSize to the plugin's known size

        *itTrackInfoSize = sizeof(ITTrackInfo);
    }

    return status;
}