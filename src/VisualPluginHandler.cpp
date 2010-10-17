
#include "iTunesVisualAPI.h"
#include "iTunesPluginUtils.h"
#include "VisualPluginData.h"
#include "guicon.h"


void InitPlugin(VisualPluginMessageInfo * messageInfo, VisualPluginData *vpd)
{
    RedirectIOToConsole();
    cout << __FUNCTION__ << ": Init\n";

    vpd = new VisualPluginData;

    vpd->appCookie	= messageInfo->u.initMessage.appCookie;
    vpd->appProc	= messageInfo->u.initMessage.appProc;

    messageInfo->u.initMessage.refCon	= (void*) vpd;
}


void SongPlay(VisualPluginMessageInfo * messageInfo, VisualPluginData *vpd)
{
    cout << __FUNCTION__ << ": Play Song\n";

    vpd->trackInfo = *messageInfo->u.playMessage.trackInfo;
    vpd->streamInfo = *messageInfo->u.playMessage.streamInfo;

    vpd->trackUniInfo = *messageInfo->u.playMessage.trackInfoUnicode;
    vpd->streamUniInfo = *messageInfo->u.playMessage.streamInfoUnicode;

    vpd->playing = true;

    vpd->log.log(vpd->trackInfo);
    vpd->log.lastPlayedSongs(5, vector<string>());
    // vpd->loggy->log(vpd->trackUniInfo);
    // vpd->loggy->serialize(vpd->music_log_file);
}

void SongChange(VisualPluginMessageInfo * messageInfo, VisualPluginData *vpd)
{
    cout << __FUNCTION__ << ": Change Track\n";

    vpd->trackInfo = *messageInfo->u.changeTrackMessage.trackInfo;
    vpd->streamInfo = *messageInfo->u.changeTrackMessage.streamInfo;

    vpd->trackUniInfo = *messageInfo->u.changeTrackMessage.trackInfoUnicode;
    vpd->streamUniInfo = *messageInfo->u.changeTrackMessage.streamInfoUnicode;

    vpd->playing = true;

    vpd->log.log(vpd->trackInfo);
    vpd->log.lastPlayedSongs(5, vector<string>());
    // vpd->loggy->log(vpd->trackUniInfo);
    // vpd->loggy->serialize(vpd->music_log_file);
}
/*
VisualPluginHandler
*/

OSStatus VisualPluginHandler(OSType message, 
                                    VisualPluginMessageInfo * messageInfo, 
                                    void * refCon)
{
    OSStatus status;

	VisualPluginData *	vpd;
	vpd = (VisualPluginData*) refCon;

    status = noErr;

    switch (message)
    {    

        /*
        Sent when the visual plugin is registered.  The plugin should do minimal
        memory allocations here.  The resource fork of the plugin is still available.
        */		
    case kVisualPluginInitMessage:
        {
            InitPlugin(messageInfo, vpd);
        }
        break;

        /*
        Sent when the visual plugin is unloaded
        */		
    case kVisualPluginCleanupMessage:
        {
            cout << __FUNCTION__ << ": Cleanup\n";

			delete vpd;
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
            SongPlay(messageInfo, vpd);
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
            SongChange(messageInfo, vpd);
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


