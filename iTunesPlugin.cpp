#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include "iTunesVisualAPI.hpp"
#include "iTunesPluginUtils.hpp"

#include "foobar2000.h"
#include "component.h"

#include "dbgUtils.hpp"
#include "gdUtils.hpp"
#include "xmlLog.hpp"
#include "httpUploader.hpp"

#include "Resource.k"

#define _WIN32_IE 0x301

#include <tchar.h>
#include <commctrl.h>

#define kPluginName		"Siggen"
#define	kPluginCreator	'Tnto'

#define	kPluginMajorVersion		1
#define	kPluginMinorVersion		0
#define	kPluginReleaseStage		0x80
#define	kPluginNonFinalRelease	0


LRESULT CALLBACK WndProc(
                         HWND wnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam
                         )
{
    BOOST_LOGL(app, info) << __FUNCTION__;

    switch(message)
    {

    case WM_CREATE:
        {
        }
        break;

    case WM_DESTROY:
        {
            ::PostQuitMessage(0);
        }
        break;

    case 0x4337:
        {
            ::DestroyWindow(wnd);
        }
        break;

    default:
        {
            return ::DefWindowProc(wnd, message, wParam, lParam);
        }
        break;
    }

    return 0;
}



INT WINAPI SheetProc(HWND UNUSED(dlg), UINT message, LPARAM lParam)
{
    switch(message)
    {
    case PSCB_PRECREATE:
        {
            LPDLGTEMPLATE  lpTemplate = (LPDLGTEMPLATE)lParam;

            if(!(lpTemplate->style & WS_SYSMENU))
            {
                lpTemplate->style |= WS_SYSMENU;
            }
        }
        break;

    case PSCB_INITIALIZED:
        break;

    }

    return 1;
}



BOOL CALLBACK DlgSubOne(HWND dlg, UINT message, WPARAM UNUSED(wParam), LPARAM lParam)
{
    switch(message)
    {

    case WM_COMMAND:

        PropSheet_Changed(GetParent(dlg), dlg);
        break;

    case WM_NOTIFY:

        switch(LPNMHDR(lParam)->code)
        {
        default:
            break;
        }
        break;

    default:
        break;
    }

    return FALSE;
}

BOOL CALLBACK DlgSubTwo(HWND dlg, UINT message, WPARAM UNUSED(wParam), LPARAM lParam)
{
    switch(message)
    {

    case WM_COMMAND:

        PropSheet_Changed(GetParent(dlg), dlg);
        break;

    case WM_NOTIFY:

        switch(LPNMHDR(lParam)->code)
        {
        default:
            break;
        }
        break;

    default:
        break;
    }

    return FALSE;
}




BOOL CALLBACK DlgSubThree(HWND dlg, UINT message, WPARAM UNUSED(wParam), LPARAM lParam)
{
    switch(message)
    {

    case WM_COMMAND:

        PropSheet_Changed(GetParent(dlg), dlg);
        break;

    case WM_NOTIFY:

        switch(LPNMHDR(lParam)->code)
        {
        default:
            break;
        }
        break;

    default:
        break;
    }

    return FALSE;
}


