#pragma once

#include <tchar.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
#include <xercesc/dom/DOMElement.hpp>
XERCES_CPP_NAMESPACE_USE

class
    Creator
{

public:

    Creator() { cout << __FUNCTION__ << "\n"; }
    ~Creator() { cout << __FUNCTION__ << "\n"; }

    void createLastPlayedChart(
        const vector<const DOMElement *> & arr, 
        LPCTSTR file, 
        LPSTR font, 
        int size) const;

    void createArtistGraph(
        const map<wstring, int> & i, 
        const basic_string<TCHAR> & file
        ) const;

    void createDynaLastPlayed(
        const vector<const DOMElement *> & arr, 
        const basic_string<TCHAR> & file
        ) const;

private:

};
