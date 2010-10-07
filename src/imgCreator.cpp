#include "stdafx.h"

#undef DOMDocument
#include "imgCreator.hpp"
#include "xmlUtils.hpp"

#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>


#include <gd.h>
#include <gdfontt.h>

using namespace std;


inline unsigned int rr(unsigned int start, unsigned int end)
{
    return (rand() % (end - start)) + start;
}


void Creator::createLastPlayedChart(const vector<const DOMElement *> & arr, 
                                    LPCTSTR file, 
                                    LPSTR font,
                                    int fsize
                                    ) 
                                    const
{
    cout << __FUNCTION__ << "\n";

    int bounds[8];
    stringstream fulltext;

    for(vector<const DOMElement *>::const_iterator i = arr.begin();
        i != arr.end(); ++i)
    {
        SYSTEMTIME t;
        wstringstream iss, oss;

        iss << (*i)->getElementsByTagName(XS("time"))->item(0)->getTextContent();

        iss >> t.wYear >> t.wMonth >> t.wDay >> t.wDayOfWeek >>
            t.wHour >> t.wMinute >> t.wSecond >> t.wMilliseconds;


        oss << (*i)->getElementsByTagName(XS("artist"))->item(0)->getTextContent() << " - "
            << (*i)->getElementsByTagName(XS("track"))->item(0)->getTextContent();

        wstring temp = oss.str();
        vector<wchar_t> m(temp.begin(), temp.end());
        m.push_back(L'\0');

        char sss[800];
        ::WideCharToMultiByte(CP_UTF8, 0, &m[0], (int) m.size(), sss, sizeof(sss), 0, 0);

        fulltext << setfill('0') << setw(2) << t.wHour << ":"
            << setfill('0') << setw(2) << t.wMinute << " ";
        fulltext << sss << "\n";
    }



    // get bounding rectangle for the text
    string fulltext_str(fulltext.str());
    vector<char> fulltext_cstr(fulltext_str.begin(), fulltext_str.end());
    fulltext_cstr.push_back('\0');

    cout << fulltext.str() << "\n";

    // get bounding box
    char * err = gdImageStringFT(0, bounds, 0, font, fsize, 0.0, 0, 0, &fulltext_cstr[0]);
    if(err)
        cout << __FUNCTION__ << "gdImageStringFT: " << err;

    int x = bounds[2]-bounds[6] + 6;
    int y = bounds[3]-bounds[7] + 6;

    gdImagePtr p = gdImageCreate(x, y);
    if(!p)
    {
        cout << "gdImageCreate\n";
        return;
    }

    unsigned int 
        r = rr(170, 255), 
        g = rr(170, 255), 
        b = rr(170, 255);

    int brush = gdImageColorResolve(p, r, g, b);
    int pen = gdImageColorResolve(p, 0, 0, 0);

    gdImageFilledRectangle(p, 1, 1, p->sx - 2, p->sy - 2, brush);
    gdImageRectangle(p, 0, 0, p->sx - 1, p->sy - 1, pen);

    x = 3 - bounds[6];
    y = 3 - bounds[7];

    // Write a string
    err = gdImageStringFT(p, bounds, pen, font, fsize, 0.0, x, y, &fulltext_cstr[0]);
    if(err) {
        gdImageDestroy(p);
        cout << __FUNCTION__ << "gdImageStringFT: " << err;
        return;
    }


    FILE * imagefile = _tfopen(file, TEXT("wb"));
    if(!imagefile) {
        cout << "fopen\n";
        gdImageDestroy(p);
        return;
    }



    // write to file
    int size;
    char* data = (char*)gdImagePngPtr(p, &size);
    fwrite(data, sizeof(char), size, imagefile);
    gdFree(data);


    fclose(imagefile);

    // free image
    gdImageDestroy(p);

    cout << "Done\n";
}



void Creator::createDynaLastPlayed(const vector<const DOMElement *> & arr, const basic_string<TCHAR> & file) const
{
    cout << __FUNCTION__ << "\n";

    DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(XS("LS"));


    DOMDocument * doc = impl->createDocument(XS("root"), XS("signature"), 0);
    //doc->setEncoding(XS("UTF-8"));

    DOMElement * items = doc->createElement(XS("items"));



    int x = 5, y = 5;

    for(vector<const DOMElement *>::const_iterator i = arr.begin();
        i != arr.end(); ++i, y += 20)
    {
        SYSTEMTIME t;
        wstringstream iss, oss;
        wstring ws;

        iss << (*i)->getElementsByTagName(XS("time"))->item(0)->getTextContent();

        iss >> t.wYear >> t.wMonth >> t.wDay >> t.wDayOfWeek >>
            t.wHour >> t.wMinute >> t.wSecond >> t.wMilliseconds;


        oss << setfill(L'0') << setw(2) << t.wMonth << L"/"
            << setfill(L'0') << setw(2) << t.wDay << L" "
            << setfill(L'0') << setw(2) << t.wHour << L":"
            << setfill(L'0') << setw(2) << t.wMinute << L":"
            << setfill(L'0') << setw(2) << t.wSecond << " - "
            << (*i)->getElementsByTagName(XS("artist"))->item(0)->getTextContent() << " - "
            << (*i)->getElementsByTagName(XS("track"))->item(0)->getTextContent();

        DOMElement * text = doc->createElement(XS("text"));
        DOMElement * string = doc->createElement(XS("string"));
        DOMElement * position = doc->createElement(XS("position"));
        DOMElement * color = doc->createElement(XS("color"));

        string->appendChild(doc->createTextNode((XMLCh*) oss.str().c_str()));
        color->appendChild(doc->createTextNode(XS("0x000000")));

        oss.str(L"");
        oss << x;
        oss >> ws;
        position->setAttribute(XS("x"), (XMLCh*) ws.c_str());

        oss.str(L"");
        oss << y;
        oss >> ws;
        position->setAttribute(XS("y"), (XMLCh*) ws.c_str());

        text->appendChild(string);
        text->appendChild(position);
        text->appendChild(color);

        items->appendChild(text);
    }

    doc->getDocumentElement()->appendChild(items);


    DOMLSSerializer* writer = impl->createLSSerializer();
    DOMConfiguration* dc = writer->getDomConfig();


    writer->writeToString(doc, 0);
    writer->release();

    delete doc;
}