DWORD WINAPI ThreadProc(LPVOID t)
{
    BOOST_LOGL(app, info) << __FUNCTION__;


    PROPSHEETHEADER	m_PropSheet = {0};
    PROPSHEETPAGE m_psp[3] = {0};

    m_psp[0].dwSize     = sizeof(PROPSHEETPAGE);
    m_psp[0].dwFlags    = PSP_USETITLE;
    m_psp[0].hInstance  = ::GetModuleHandle(_T("Itunesplugin.dll"));
    m_psp[0].pszTemplate = MAKEINTRESOURCE(IDD_DIALOG_SUBONE);
    m_psp[0].pszTitle   = _T("File Settings");
    m_psp[1].pfnDlgProc = &DlgSubOne;

    m_psp[1].dwSize     = sizeof(PROPSHEETPAGE);
    m_psp[1].dwFlags    = PSP_USETITLE;
    m_psp[1].hInstance  = ::GetModuleHandle(_T("Itunesplugin.dll"));
    m_psp[1].pszTemplate = MAKEINTRESOURCE(IDD_DIALOG_SUBTWO);
    m_psp[1].pszTitle   = _T("Internet Settings");
    m_psp[1].pfnDlgProc = &DlgSubTwo;

    m_psp[2].dwSize     = sizeof(PROPSHEETPAGE);
    m_psp[2].dwFlags    = PSP_USETITLE;
    m_psp[2].hInstance  = ::GetModuleHandle(_T("Itunesplugin.dll"));
    m_psp[2].pszTemplate = MAKEINTRESOURCE(IDD_DIALOG_SUBTHREE);
    m_psp[2].pszTitle   = _T("Image Settings");
    m_psp[2].pfnDlgProc = &DlgSubThree;

    m_PropSheet.dwSize      = sizeof(PROPSHEETHEADER);
    m_PropSheet.dwFlags     = PSH_PROPSHEETPAGE | PSH_USECALLBACK | PSH_MODELESS;
    m_PropSheet.hInstance   = ::GetModuleHandle(_T("Itunesplugin.dll"));
    m_PropSheet.pszCaption  = _T("Cell Properties");
    m_PropSheet.nPages      = sizeof(m_psp) / sizeof(m_psp[0]);
    m_PropSheet.nStartPage  = 0;
    m_PropSheet.ppsp        = m_psp;
    m_PropSheet.pfnCallback = &SheetProc;

    HWND properties = reinterpret_cast<HWND>(PropertySheet(&m_PropSheet));

    BOOST_LOG(app) << properties;

    //::SetWindowLongPtr(wnd, GWLP_USERDATA, LONG_PTR(t));
    //::ShowWindow(wnd, SW_SHOW);
    //((VisualPluginData *) t)->dialog = wnd;

    MSG msg;

    while(::GetMessage(&msg, 0, 0, 0) > 0)
    {
        if(!::SendMessage(properties, PSM_ISDIALOGMESSAGE, 0, LPARAM(&msg)))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }

        if(properties && !::SendMessage(properties, PSM_GETCURRENTPAGEHWND, 0, 0))
        {
            BOOST_LOG(app) << "Destroyableed";

            ::DestroyWindow(properties);
            properties = 0;
        }
    }

    VisualPluginHandler(0x4337, 0, t);

    return 0;
}


