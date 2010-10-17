#pragma once

#include <xercesc/dom/DOMException.hpp>
#include <curl/curl.h>

#include "iTunesAPI.h"
#include "iTunesVisualAPI.h"

#include "xmlLog.hpp"
#include "sqlite/sqlite3.h"


#include <ctime>
#include <string>
#include <vector>
#include <iostream>

using namespace std;


class CVisualPlugin {
public:

    CVisualPlugin(const string & database) : m_dbfilename(database)
    {
        cout << __FUNCTION__ << "\n";
    }

    ~CVisualPlugin()
    {
        cout << __FUNCTION__ << "\n";
    }

    ITAppProcPtr	proc;
    ITTrackInfoV1	trackInfo;
    ITStreamInfoV1	streamInfo;
    ITTrackInfo     trackUniInfo;
    ITStreamInfo    streamUniInfo;

    void *			cookie;
    Boolean	        playing;
    string          m_dbfilename;
    sqlite3 *       db;

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

static OSStatus VisualPluginHandler(OSType message,
                                    VisualPluginMessageInfo * messageInfo,
                                    void * refCon);


BOOL CALLBACK DlgProc(HWND dlg,
                      UINT message,
                      WPARAM wParam,
                      LPARAM lParam);

BOOL WINAPI DllMain(HINSTANCE /* instance */,
                    DWORD reason,
                    LPVOID /* reserved */);


extern "C" __declspec(dllexport) OSStatus iTunesPluginMain(
    OSType message,
    PluginMessageInfo * messageInfo,
    void * /* refCon */);
