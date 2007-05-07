#pragma once

#include <curl/curl.h>
#include <string>

class Uploader
{

public:

    Uploader();
    ~Uploader();

    void setScriptBackend(const std::string & script);
    void setFormElement(const std::string & form_element);
    void setFormElementName(const std::string & form_element_name);
    int uploadFile(const std::string & file);

private:

    std::string backend;
    std::string element;
    std::string element_name;

    CURL * curl;

};