static OSStatus VisualPluginHandler(
                                    OSType message,
                                    VisualPluginMessageInfo * messageInfo,
                                    void * refCon
                                    )
{
    OSStatus            status  = noErr;
    VisualPluginData *  vpd     = static_cast<VisualPluginData *>(refCon);


    switch (message)
    {

    case 0x4337:
        {
            BOOST_LOGL(app, info) << __FUNCTION__
                << ": Killin' Threadz";

            CloseHandle(vpd->p);
        }
        break;

    case kVisualPluginSetWindowMessage:
    case kVisualPluginUpdateMessage:
    case kVisualPluginEnableMessage:
    case kVisualPluginDisableMessage:
    case kVisualPluginIdleMessage:
    case kVisualPluginSetPositionMessage:
        break;

    case kVisualPluginShowWindowMessage:
        {
            BOOST_LOGL(app, info) << __FUNCTION__
                << ": Show Window";


            memcpy(&vpd->r, &messageInfo->u.showWindowMessage.drawRect, sizeof(Rect));
            vpd->itunes = messageInfo->u.showWindowMessage.window;

            /*

            HWND dlg = CreateDialogParam(
            GetModuleHandle(TEXT("Itunesplugin.dll")),
            MAKEINTRESOURCE(IDD_DIALOG),
            0, &DlgProc, LPARAM(vpd));

            */
        }
        break;

    case kVisualPluginHideWindowMessage:
        {
            BOOST_LOGL(app, info) << __FUNCTION__
                << ": Hide Window";

            memset(&vpd->r, 0, sizeof(Rect));
            vpd->itunes = 0;
        }
        break;

    case kVisualPluginRenderMessage:
        {
            HDC dc = ::GetDC(vpd->itunes);
            RECT r;

            r.bottom = vpd->r.bottom;
            r.left = vpd->r.left;
            r.right = vpd->r.right;
            r.top = vpd->r.top;

            ::FillRect(dc, &r, (HBRUSH) ::GetStockObject(DKGRAY_BRUSH));
            ::ReleaseDC(vpd->itunes, dc);
        }
        break;

    case kVisualPluginConfigureMessage:
        {
            BOOST_LOGL(app, info) << __FUNCTION__
                << ": Configure";

            vpd->p = CreateThread(0, 0, &ThreadProc, LPVOID(vpd), 0, &vpd->tid);
        }
        break;

    case kVisualPluginInitMessage:
        {
            BOOST_LOGL(app, info) << __FUNCTION__
                << ": Initialization";

            INITCOMMONCONTROLSEX icx;

            icx.dwSize  = sizeof(INITCOMMONCONTROLSEX);
            icx.dwICC   = ICC_TAB_CLASSES;

            InitCommonControlsEx(&icx);

            try
            {
                XMLPlatformUtils::Initialize();
            }

            catch(XMLException & e)
            {
                ::MessageBox(HWND_DESKTOP, TEXT("Xerces XML API failed to initialize"),
                    TEXT("Fatal Error"), MB_OK | MB_ICONEXCLAMATION);

                BOOST_LOGL(app, fatal) << __FUNCTION__
                    << ": Xerces Initialization Error - " << SX(e.getMessage());

                status = unimpErr;
            }

            vpd = new VisualPluginData(TEXT("C:/test.xml"), TEXT("C:/img.jpg"), TEXT("C:/log.dtd"), TEXT("C:/errors.log"));

            vpd->cookie	= messageInfo->u.initMessage.appCookie;
            vpd->proc = messageInfo->u.initMessage.appProc;

            vpd->up.setScriptBackend("http://coldie.freehostia.com/upload.php");
            vpd->up.setFormElement("log");
            vpd->up.setFormElementName("name");

            messageInfo->u.initMessage.refCon = vpd;
        }
        break;


    case kVisualPluginCleanupMessage:
        {
            BOOST_LOGL(app, info) << __FUNCTION__
                << ": Cleanup";

            delete vpd;

            XMLPlatformUtils::Terminate();
        }
        break;



    case kVisualPluginChangeTrackMessage:
        {
            BOOST_LOGL(app, info) << __FUNCTION__ << ": Change Track";

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
                    vpd->img_file, 
                    "C:/the_King__26_Queen_font.ttf",
                    18);

                #if defined(_UNICODE)
                    if(!vpd->up.uploadFile(SX(vpd->img_file.c_str())))
#else
                if(!vpd->up.uploadFile(vpd->img_file.c_str()))
#endif
                {
                    BOOST_LOGL(app, err) << "Upload failed";
                }

                vpd->loggy->serialize(vpd->music_log_file);
            }
            catch(DOMException &)
            {
                BOOST_LOGL(app, info) << "DOM Exception";
            }
        }
        break;

    case kVisualPluginPlayMessage:
        {
            BOOST_LOGL(app, info) << __FUNCTION__ << ": Play Song";

            vpd->trackInfo = *messageInfo->u.playMessage.trackInfo;
            vpd->streamInfo = *messageInfo->u.playMessage.streamInfo;

            vpd->trackUniInfo = *messageInfo->u.playMessage.trackInfoUnicode;
            vpd->streamUniInfo = *messageInfo->u.playMessage.streamInfoUnicode;

            vpd->playing = true;

            try
            {
                vpd->loggy->log(vpd->trackUniInfo);

                vpd->ic.createLastPlayedChart(
                    vpd->loggy->lastPlayedSongs(1), 
                    vpd->img_file, 
                    "C:/the_King__26_Queen_font.ttf",
                    18);

                #if defined(_UNICODE)
                    if(!vpd->up.uploadFile(SX(vpd->img_file.c_str())))
                #else
                    if(!vpd->up.uploadFile(vpd->img_file.c_str()))
                #endif
                {
                    BOOST_LOGL(app, err) << "Upload failed";
                }

                vpd->loggy->serialize(vpd->music_log_file);
            }
            catch(DOMException & e)
            {
                BOOST_LOGL(app, info) << "DOM Exception: " << e.getMessage();
            }
        }
        break;


    case kVisualPluginUnpauseMessage:
        {
            vpd->playing = true;
        }
        break;


    default:
        {
            status = unimpErr;
        }
        break;
    }

    return status;
}


