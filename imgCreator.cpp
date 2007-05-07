#include "imgCreator.hpp"
#include "gdUtils.hpp"
#include "dbgUtils.hpp"
#include "xmlUtils.hpp"

#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>

inline unsigned int rr(
                       unsigned int start, 
                       unsigned int end
                       )
{
    if(start == end)
        return end;

    return (rand() % (start - end)) + start;
}


void Creator::createLastPlayedChart(const std::vector<const DOMElement *> & arr, const std::basic_string<TCHAR> & file) const
{
    BOOST_LOGL(app, info) << __FUNCTION__;

    gdFontPtr font = gdFontGetTiny();
	gdWimage image(500, 97);

    int brush = gdImageColorResolve(image.p, 255, 255, 255);
    int pen = gdImageColorResolve(image.p, 0, 0, 0);

    gdImageFilledRectangle(image.p, 1, 1, image.p->sx - 2, image.p->sy - 2, brush);
    gdImageRectangle(image.p, 0, 0, image.p->sx - 1, image.p->sy - 1, pen);

    int stringPlacement = 15;

    for(std::vector<const DOMElement *>::const_iterator i = arr.begin(); 
        i != arr.end(); ++i)
    {
        SYSTEMTIME t;
        std::wstringstream iss, oss;

        iss << (*i)->getElementsByTagName(XS("time"))->item(0)->getTextContent();

        iss >> t.wYear >> t.wMonth >> t.wDay >> t.wDayOfWeek >> 
            t.wHour >> t.wMinute >> t.wSecond >> t.wMilliseconds;


        oss << std::setfill(L'0') << std::setw(2) << t.wHour << L":" 
            << std::setfill(L'0') << std::setw(2) << t.wMinute << L":" 
            << std::setfill(L'0') << std::setw(2) << t.wSecond << " - " 
            << (*i)->getElementsByTagName(XS("artist"))->item(0)->getTextContent() << " - "
            << (*i)->getElementsByTagName(XS("track"))->item(0)->getTextContent();

        std::wstring temp = oss.str();
        std::vector<wchar_t> m(temp.begin(), temp.end());
        m.push_back(L'\0');

        int b[] = { 10, stringPlacement + 20, 500, stringPlacement + 20, 10, stringPlacement, 500, stringPlacement };

        char sss[800];
        ::WideCharToMultiByte(CP_UTF8, 0, &m[0], m.size(), sss, 800, 0, 0);
        char * e = gdImageStringFT(image.p, b, pen, "C:/Documents and Settings/Tonto/My Documents/BEAUTYSC.TTF", 24, 0.0, 10, stringPlacement + 15, sss);

        if(e)
            BOOST_LOG(app) << e;

        stringPlacement += 29;
    }


    std::fstream o(file.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);

    int size = 0;
    void * rawJpegData = gdImageJpegPtr(image.p, &size, -1);

    o.write(static_cast<char *>(rawJpegData), size);
    gdFree(rawJpegData);
}



void Creator::createDynaLastPlayed(const std::vector<const DOMElement *> & arr, const std::basic_string<TCHAR> & file) const
{
    BOOST_LOGL(app, info) << __FUNCTION__;

    DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(XS("LS"));
    DOMImplementationLS * impl_ls = (DOMImplementationLS *) impl;

    DOMWriter * writer = impl_ls->createDOMWriter();

    DOMDocument * doc = impl->createDocument(XS("root"), XS("signature"), 0);
    doc->setEncoding(XS("UTF-8"));

    DOMElement * items = doc->createElement(XS("items"));

    int x = 5, y = 5;
    for(std::vector<const DOMElement *>::const_iterator i = arr.begin(); 
        i != arr.end(); ++i, y += 20)
    {
        SYSTEMTIME t;
        std::wstringstream iss, oss;
        std::wstring ws;

        iss << (*i)->getElementsByTagName(XS("time"))->item(0)->getTextContent();

        iss >> t.wYear >> t.wMonth >> t.wDay >> t.wDayOfWeek >> 
            t.wHour >> t.wMinute >> t.wSecond >> t.wMilliseconds;


        oss << std::setfill(L'0') << std::setw(2) << t.wMonth << L"/" 
            << std::setfill(L'0') << std::setw(2) << t.wDay << L" " 
            << std::setfill(L'0') << std::setw(2) << t.wHour << L":" 
            << std::setfill(L'0') << std::setw(2) << t.wMinute << L":" 
            << std::setfill(L'0') << std::setw(2) << t.wSecond << " - " 
            << (*i)->getElementsByTagName(XS("artist"))->item(0)->getTextContent() << " - "
            << (*i)->getElementsByTagName(XS("track"))->item(0)->getTextContent();

        DOMElement * text = doc->createElement(XS("text"));
        DOMElement * string = doc->createElement(XS("string"));
        DOMElement * position = doc->createElement(XS("position"));
        DOMElement * color = doc->createElement(XS("color"));

        string->appendChild(doc->createTextNode(oss.str().c_str()));
        color->appendChild(doc->createTextNode(XS("0x000000")));

        oss.str(L"");
        oss << x;
        oss >> ws;
        position->setAttribute(XS("x"), ws.c_str());
        
        oss.str(L"");
        oss << y;
        oss >> ws;
        position->setAttribute(XS("y"), ws.c_str());

        text->appendChild(string);
        text->appendChild(position);
        text->appendChild(color);

        items->appendChild(text);
    }

    doc->getDocumentElement()->appendChild(items);

    if(writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
        writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

    if(writer->canSetFeature(XMLUni::fgDOMWRTBOM, true))
        writer->setFeature(XMLUni::fgDOMWRTBOM, true);

    if(writer->canSetFeature(XMLUni::fgDOMWhitespaceInElementContent, true))
        writer->setFeature(XMLUni::fgDOMWhitespaceInElementContent, true);

    XMLFormatTarget * ft = new LocalFileFormatTarget(file.c_str());

    writer->writeNode(ft, *doc);
    writer->release();

    delete ft;
    delete doc;
}
