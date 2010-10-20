#pragma once

#include "iTunesVisualAPI.h"

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <memory>
#include <iostream>

#include "sqlite/sqlite3.h"

using namespace std;

class CSQLiteLog {

public:

    CSQLiteLog(const string &s);
    ~CSQLiteLog(void);

    void log(ITTrackInfoV1 & ti);
    void log(ITTrackInfo & ti);
    int lastPlayedSongs(unsigned int n) const;

private:

    sqlite3 * db;

};
