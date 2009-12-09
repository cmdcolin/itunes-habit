#include "iTunesPluginUtils.hpp"
#include "dbgUtils.hpp"


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

bool normalizeCurrentDirectory()
{
    std::vector<TCHAR> v(256);
    DWORD ret = 0;
    
    ret = ::GetModuleFileName(0, &v[0], static_cast<DWORD>(v.size()));

    if(ret == 0)
    {
        BOOST_LOGL(app, info) << "Failed at GetModuleFileName";
        return false;
    }

    std::basic_string<TCHAR> s(v.begin(), v.end());

    ret = ::SetCurrentDirectory(s.substr(0, s.rfind('\\')).c_str());

    if(ret == 0)
    {
        BOOST_LOGL(app, info) << "Failed at SetCurrentDirectory";
        return false;
    }

    ret = ::GetCurrentDirectory(static_cast<DWORD>(v.size()), &v.at(0));

    if(ret == 0)
    {
        BOOST_LOGL(app, info) << "Failed at GetCurrentDirectory";
        return false;
    }

    return true;
}


BOOL WINAPI DllMain(
    HINSTANCE /* instance */,
    DWORD reason,
    LPVOID /* reserved */
    )
{
	return TRUE;
}