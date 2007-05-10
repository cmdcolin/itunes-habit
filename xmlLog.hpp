#pragma once

#include "xmlUtils.hpp"
#include "iTunesVisualAPI.hpp"

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <memory>

XERCES_CPP_NAMESPACE_USE

class
    Log
{

public:

    Log(const std::basic_string<TCHAR> & t, bool exists);
    ~Log(void);

    void log(ITTrackInfoV1 & ti);
    void log(ITTrackInfo & ti);

    void serialize(const std::basic_string<TCHAR> & s) const;

    std::vector<const DOMElement *> lastPlayedSongs(unsigned int n) const;
    std::map<std::basic_string<XMLCh>, int> artistListeningDistribution() const;
    static void artistDistributionHelper(DOMTreeWalker * k, std::map<std::basic_string<XMLCh>, int> & m);

    inline const XERCES_CPP_NAMESPACE::DOMDocument * getContainedDocument() const { return m_doc; }
    inline XERCES_CPP_NAMESPACE::DOMDocument * getContainedDocument() { return m_doc; }

private:

    std::auto_ptr<XercesDOMParser> parser;
    XERCES_CPP_NAMESPACE::DOMDocument * m_doc;
    bool m_release_doc;

};
