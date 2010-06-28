#include "xmlUtils.hpp"
#include <iostream>

using namespace std;

#include <xercesc/sax/SAXParseException.hpp>


void ErrorReporter::warning(const SAXParseException & e)
{
    num_warnings++;

    cerr
        << L"Warning at file \"" << SX(e.getSystemId())
        << L"\", line " << e.getLineNumber()
        << L", column " << e.getColumnNumber()
        << L". Message: " << SX(e.getMessage()) << std::endl;

    cerr
        << "Warning at file \"" << SX(e.getSystemId())
        << "\", line " << e.getLineNumber()
        << ", column " << e.getColumnNumber()
        << ". Message: " << SX(e.getMessage());
}

void ErrorReporter::error(const SAXParseException & e)
{
    num_errors++;

    cerr
        << L"Error at file \"" << SX(e.getSystemId())
        << L"\", line " << e.getLineNumber()
        << L", column " << e.getColumnNumber()
        << L". Message: " << SX(e.getMessage()) << std::endl;

    cerr
        << "Error at file \"" << SX(e.getSystemId())
        << "\", line " << e.getLineNumber()
        << ", column " << e.getColumnNumber()
        << ". Message: " << SX(e.getMessage());
}

void ErrorReporter::fatalError(const SAXParseException & e)
{
    num_fatals++;

    cerr
        << L"Fatal at file \"" << SX(e.getSystemId())
        << L"\", line " << e.getLineNumber()
        << L", column " << e.getColumnNumber()
        << L". Message: " << SX(e.getMessage()) << std::endl;

    cerr
        << "Fatal at file \"" << SX(e.getSystemId())
        << "\", line " << e.getLineNumber()
        << ", column " << e.getColumnNumber()
        << ". Message: " << SX(e.getMessage());
}

void ErrorReporter::resetErrors()
{
    num_fatals = num_errors = num_warnings = 0;

    warnings.str(L"");
    errs.str(L"");
    fatals.str(L"");
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
