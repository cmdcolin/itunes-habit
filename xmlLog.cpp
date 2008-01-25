#define _CRT_SECURE_NO_WARNINGS

#include "xmlLog.hpp"
#include "dbgUtils.hpp"
#include "gdUtils.hpp"
#include "iTunesVisualAPI.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>


Log::Log(
         const std::basic_string<TCHAR> & t,
         bool exists
         )
{
    BOOST_LOGL(app, info) << __FUNCTION__;

    if(exists)
    {
        parser.reset(new XercesDOMParser());

        BOOST_LOGL(app, info) << __FUNCTION__;

        parser->setErrorHandler(std::auto_ptr<ErrorReporter>(new ErrorReporter()).get());
        parser->setValidationScheme(XercesDOMParser::Val_Always);
        parser->setDoNamespaces(false);
        parser->setDoSchema(true);
        parser->setValidationSchemaFullChecking(true);
        parser->setLoadExternalDTD(true);

        BOOST_LOGL(app, info) << __FUNCTION__ << ": " << t.c_str();

        try
        {
            parser->parse(t.c_str());
        }
        catch(XMLException & e)
        {
            BOOST_LOGL(app, info) << __FUNCTION__ << ": Rad - " << e.getMessage();
        }
        catch(DOMException & e)
        {
            BOOST_LOGL(app, info) << __FUNCTION__ << ": Bad - " << e.getMessage();
        }
        catch(...)
        {
            BOOST_LOGL(app, info) << __FUNCTION__ << ": Unknown exception";
        }

        BOOST_LOGL(app, info) << __FUNCTION__;

        m_doc = parser->getDocument();
        m_release_doc = false;

        BOOST_LOGL(app, info) << __FUNCTION__ << ": Log Parsed: " << m_doc;
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

        m_doc = impl->createDocument(XS("root"), XS("log"), dtype);

        m_doc->setEncoding(XS("UTF-8"));
        m_release_doc = true;

        BOOST_LOGL(app, info) << __FUNCTION__ << ": Log Created";
    }
}


Log::~Log(void)
{
    BOOST_LOGL(app, info) << __FUNCTION__;

    if(m_release_doc)
    {
        delete m_doc;
    }
}

