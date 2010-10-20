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
    


    char *zSQL = sqlite3_mprintf("INSERT INTO t1 VALUES(NULL,%Q,%Q,%Q)", artist.c_str(), song.c_str(), album.c_str());


    sqlite3_exec(db, zSQL, 0, 0, 0);
    sqlite3_free(zSQL);


    cout << "here 2 " << sqlite3_errmsg(db) << "\n";
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




int CSQLiteLog::lastPlayedSongs(unsigned int n) const
{
    cout << __FUNCTION__ << "\n";

    int it = 0;
    const char *sz = "SELECT * FROM t1 GROUP BY t1.artist";
    const char *err;
    sqlite3_stmt  *stmt;
    sqlite3_prepare(db, sz, strlen(sz), &stmt, &err);

    while( SQLITE_ROW==sqlite3_step(stmt) && it < n ){
        for(int i=1; i<= 3; i++){
            const char *zText = (const char *)sqlite3_column_text(stmt, i);
            cout << zText << " ";
        }
        it++;
        cout << "\n";
    }

    return 0;
}



