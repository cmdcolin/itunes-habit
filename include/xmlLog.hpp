#pragma once

#include "xmlUtils.hpp"
#include "iTunesVisualAPI.h"

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <memory>

using namespace std;
XERCES_CPP_NAMESPACE_USE

class
    Log
{

public:

    Log(const basic_string<TCHAR> & t, bool exists);
    ~Log(void);

    void log(ITTrackInfoV1 & ti);
    void log(ITTrackInfo & ti);

    void serialize(const basic_string<TCHAR> & s) const;

    vector<const DOMElement *> lastPlayedSongs(unsigned int n) const;
    map<basic_string<XMLCh>, int> artistListeningDistribution() const;
    static void artistDistributionHelper(DOMTreeWalker * k, map<basic_string<XMLCh>, int> & m);

    inline const DOMDocument * getContainedDocument() const { return m_doc; }
    inline DOMDocument * getContainedDocument() { return m_doc; }

private:

    auto_ptr<XercesDOMParser> parser;
    DOMDocument * m_doc;
    bool m_release_doc;

};
