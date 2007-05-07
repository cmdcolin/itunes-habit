#include "xmlUtils.hpp"
#include "dbgUtils.hpp"

#include <xercesc/sax/SAXParseException.hpp>

void ErrorReporter::warning(const SAXParseException & e)
{
    num_warnings++;

    warnings
        << L"Warning at file \"" << SX(e.getSystemId())
        << L"\", line " << e.getLineNumber()
        << L", column " << e.getColumnNumber()
        << L". Message: " << SX(e.getMessage()) << std::endl;

    BOOST_LOGL(app, warn)
        << "Warning at file \"" << SX(e.getSystemId())
        << "\", line " << e.getLineNumber()
        << ", column " << e.getColumnNumber()
        << ". Message: " << SX(e.getMessage());
}

void ErrorReporter::error(const SAXParseException & e)
{
    num_errors++;

    errs
        << L"Error at file \"" << SX(e.getSystemId())
        << L"\", line " << e.getLineNumber()
        << L", column " << e.getColumnNumber()
        << L". Message: " << SX(e.getMessage()) << std::endl;

    BOOST_LOGL(app, err)
        << "Error at file \"" << SX(e.getSystemId())
        << "\", line " << e.getLineNumber()
        << ", column " << e.getColumnNumber()
        << ". Message: " << SX(e.getMessage());
}

void ErrorReporter::fatalError(const SAXParseException & e)
{
    num_fatals++;

    fatals
        << L"Fatal at file \"" << SX(e.getSystemId())
        << L"\", line " << e.getLineNumber()
        << L", column " << e.getColumnNumber()
        << L". Message: " << SX(e.getMessage()) << std::endl;

    BOOST_LOGL(app, fatal)
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

short Filter::acceptNode(const DOMNode * /* node */) const
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
