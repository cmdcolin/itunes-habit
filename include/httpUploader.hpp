#pragma once

#include <curl/curl.h>
#include <string>


using namespace std;
class Uploader
{

public:

    Uploader();
    ~Uploader();

    void setScriptBackend(const string & script);
    void setFormElement(const string & form_element);
    void setFormElementName(const string & form_element_name);
    int uploadFile(const string & file);

private:

    string backend;
    string element;
    string element_name;

    CURL * curl;

};
