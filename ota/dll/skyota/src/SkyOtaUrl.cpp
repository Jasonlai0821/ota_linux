#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "SkyOtaUrl.h"

using namespace skyota::io;

const std::string SkyOtaUrl::Http   = "http";
const std::string SkyOtaUrl::File   = "file";
const std::string SkyOtaUrl::Rtsp   = "rtsp";
const std::string SkyOtaUrl::Https  = "https";

SkyOtaUrl::SkyOtaUrl()
{

}

SkyOtaUrl::SkyOtaUrl(std::string & url)
:url(url)
{
    if(url.substr(0, 4).compare(SkyOtaUrl::Http) == 0)
    {
        if(url.substr(0, 5).compare(SkyOtaUrl::Https) == 0)
        {
            this->protocol = SkyOtaUrl::Https;
        }
        else
        {
            this->protocol = SkyOtaUrl::Http;
        }
    } 
    else if(url.substr(0, 4).compare(SkyOtaUrl::File) == 0)
    {
        this->protocol = SkyOtaUrl::File;
        this->file     = url.substr(6, url.length());
    } 
    else if(url.substr(0, 4).compare(SkyOtaUrl::Rtsp) == 0)
    {
        this->protocol = SkyOtaUrl::Rtsp;
    }
}


SkyOtaUrl::SkyOtaUrl(std::string & protocol, 
            std::string & host,
            int         & port,  
            std::string & file)
            :protocol(protocol), host(host), port(port), file(file) 
{
    this->url = std::string();
    this->url.append(protocol).append(":://");
    this->url.append(host);
    if (port != 0)
    {
        char ac[16] = {0};
        sprintf(ac, "%d", port);
        this->url.append(":").append(ac);
    }
    this->url.append("/").append(file);
}

SkyOtaUrl::~SkyOtaUrl()
{

}

