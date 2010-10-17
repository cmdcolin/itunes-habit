#include "xmlLog.hpp"
#include "iTunesVisualAPI.h"



CSQLiteLog::CSQLiteLog(sqlite3 * ptr)
{
    db = ptr;
    cout << __FUNCTION__ << "\n";
}


CSQLiteLog::~CSQLiteLog(void)
{
    cout << __FUNCTION__ << "\n";
}

void CSQLiteLog::serialize(const string& s) const
{
    cout << __FUNCTION__ << "\n";



    //DOMImplementationLS * impl = (DOMImplementationLS *)
    //    DOMImplementationRegistry::getDOMImplementation(XS("LS"));

    //DOMLSSerializer* writer = impl->createLSSerializer();

    //DOMConfiguration* dc = writer->getDomConfig();


    //writer->writeToString(m_doc, 0);
    ////writer->writeNode(ft, *m_doc);
    //writer->release();

    //delete ft;
}

void CSQLiteLog::log(ITTrackInfoV1 & ti)
{
    cout << __FUNCTION__ << "\n";

    SYSTEMTIME t;
    GetLocalTime(&t);

    stringstream ss;
    ss << t.wYear << L":" << t.wMonth << L":" << t.wDay << L":"
        << t.wDayOfWeek << L":" << t.wHour << L":" << t.wMinute
        << L":" << t.wSecond << L":" << t.wMilliseconds;

    string artist = (char*) ti.artist + 1;
    string album = (char*) ti.album + 1;
    string song = (char*) ti.name + 1;
    string time = ss.str();


    sqlite3_stmt *stmt;
    const char *err;
    char *sz = "insert into t1 VALUES(%s, %s, %s)";
    sqlite3_prepare(db, sz, strlen(sz), &stmt, &err);
    sqlite3_step(stmt);
    //m_doc->getDocumentElement()->appendChild(song);
}

void CSQLiteLog::log(ITTrackInfo & ti)
{
    cout << __FUNCTION__ << "\n";

    SYSTEMTIME t;
    GetLocalTime(&t);
    wstringstream wss;
    wss << t.wYear << L" " << t.wMonth << L" " << t.wDay << L" "
        << t.wDayOfWeek << L" " << t.wHour << L" " << t.wMinute
        << L" " << t.wSecond << L" " << t.wMilliseconds;

    wstring artist = (wchar_t *) ti.artist + 1;
    wstring album = (wchar_t *) ti.album + 1;
    wstring song = (wchar_t *) ti.name + 1;
    wstring time = wss.str();

    //m_doc->getDocumentElement()->appendChild(song);
}




int CSQLiteLog::lastPlayedSongs(unsigned int n, vector<string> &v) const
{
    cout << __FUNCTION__ << "\n";

    const char *sz = "select * from tbl";
    const char *err;
    sqlite3_stmt  *stmt;
    sqlite3_prepare(db, sz, strlen(sz), &stmt, &err);

    for(int i = 0; i < n; i++) {
        sqlite3_step(stmt);
    }

    return 0;
}



