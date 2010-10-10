#pragma once

#include <xercesc/dom/DOMException.hpp>
#include <curl/curl.h>

#include "iTunesAPI.h"
#include "iTunesVisualAPI.h"

#include "xmlLog.hpp"
#include "httpUploader.hpp"

#include <ctime>
#include <string>
#include <vector>
#include <iostream>

using namespace std;


struct VisualPluginDataz
{
    VisualPluginDataz(
        const basic_string<TCHAR> & mlog,
        const basic_string<TCHAR> & imgfike,
        const basic_string<TCHAR> & grammar,
        const basic_string<TCHAR> & elog
        ) :

    music_log_file(mlog), error_log_file(elog),
        img_file(imgfike), grammar_file(grammar)

    {
        cout << __FUNCTION__ << "\n";

        try
        {
            HANDLE file = ::CreateFile(mlog.c_str(), GENERIC_READ,
                FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

            if(file == INVALID_HANDLE_VALUE &&
                ::GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                loggy = new Log(grammar_file.c_str(), false);
            }
            else
            {
                ::CloseHandle(file);

                loggy = new Log(music_log_file.c_str(), true);
            }
        }
        catch(XMLException & e)
        {
            cout << SX(e.getMessage()) << "\n";
        }
        catch(DOMException & e)
        {
            cout << SX(e.getMessage()) << "\n";
        }
    }

    ~VisualPluginDataz()
    {
        cout << __FUNCTION__ << "\n";

        delete loggy;
    }

    void *			cookie;
    ITAppProcPtr	proc;

    ITTrackInfoV1	trackInfo;
    ITStreamInfoV1	streamInfo;

    ITTrackInfo     trackUniInfo;
    ITStreamInfo    streamUniInfo;

    Boolean	playing;

    Log *     loggy;
    Uploader  up;

    basic_string<TCHAR> host;
    basic_string<TCHAR> music_log_file;
    basic_string<TCHAR> error_log_file;
    basic_string<TCHAR> img_file;
    basic_string<TCHAR> grammar_file;

};

//
// Deprecated - Functor converts type U to type vector of T's. Work on this
//

template <typename T>
struct
    converter
{
    vector<T> r;

    template <typename U>
    void operator () (U u)
    {
        r.push_back(static_cast<T>(u));
    }

    T * str(void)
    {
        if(r.back() != 0)
            r.push_back(0);

        return &r.at(0);
    }
};

unsigned int seed();
bool NormalizeCurrentDirectory();

static OSStatus VisualPluginHandler(
                                    OSType message,
                                    VisualPluginMessageInfo * messageInfo,
                                    void * refCon
                                    );



DWORD WINAPI ThreadProc(LPVOID t);



BOOL CALLBACK DlgProc(
                      HWND dlg,
                      UINT message,
                      WPARAM wParam,
                      LPARAM lParam
                      );

BOOL WINAPI DllMain(
                    HINSTANCE /* instance */,
                    DWORD reason,
                    LPVOID /* reserved */
                    );


extern "C" __declspec(dllexport) OSStatus iTunesPluginMain(
    OSType message,
    PluginMessageInfo * messageInfo,
    void * /* refCon */
    );
