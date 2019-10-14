#include <string>

#include "SkyOtaHttpConnection.h"
#include "SkyOtaFileOutputStream.h"
#include "SkyOtaCurl.h"

using namespace skyota::net;
using namespace skyota::io;

#define RHAPSODY_HTTP_CONNECTION_DEBUG 0 // Ye Chen

class SkyOtaHttpConnection::Implementation : public SkyOtaCurl
{
public:
    Implementation(
        const SkyOtaHttpConnection * container,
        SkyOtaUrl & url,
        long long int & start, 
        long long int & end,
		CurlUsecaseOta & usecase);
    ~Implementation();

protected:
    int notify(int action, SkyOtaCurlStatusCode code, char * data, int size);

private: 
    const SkyOtaHttpConnection * container;
#if RHAPSODY_HTTP_CONNECTION_DEBUG
    SkyOtaFileOutputStream * output ;
#endif
	SkyOtaCurlStatusCode mResponseCode;
	long long int mContentLength;
};


int SkyOtaHttpConnection::Implementation::notify(int action, SkyOtaCurlStatusCode code, char * data, int size)
{
    SkyOtaConnectionListener * listener = container->listener;
    int ret;
    if (listener != NULL)
    {
        int error;
        if ((code == HttpOK) ||(code == HttpPartialContent))
        {
            error = SkyOtaConnectionEvent::CodeOk; 
        } 
        else if (code >=  HttpInternalServerError)
        {
            error = SkyOtaConnectionEvent::CodeServerError;
        }
        else if (code >=  HttpBadRequest)
        {
            error = SkyOtaConnectionEvent::CodeClientError;
        }
        else if (code >=  HttpMultipleChoices)
        {
            error = SkyOtaConnectionEvent::CodeFail;
        }
        else
        {
            error = SkyOtaConnectionEvent::CodeFail;
        }
        
        
        if (action == SkyOtaCurl::Ok)
        {
            if (error == SkyOtaConnectionEvent::CodeOk)
            {
                SkyOtaConnectionEvent event(SkyOtaConnectionEvent::Connected, error, data, size);
                ret = listener->connectAction(event);
#if RHAPSODY_HTTP_CONNECTION_DEBUG
                if(ret == SkyOtaConnectionListener::Ok)
                {
                    output->write(data, size);
                }
#endif                
            }
            else
            {
                SkyOtaConnectionEvent event(SkyOtaConnectionEvent::Fail, error, data, size);
                ret = listener->connectAction(event);                
            }
        }
        else if (action == SkyOtaCurl::End)
        {
            SkyOtaConnectionEvent event(SkyOtaConnectionEvent::Disconnected, error, data, size);
            ret = listener->connectAction(event);
        }
        else if (action == SkyOtaCurl::Abort)
        {
            SkyOtaConnectionEvent event(SkyOtaConnectionEvent::Abort, error, data, size);
            ret = listener->connectAction(event);
        }  
        else if (action == SkyOtaCurl::Fail)
        {
            SkyOtaConnectionEvent event(SkyOtaConnectionEvent::Fail, error, data, size);
            ret = listener->connectAction(event);
        }

        if(ret == SkyOtaConnectionListener::Pause)
        {
            return SkyOtaCurl::CurlPause; 
        }
    } 
    return SkyOtaCurl::Ok;
}


SkyOtaHttpConnection::Implementation::Implementation(
    const SkyOtaHttpConnection * container,
    SkyOtaUrl & url,
    long long int & start, 
    long long int & end,
	CurlUsecaseOta & usecase)
    :SkyOtaCurl(url, start, end, usecase), container(container)
{ 
	//mResponseCode = SkyOtaCurlStatusCode::HttpUnknown;
	mResponseCode = HttpUnknown;
	mContentLength = -1;

#if RHAPSODY_HTTP_CONNECTION_DEBUG
    std::string name = "connection";
    char format[9];
    sprintf(format, "%08x", this);
    format[8] = 0;
    name.append(format);
    name.append(".xml");
    output = new SkyOtaFileOutputStream(name);
#endif
}

SkyOtaHttpConnection::Implementation::~Implementation()
{
#if RHAPSODY_HTTP_CONNECTION_DEBUG
    delete output;
#endif
}


SkyOtaHttpConnection::SkyOtaHttpConnection()
{    
   
}

SkyOtaHttpConnection::SkyOtaHttpConnection(skyota::io::SkyOtaUrl & url)
{  
    impl = NULL;
    this->url = url;  
    start = 0;
    end = 0;
}

SkyOtaHttpConnection::~SkyOtaHttpConnection()
{
    if (impl != NULL)
    {
        delete impl;
        impl = NULL;
    }
}


void SkyOtaHttpConnection::range(long long int start, long long int end)
{
    this->start = start;
    this->end = end;
}

void SkyOtaHttpConnection::connect(CurlUsecaseOta & usecase)
{
    if (impl == NULL)
    {
        const SkyOtaHttpConnection * container = this;
        impl = new SkyOtaHttpConnection::Implementation(container, url, start, end, usecase); 
    }   
}

void SkyOtaHttpConnection::disconnect()
{
    if (impl != NULL)
    {
        impl->cancel(); 
        delete impl;
        impl = NULL;
    }
}

bool SkyOtaHttpConnection::suspensive()
{
    bool b = false; 
    if (impl != NULL)
    {
        b = impl->suspensive();
    }
    return b;
}
int SkyOtaHttpConnection::suspend()
{
    int ret = 0; 
    if (impl != NULL)
    {
        ret = impl->suspend();
    }
    return ret;
}

int SkyOtaHttpConnection::resume()
{
    int ret = 0; 
    if (impl != NULL)
    {
        ret = impl->resume();
    }
    return ret;
}