void Log::serialize(const std::basic_string<TCHAR> & s) const
{
    BOOST_LOGL(app, info) << __FUNCTION__;

    DOMImplementationLS * impl = (DOMImplementationLS *)
        DOMImplementationRegistry::getDOMImplementation(XS("LS"));

    DOMWriter * writer = impl->createDOMWriter();

    if(writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

    if(writer->canSetFeature(XMLUni::fgDOMWRTBOM, true))
        writer->setFeature(XMLUni::fgDOMWRTBOM, true);

    if(writer->canSetFeature(XMLUni::fgDOMWhitespaceInElementContent, true))
        writer->setFeature(XMLUni::fgDOMWhitespaceInElementContent, true);

    XMLFormatTarget * ft = new LocalFileFormatTarget(s.c_str());

    writer->writeNode(ft, *m_doc);
    writer->release();

    delete ft;
}

void Log::log(ITTrackInfoV1 & ti)
{
    BOOST_LOGL(app, info) << __FUNCTION__ << " " << m_doc;

    if(!m_doc)
    {
        BOOST_LOGL(app, info) << __FUNCTION__ << ": No log";
        return;
    }

    DOMElement * song = m_doc->createElement(XS("song"));
    DOMElement * song_child = 0;

    BOOST_LOGL(app, info) << __FUNCTION__;
    song_child = m_doc->createElement(XS("artist"));
    song_child->appendChild(m_doc->createTextNode
        (XS(reinterpret_cast<char *>(ti.artist + 1))));
    song->appendChild(song_child);


    BOOST_LOGL(app, info) << __FUNCTION__;
    song_child = m_doc->createElement(XS("album"));
    song_child->appendChild(m_doc->createTextNode
        (XS(reinterpret_cast<char *>(ti.album + 1))));
    song->appendChild(song_child);


    BOOST_LOGL(app, info) << __FUNCTION__;
    song_child = m_doc->createElement(XS("track"));
    song_child->appendChild(m_doc->createTextNode
        (XS(reinterpret_cast<char *>(ti.name + 1))));
    song->appendChild(song_child);

    BOOST_LOGL(app, info) << __FUNCTION__;
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
    BOOST_LOGL(app, info) << __FUNCTION__ << " " << m_doc;

    if(!m_doc)
    {
        BOOST_LOGL(app, info) << __FUNCTION__ << ": No log";
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

    m_doc->getDocumentElement()->appendChild(song);
}


std::map<std::basic_string<XMLCh>, int> Log::artistListeningDistribution() const
{
    BOOST_LOGL(app, info) << __FUNCTION__;

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
    BOOST_LOGL(app, info) << __FUNCTION__;

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
                BOOST_LOGL(app, info) << "Why? Item is zero";
            }
            else
            {
                arr[i] = cur;
            }
        }
        catch(std::bad_cast & e)
        {
            BOOST_LOGL(app, info) << __FUNCTION__ << ": Bad cast - " << e.what();
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


/*

void Plugin::imgwrite()
{
    BOOST_LOGL(app, info) << __FUNCTION__;

    std::map<std::wstring, int> a = artists();

    struct comparator :
        public std::binary_function
            <
                std::pair<std::wstring, int>,
                std::pair<std::wstring, int>,
                bool
            >
    {
        bool operator () (const std::pair<std::wstring, int> & x, const std::pair<std::wstring, int> & y)
        {
            return x.second < y.second;
        }
    };

    std::vector< std::pair<std::wstring, int> > v(a.begin(), a.end());
    std::sort(v.begin(), v.end(), comparator());

    size_t r = 0;

    std::vector< std::pair<std::wstring, int> >::reverse_iterator it = v.rbegin();
    std::vector< std::pair<std::wstring, int> >::iterator ita = v.begin();

    while(r++ < ((v.size() < 5) ? v.size() : 5))
    {
        ita++;
        it++;
    }


    gdFontPtr font = gdFontGetTiny();
	gdWimage image(500, 10 + font->h * 5);

    gdImageFilledRectangle(image.p, 1, 1, image.p->sx - 2, image.p->sy - 2,
        gdImageColorAllocate(image.p, randRange(128, 196), randRange(128, 196), randRange(128, 196)));
    gdImageRectangle(image.p, 0, 0, image.p->sx - 1, image.p->sy - 1,
        gdImageColorAllocate(image.p, 0, 0, 0));

    DOMNodeList * songs = m_doc->getElementsByTagName(XS("song"));

    for(XMLSize_t i = 0, k = songs->getLength(), j = 5; i < ((v.size() < 5) ? v.size() : 5); ++i, j += font->h)
    {
        try
        {
            DOMElement * child = dynamic_cast<DOMElement *>(songs->item(k - 1 - i));

            SYSTEMTIME t;
            std::wstringstream iss, oss;

            iss << child->getElementsByTagName(XS("time"))->item(0)->getTextContent();

            iss >> t.wYear >> t.wMonth >> t.wDay >> t.wDayOfWeek >>
                t.wHour >> t.wMinute >> t.wSecond >> t.wMilliseconds;

            oss << t.wYear << L"/" << t.wMonth << L"/" << t.wDay << L" "
                << t.wHour << L":" << t.wMinute << L":" << t.wSecond << " - "
                << child->getElementsByTagName(XS("artist"))->item(0)->getTextContent() << " - "
                << child->getElementsByTagName(XS("track"))->item(0)->getTextContent();

            std::wstring temp = oss.str();
            std::vector<wchar_t> m(temp.begin(), temp.end());
            m.push_back(L'\0');

            gdImageString16(image.p, font, 10, j,
                reinterpret_cast<unsigned short *>(&m[0]),
                gdImageColorAllocate(image.p, 0, 0, 0));
        }

        catch(std::bad_cast & e)
        {
            BOOST_LOGL(app, info) << __FUNCTION__ << ": " << e.what();
        }
    }

    FILE * out = fopen("C:/img.jpg", "wb");

    int size = 0;
    void * rawJpegData = gdImageJpegPtr(image.p, &size, -1);

    fwrite(rawJpegData, 1, size, out);
    gdFree(rawJpegData);

    fclose(out);
}

*/
