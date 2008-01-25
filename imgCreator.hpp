#pragma once

#include <tchar.h>
#include <map>
#include <string>
#include <vector>

#include "dbgUtils.hpp"
#include <xercesc/dom/DOMElement.hpp>
XERCES_CPP_NAMESPACE_USE

class
    Creator
{

public:

    Creator() { BOOST_LOGL(app, info) << __FUNCTION__; }
    ~Creator() { BOOST_LOGL(app, info) << __FUNCTION__; }

    void createLastPlayedChart(
        const std::vector<const DOMElement *> & arr, 
        const std::basic_string<TCHAR> & file, 
        const std::string & font, 
        int size) const;

    void createArtistGraph(
        const std::map<std::wstring, int> & i, 
        const std::basic_string<TCHAR> & file
        ) const;

    void createDynaLastPlayed(
        const std::vector<const DOMElement *> & arr, 
        const std::basic_string<TCHAR> & file
        ) const;

private:

};