//
// Registration of plug-in
//

static OSStatus RegisterPlugin(
                               PluginMessageInfo * messageInfo
                               )
{
    BOOST_LOGL(app, info) << __FUNCTION__;

    PlayerMessageInfo playerMessageInfo = {0};

    //
    // Set plug-in version information
    //

    SetNumVersion(&playerMessageInfo.u.registerVisualPluginMessage.pluginVersion,
        kPluginMajorVersion, kPluginMinorVersion,
        kPluginReleaseStage, kPluginNonFinalRelease);


    //
    // Set plug-in configuration
    //

    playerMessageInfo.u.registerVisualPluginMessage.options
        = kVisualWantsIdleMessages | kVisualWantsConfigure; // Notifcations
    playerMessageInfo.u.registerVisualPluginMessage.handler
        = &VisualPluginHandler; // Message handler callback
    playerMessageInfo.u.registerVisualPluginMessage.creator
        = kPluginCreator;

    playerMessageInfo.u.registerVisualPluginMessage.timeBetweenDataInMS		= 0xFFFFFFFF;
    playerMessageInfo.u.registerVisualPluginMessage.numWaveformChannels		= 2;
    playerMessageInfo.u.registerVisualPluginMessage.numSpectrumChannels		= 2;

    playerMessageInfo.u.registerVisualPluginMessage.minWidth				= 64;
    playerMessageInfo.u.registerVisualPluginMessage.minHeight				= 64;
    playerMessageInfo.u.registerVisualPluginMessage.maxWidth				= 32767;
    playerMessageInfo.u.registerVisualPluginMessage.maxHeight				= 32767;

    playerMessageInfo.u.registerVisualPluginMessage.minFullScreenBitDepth	= 0;
    playerMessageInfo.u.registerVisualPluginMessage.maxFullScreenBitDepth	= 0;
    playerMessageInfo.u.registerVisualPluginMessage.windowAlignmentInBytes	= 0;
    playerMessageInfo.u.registerVisualPluginMessage.registerRefCon	        = 0;


    //
    // Plug-in name (length prepended)
    //

    playerMessageInfo.u.registerVisualPluginMessage.name[0] =
        static_cast<UInt8>(strlen(kPluginName));

    std::copy(kPluginName, kPluginName + strlen(kPluginName),
        &playerMessageInfo.u.registerVisualPluginMessage.name[1]);

    //
    // Register it
    //

    return PlayerRegisterVisualPlugin(messageInfo->u.initMessage.appCookie,
        messageInfo->u.initMessage.appProc, &playerMessageInfo);
}


