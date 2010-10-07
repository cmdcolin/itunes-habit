
#include "stdafx.h"

#include "xmlUtils.hpp"
#include <iostream>

using namespace std;

#include <xercesc/sax/SAXParseException.hpp>


void ErrorReporter::warning(const SAXParseException & e)
{
    num_warnings++;
    def(e);
}

void ErrorReporter::error(const SAXParseException & e)
{
    num_errors++;
    def(e);
}

void ErrorReporter::fatalError(const SAXParseException & e)
{
    num_fatals++;
    def(e);
}

void ErrorReporter::def(const SAXParseException & e) {

    cout
        << L"Err at file \"" << SX(e.getSystemId())
        << L"\", line " << e.getLineNumber()
        << L", column " << e.getColumnNumber()
        << L". Message: " << SX(e.getMessage()) << "\n";

    cout
        << "Err at file \"" << SX(e.getSystemId())
        << "\", line " << e.getLineNumber()
        << ", column " << e.getColumnNumber()
        << ". Message: " << SX(e.getMessage()) << "\n";
}


void ErrorReporter::resetErrors()
{
    num_fatals = num_errors = num_warnings = 0;
}


Filter::Filter(unsigned long s)
: show(s)
{
}

DOMNodeFilter::FilterAction Filter::acceptNode(const DOMNode * /* node */) const
{
    /*

    switch (node->getNodeType())
    {

    case DOMNode::ELEMENT_NODE:
    {
    return DOMNodeFilter::FILTER_ACCEPT;
    }
    break;

    case DOMNode::COMMENT_NODE:
    {
    return DOMNodeFilter::FILTER_REJECT;

    }
    break;

    case DOMNode::TEXT_NODE:
    {
    return DOMNodeFilter::FILTER_ACCEPT;
    }
    break;

    case DOMNode::DOCUMENT_TYPE_NODE:
    {
    return DOMNodeFilter::FILTER_ACCEPT;
    }
    break;

    case DOMNode::DOCUMENT_NODE:
    {
    return DOMNodeFilter::FILTER_ACCEPT;
    }
    break;

    default:
    {
    }
    break;
    }

    */

    return DOMNodeFilter::FILTER_ACCEPT;
}
