#define _CRT_SECURE_NO_WARNINGS

#include "xmlLog.hpp"
#include "gdUtils.hpp"
#include "iTunesVisualAPI.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include <iostream>

using namespace std;

Log::Log(
         const std::basic_string<TCHAR> & t,
         bool exists
         )
{
    cerr << __FUNCTION__;

    if(exists)
    {
        std::auto_ptr<ErrorReporter> err(new ErrorReporter());

        parser.reset(new XercesDOMParser());
        parser->setErrorHandler(err.get());
        parser->setValidationScheme(XercesDOMParser::Val_Always);
        parser->setDoNamespaces(false);
        parser->setDoSchema(true);
        parser->setValidationSchemaFullChecking(true);
        parser->setLoadExternalDTD(true);

        cerr << __FUNCTION__ << ": " << t.c_str();

        try
        {
            parser->parse(t.c_str());
        }
        catch(XMLException & e)
        {
            cerr << __FUNCTION__ << ": Rad - " << e.getMessage();
        }
        catch(DOMException & e)
        {
            cerr << __FUNCTION__ << ": Bad - " << e.getMessage();
        }
        catch(...)
        {
            cerr << __FUNCTION__ << ": Unknown exception";
        }

        cerr << __FUNCTION__;

        m_doc = parser->getDocument();
        m_release_doc = false;

        cerr << __FUNCTION__ << ": Log Parsed: " << m_doc;
    }

    else
    {
        DOMImplementation * impl = DOMImplementationRegistry::
            getDOMImplementation(XS("LS"));

        DOMDocumentType * dtype = impl->createDocumentType(XS("log"), XS("id"),

#ifndef _UNICODE
            XS(t.c_str()));
#else
            t.c_str());
#endif

        m_doc = impl->createDocument(0, XS("log"), dtype);

        // m_doc->setEncoding(XS("UTF-8"));
        m_release_doc = true;

        cerr << __FUNCTION__ << ": Log Created";
    }
}


Log::~Log(void)
{
    cerr << __FUNCTION__;

    if(m_release_doc)
    {
        delete m_doc;
    }
}

void Log::serialize(const std::basic_string<TCHAR> & s) const
{
    cerr << __FUNCTION__;

    DOMImplementationLS * impl = (DOMImplementationLS *)
        DOMImplementationRegistry::getDOMImplementation(XS("LS"));

    DOMLSSerializer* writer = impl->createLSSerializer();

    DOMConfiguration* dc = writer->getDomConfig();


    writer->writeToString(m_doc, 0);
    //writer->writeNode(ft, *m_doc);
    writer->release();

    //delete ft;
}

void Log::log(ITTrackInfoV1 & ti)
{
    cerr << __FUNCTION__ << " " << m_doc;

    if(!m_doc)
    {
        cerr << __FUNCTION__ << ": No log";
        return;
    }

    DOMElement * song = m_doc->createElement(XS("song"));
    DOMElement * song_child = 0;

    cerr << __FUNCTION__;
    song_child = m_doc->createElement(XS("artist"));
    song_child->appendChild(m_doc->createTextNode
        (XS(reinterpret_cast<char *>(ti.artist + 1))));
    song->appendChild(song_child);


    cerr << __FUNCTION__;
    song_child = m_doc->createElement(XS("album"));
    song_child->appendChild(m_doc->createTextNode
        (XS(reinterpret_cast<char *>(ti.album + 1))));
    song->appendChild(song_child);


    cerr << __FUNCTION__;
    song_child = m_doc->createElement(XS("track"));
    song_child->appendChild(m_doc->createTextNode
        (XS(reinterpret_cast<char *>(ti.name + 1))));
    song->appendChild(song_child);

    cerr << __FUNCTION__;
    SYSTEMTIME t;
    ::GetLocalTime(&t);

    std::wstringstream wss;
    wss
        << t.wYear << L":" << t.wMonth << L":" << t.wDay << L":"
        << t.wDayOfWeek << L":" << t.wHour << L":" << t.wMinute
        << L":" << t.wSecond << L":" << t.wMilliseconds;

    song_child = m_doc->createElement(XS("time"));
    song_child->appendChild(m_doc->createTextNode((XMLCh*) wss.str().c_str()));
    song->appendChild(song_child);

    m_doc->getDocumentElement()->appendChild(song);
}

