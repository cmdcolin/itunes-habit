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


#include "iTunesVisualAPI.h"
#include "iTunesPluginUtils.h"
#include "VisualPluginData.h"
#include "guicon.h"




//########################################
//	local ( static ) globals
//########################################

static Boolean	gColorFlag = TRUE;


//########################################
//	exported function prototypes
//########################################


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

static void RenderVisualPort(VisualPluginData *vpd, GRAPHICS_DEVICE destPort,const Rect *destRect,Boolean onlyUpdate)
{

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
            hBrush = CreateSolidBrush(RGB((UInt16)vpd->maxLevel[1]<<1, (UInt16)vpd->maxLevel[1]<<1, (UInt16)vpd->maxLevel[0]<<1));
        else
            hBrush = CreateSolidBrush(RGB((UInt16)vpd->maxLevel[1]<<1, (UInt16)vpd->maxLevel[1]<<1, (UInt16)vpd->maxLevel[1]<<1));
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