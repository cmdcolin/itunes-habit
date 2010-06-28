#include "xmlLog.hpp"
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
    cout << __FUNCTION__ << "\n";

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

        try
        {
            parser->parse(t.c_str());
        }
        catch(XMLException & e)
        {
            cout << __FUNCTION__ << ": Rad - " << e.getMessage() << "\n";
        }
        catch(DOMException & e)
        {
            cout << __FUNCTION__ << ": Bad - " << e.getMessage() << "\n";
        }
        catch(...)
        {
            cout << __FUNCTION__ << ": Unknown exception" << "\n";
        }


        m_doc = parser->getDocument();
        m_release_doc = false;

        cout << __FUNCTION__ << ": Log Parsed: " << m_doc << "\n";
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

        cout << __FUNCTION__ << ": Log Created" << "\n";
    }
}


Log::~Log(void)
{
    cout << __FUNCTION__ << "\n";

    if(m_release_doc)
    {
        delete m_doc;
    }
}

void Log::serialize(const std::basic_string<TCHAR> & s) const
{
    cout << __FUNCTION__ << "\n";

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
    cout << __FUNCTION__ << " " << m_doc << "\n";

    if(!m_doc)
    {
        cout << __FUNCTION__ << ": No log" << "\n";
        return;
    }

    DOMElement * song = m_doc->createElement(XS("song"));
    DOMElement * song_child = 0;

    song_child = m_doc->createElement(XS("artist"));
    song_child->appendChild(m_doc->createTextNode
        (XS((char*) ti.artist + 1)));
    song->appendChild(song_child);


    song_child = m_doc->createElement(XS("album"));
    song_child->appendChild(m_doc->createTextNode
        (XS((char*) ti.album + 1)));
    song->appendChild(song_child);


    song_child = m_doc->createElement(XS("track"));
    song_child->appendChild(m_doc->createTextNode
        (XS((char*)  ti.name + 1)));
    song->appendChild(song_child);

    SYSTEMTIME t;
    GetLocalTime(&t);

    wstringstream wss;
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
    cout << __FUNCTION__ << "\n";

    if(!m_doc)
    {
        cout << __FUNCTION__ << ": No log" << "\n";
        return;
    }

    DOMElement * song = m_doc->createElement(XS("song"));
    DOMElement * song_child = 0;

    song_child = m_doc->createElement(XS("artist"));
    song_child->appendChild(m_doc->createTextNode
        ((XMLCh *) ti.artist + 1));
    song->appendChild(song_child);

    song_child = m_doc->createElement(XS("album"));
    song_child->appendChild(m_doc->createTextNode
        ((XMLCh *) ti.album + 1));
    song->appendChild(song_child);


    song_child = m_doc->createElement(XS("track"));
    song_child->appendChild(m_doc->createTextNode
        ((XMLCh *) ti.name + 1));
    song->appendChild(song_child);

    SYSTEMTIME t;
    GetLocalTime(&t);

    wstringstream wss;
    wss
        << t.wYear << L" " << t.wMonth << L" " << t.wDay << L" "
        << t.wDayOfWeek << L" " << t.wHour << L" " << t.wMinute
        << L" " << t.wSecond << L" " << t.wMilliseconds;

    song_child = m_doc->createElement(XS("time"));
    song_child->appendChild(m_doc->createTextNode((XMLCh*) wss.str().c_str()));
    song->appendChild(song_child);

    cout << wss.str().c_str() << "\n";

    m_doc->getDocumentElement()->appendChild(song);
}


map<basic_string<XMLCh>, int> Log::artistListeningDistribution() const
{
    cout << __FUNCTION__ << "\n";

    DOMTreeWalker * i = m_doc->createTreeWalker(
        m_doc->getDocumentElement(), DOMNodeFilter::SHOW_TEXT |
        DOMNodeFilter::SHOW_ELEMENT, 0, false);

    map<basic_string<XMLCh>, int> m;

    artistDistributionHelper(i, m);

    i->release();

    return m;
}


std::vector<const DOMElement *> Log::lastPlayedSongs(unsigned int n) const
{
    cout << __FUNCTION__ << "\n";

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
                cout << "Why? Item is zero" << "\n";
            }
            else
            {
                arr[i] = cur;
            }
        }
        catch(std::bad_cast & e)
        {
            cout << __FUNCTION__ << ": Bad cast - " << e.what() << "\n";
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