void Log::log(ITTrackInfo & ti)
{
    cerr << __FUNCTION__;

    if(!m_doc)
    {
        cerr << __FUNCTION__ << ": No log";
        return;
    }

    DOMElement * song = m_doc->createElement(XS("song"));
    DOMElement * song_child = 0;

    song_child = m_doc->createElement(XS("artist"));
    song_child->appendChild(m_doc->createTextNode
        (reinterpret_cast<XMLCh *>(ti.artist + 1)));
    song->appendChild(song_child);

    song_child = m_doc->createElement(XS("album"));
    song_child->appendChild(m_doc->createTextNode
        (reinterpret_cast<XMLCh *>(ti.album + 1)));
    song->appendChild(song_child);


    song_child = m_doc->createElement(XS("track"));
    song_child->appendChild(m_doc->createTextNode
        (reinterpret_cast<XMLCh *>(ti.name + 1)));
    song->appendChild(song_child);

    SYSTEMTIME t;
    ::GetLocalTime(&t);

    std::wstringstream wss;
    wss
        << t.wYear << L" " << t.wMonth << L" " << t.wDay << L" "
        << t.wDayOfWeek << L" " << t.wHour << L" " << t.wMinute
        << L" " << t.wSecond << L" " << t.wMilliseconds;

    song_child = m_doc->createElement(XS("time"));
    song_child->appendChild(m_doc->createTextNode((XMLCh*) wss.str().c_str()));
    song->appendChild(song_child);

    cerr << wss.str().c_str();

    m_doc->getDocumentElement()->appendChild(song);
}


std::map<std::basic_string<XMLCh>, int> Log::artistListeningDistribution() const
{
    cerr << __FUNCTION__;

    DOMTreeWalker * i = m_doc->createTreeWalker(
        m_doc->getDocumentElement(), DOMNodeFilter::SHOW_TEXT |
        DOMNodeFilter::SHOW_ELEMENT, 0, false);

    std::map<std::basic_string<XMLCh>, int> m;

    artistDistributionHelper(i, m);

    i->release();

    return m;
}


std::vector<const DOMElement *> Log::lastPlayedSongs(unsigned int n) const
{
    cerr << __FUNCTION__;

    DOMNodeList * list = m_doc->getElementsByTagName(XS("song"));
    XMLSize_t e = list->getLength();

    if(e < n)
    {
        n = e;
    }

    e--;
    std::vector<const DOMElement *> arr(n);

    for(unsigned int i = 0; i < n; ++i, e--)
    {
        try
        {
            DOMElement * cur = dynamic_cast<DOMElement *>(list->item(e));

            if(!cur)
            {
                cerr << "Why? Item is zero";
            }
            else
            {
                arr[i] = cur;
            }
        }
        catch(std::bad_cast & e)
        {
            cerr << __FUNCTION__ << ": Bad cast - " << e.what();
        }
    }

    return arr;
}


/* static */ void Log::artistDistributionHelper(DOMTreeWalker * k, std::map<std::basic_string<XMLCh>, int> & m)
{
    DOMNode * n = k->getCurrentNode();

    if(n->getNodeType() == DOMNode::TEXT_NODE)
    {
        if(std::basic_string<XMLCh>(n->getParentNode()->getNodeName()) == XS("artist"))
        {
            m[n->getParentNode()->getTextContent()]++;
        }
    }

    for(DOMNode * child = k->firstChild(); child != 0; child = k->nextSibling())
    {
        artistDistributionHelper(k, m);
    }

    k->setCurrentNode(n);
}