extern "C" __declspec(dllexport) BOOL WINAPI DllMain(
    HINSTANCE /* instance */,
    DWORD reason,
    LPVOID /* reserved */
    )
{
    switch(reason)
    {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}


//
// Entry point for the app
//



static HINSTANCE g_hIns;

static pfc::string_simple g_name, g_full_path;

static bool g_services_available = false, g_initialized = false;

static foobar2000_api * g_api;

namespace core_api
{

	HINSTANCE get_my_instance()
	{
		return g_hIns;
	}

	HWND get_main_window()
	{
		return g_api->get_main_window();
	}

	pcchar get_my_file_name()
	{
		return g_name;
	}

	pcchar get_my_full_path()
	{
		return g_full_path;
	}

	bool are_services_available()
	{
		return g_services_available;
	}
	bool assert_main_thread()
	{
		return (g_services_available && g_api) ? 
            g_api->assert_main_thread() : true;
	}

	void ensure_main_thread() 
    {
		if (!assert_main_thread()) 
            throw exception_wrong_thread();
	}

	bool is_main_thread()
	{
		return (g_services_available && g_api) ? 
            g_api->is_main_thread() : true;
	}

	pcchar get_profile_path()
	{
		return (g_services_available && g_api) ? 
            g_api->get_profile_path() : 0;
	}

	bool is_shutting_down()
	{
		return (g_services_available && g_api) ? 
            g_api->is_shutting_down() : g_initialized;
	}
	bool is_initializing()
	{
		return (g_services_available && g_api) ? 
            g_api->is_initializing() : !g_initialized;
	}
}

namespace 
{
	class foobar2000_client_impl : 
        public foobar2000_client
	{

	public:

		t_uint32 get_version() 
        {
            return FOOBAR2000_CLIENT_VERSION;
        }

		pservice_factory_base get_service_list() 
        {
            return service_factory_base::__internal__list;
        }

		void get_config(stream_writer * p_stream, abort_callback & p_abort) 
        {
			cfg_var::config_write_file(p_stream, p_abort);
		}

		void set_config(stream_reader * p_stream, abort_callback & p_abort) 
        {
			cfg_var::config_read_file(p_stream, p_abort);
		}

		void set_library_path(const char * path, const char * name) 
        {
			g_full_path = path;
			g_name = name;
		}

		void services_init(bool val) 
        {
            ::MessageBox(0, L"h", 0, 0);

			if (val) 
                g_initialized = true;

			g_services_available = val;
		}

		bool is_debug() 
        {
#ifdef _DEBUG
			return true;
#else
			return false;
#endif
		}
	};
}

static foobar2000_client_impl g_client;

extern "C"
{
	__declspec(dllexport) foobar2000_client * _cdecl foobar2000_get_interface(foobar2000_api * p_api, HINSTANCE hIns)
	{
            ::MessageBox(0, L"h", 0, 0);

		g_hIns = hIns;
		g_api = p_api;

		return &g_client;
	}
}



extern "C"
{
    __declspec(dllexport) OSStatus iTunesPluginMain
        (
        OSType message,
        PluginMessageInfo * messageInfo,
        void * /* refCon */
        )
    {
        OSStatus status = noErr;

        switch (message)
        {

        case kPluginluginInitMessage:
            {
                using namespace boost::logging;

                flush_log_cache();
                add_modifier(_T("*"), prepend_time(_T("$hh:$mm:$ss ")), _T(""), INT_MAX);
                add_modifier(_T("*"), append_enter);
                add_appender(_T("*"), write_to_dbg_wnd);
                manipulate_logs(_T("*")).enable(level::enable_all);

                normalizeCurrentDirectory();
                srand(seed());

                BOOST_LOGL(app, info) << __FUNCTION__ << ": kPlugluginInitMessage";
                status = RegisterPlugin(messageInfo);
            }
            break;

        case kPluginluginCleanupMessage:
            {
                BOOST_LOGL(app, info) << __FUNCTION__ << ": kPluginluginCleanupMessage";
                status = noErr;
            }
            break;

        case kPluginlayerRegisterVisualPluginMessage:
            {
                BOOST_LOGL(app, info) << __FUNCTION__ << ": kPluginlayerRegisterVisualPluginMessage";
                status = noErr;
            }
            break;

        default:
            {
                status = unimpErr;
            }
            break;

        }

        return status;
    }
}
