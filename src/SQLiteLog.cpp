#include "SQLiteLog.h"
#include "iTunesVisualAPI.h"



CSQLiteLog::CSQLiteLog(const string &filename)
{
    cout << __FUNCTION__ << "\n";

    if(sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
        sqlite3_close(db);
        cout << "sqlite3_open" << " " << sqlite3_errmsg(db) << "\n";
    }

    sqlite3_stmt *stmt;
    const char *sz = "create table t1 (t1key INTEGER PRIMARY KEY,d1 TEXT,d2 TEXT,d3 TEXT);";
    const char *err;

    sqlite3_prepare(db, sz, strlen(sz), &stmt, &err);
    sqlite3_step(stmt);
}


CSQLiteLog::~CSQLiteLog()
{
    cout << __FUNCTION__ << "\n";
    sqlite3_close(db);
}

void CSQLiteLog::serialize(const string& s) const
{
    cout << __FUNCTION__ << "\n";
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



