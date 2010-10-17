#include "iTunesPluginUtils.h"
#include <strsafe.h>

unsigned int seed()
{
    time_t now = std::time(0);

    unsigned char * p = reinterpret_cast<unsigned char *>(&now);
    unsigned int seed = 0;

    for(unsigned int i = 0; i < sizeof(now); ++i)
        seed = seed * (UCHAR_MAX + 2U) + p[i];

    return seed;
}

// Returns false on failure
// More info in GetLastError

bool NormalizeCurrentDirectory()
{
    TCHAR buffer[MAX_PATH];
    TCHAR substr[MAX_PATH];
    long ret = 0;

    if(!GetModuleFileName(0, buffer, MAX_PATH))
    {
        cout << "Failed at GetModuleFileName" << "\n";
        return false;
    }

    // reverse find backslash
#ifndef UNICODE
    ret = strrchr(buffer, '\\') - buffer;
#else
    ret = wcsrchr(buffer, L'\\') - buffer;
#endif


    // copy up to last backslash
    StringCchCopy(substr, ret, buffer);
    substr[ret] = TEXT('\0');

    if(!SetCurrentDirectory(substr))
    {
        cout << "Failed at SetCurrentDirectory" << "\n";
        return false;
    }

    return true;
}


BOOL WINAPI DllMain(HINSTANCE /* instance */,
                    DWORD /* reason */,
                    LPVOID /* reserved */)
{
    return TRUE;
}