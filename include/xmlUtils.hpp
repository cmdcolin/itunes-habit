#pragma once

#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>

#include <string>
#include <sstream>

XERCES_CPP_NAMESPACE_USE

class
    StrX
{

public:

    StrX(const XMLCh * const toTranscode) :
      l(XMLString::transcode(toTranscode))
      {
      }

      ~StrX(void)
      {
          XMLString::release(&l);
      }


      inline const char * local(void) const
      {
          return l;
      }


private:

    char * l;

};

class
    XStr
{

public:

    XStr(const char * const toTranscode) :
      u(XMLString::transcode(toTranscode))
      {
      }

      ~XStr(void)
      {
          XMLString::release(&u);
      }

      inline const XMLCh * unicode(void) const
      {
          return u;
      }

private:

    XMLCh * u;

};

#define XS(str) XStr(str).unicode()
#define SX(str) StrX(str).local()


class ErrorReporter :
    public ErrorHandler
{

public:

    ErrorReporter() : num_errors(0), num_warnings(0), num_fatals(0) { }
    ~ErrorReporter() { }


    //
    //  Implementation of the error handler interface
    //

    void warning(const SAXParseException & c);
    void error(const SAXParseException & c);
    void fatalError(const SAXParseException & c);
    void resetErrors();

    inline bool errors_exist() const { return num_errors != 0 || num_fatals != 0; }
    inline bool warnings_exist() const { return num_warnings != 0; }

    std::wstringstream errs, warnings, fatals;
    int num_errors, num_warnings, num_fatals;

};


class Filter :
    public DOMNodeFilter
{

public:

    Filter(unsigned long s = DOMNodeFilter::SHOW_ALL);

    virtual FilterAction acceptNode(const DOMNode *) const;
    virtual unsigned long getWhatToShow(void) const { return show; }
    virtual void setWhatToShow(unsigned long s) { show = s; }

private:

    Filter(const Filter &);
    Filter & operator = (const Filter &);

    unsigned long show;

};
