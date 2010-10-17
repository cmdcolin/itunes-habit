#include "SQLiteLog.h"
#include "iTunesVisualAPI.h"



CSQLiteLog::CSQLiteLog(const string &filename)
{
    cout << __FUNCTION__ << "\n";

    if(sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
        sqlite3_close(db);
        cout << "sqlite3_open" << " " << sqlite3_errmsg(db) << "\n";
    }

    cout << "Database opened\n";
}


CSQLiteLog::~CSQLiteLog()
{
    cout << __FUNCTION__ << "\n";
    sqlite3_close(db);
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
    



    const char *sz = "INSERT INTO t1 VALUES(%d,%Q,%Q,%Q)";
    const char *err;
    sqlite3_stmt *stmt;
    char * jz = sqlite3_mprintf(sz, 6, artist.c_str(), song.c_str(), album.c_str());
    cout << "here " << jz << "\n";

    sqlite3_prepare(db, jz, strlen(sz), &stmt, &err);
    sqlite3_step(stmt);
    sqlite3_free(jz);
    sqlite3_finalize(stmt);

    cout << "here 2 " << err << "\n";
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

    const char *sz = "select * from t1";
    const char *err;
    sqlite3_stmt  *stmt;
    sqlite3_prepare(db, sz, strlen(sz), &stmt, &err);

    for(int i = 0; i < n; i++) {
        sqlite3_step(stmt);
        cout << "here";
    }

    return 0;
}



