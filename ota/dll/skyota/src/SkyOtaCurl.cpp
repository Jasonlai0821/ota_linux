
#include "SkyOtaCurl.h"

#include <stdio.h>

#include "Thread.h"
#include "MessageQueue.h"

#include <fstream>
#include <map>

#include "curl/curl.h"
#include "curl/easy.h"
#include <list>

#include "SkyOtaFileOutputStream.h"
#include "debug_log.h"
#include "Utility.h"

#define SKYOTA_CURL_DEBUG 1
#define SKYOTA_CURL_VERBOSE 1

using namespace skyota::net;
using namespace skyota::io;
using namespace util;

class SkyOtaCurl::TaskItem{
public:
    TaskItem(SkyOtaCurl*ptr);
    CURL*curl;
    SkyOtaCurl*ptr;
    ~TaskItem();
#if SKYOTA_CURL_DEBUG
	SkyOtaFileOutputStream * output ;
#endif
private:
    static size_t httpBodyWrite(void * data, 
        size_t size, 
        size_t block, 
        void* tag);

	struct curl_slist * headers; 
};

size_t SkyOtaCurl::TaskItem::httpBodyWrite(void * data, 
    size_t size, 
    size_t block, 
    void* tag)
{
#if SKYOTA_CURL_DEBUG
	static int TOTAL_LENGTH = 0;
#endif
    size_t length = size*block;
    SkyOtaCurlStatusCode response;
    int ret;
    SkyOtaCurl::TaskItem * item = (SkyOtaCurl::TaskItem *)tag;    
    if(item->ptr != NULL)
    {
        curl_easy_getinfo(item->curl, CURLINFO_RESPONSE_CODE, &response); 
		if (response == HttpFound)//would be removed
		{
			return length;
		}
        ret = item->ptr->notify(SkyOtaCurl::Ok, response, (char*)data, length);
        if(ret == SkyOtaCurl::CurlPause)
        {
            item->ptr->pause = true;
            return CURL_WRITEFUNC_PAUSE;            
        }
#if SKYOTA_CURL_DEBUG
		if (item->ptr->getUsecase() == CURL_USECASE_OTA_DATA)
		{
			TOTAL_LENGTH += length;
			DEBUGPRINT(DEBUG_INFO,("hi guys! downloading length = %d\n", TOTAL_LENGTH));
		}
        item->output->write((char*)data, length);
#endif
        item->ptr->pause = false;
    }  
    return length;
}

SkyOtaCurl::TaskItem::TaskItem(SkyOtaCurl*ptr)
    :ptr(ptr),headers(NULL)
{
    skyota::io::SkyOtaUrl & url = ptr->getUrl();
    long long int & start = ptr->getStart();
    long long int & end   = ptr->getEnd();
    curl = curl_easy_init();
    if (curl == NULL)
    {           
        return ;
    }  
#if SKYOTA_CURL_DEBUG
	std::string filepath = SkyLocalMachineInfo::getInstance().getPath();
    std::string name = "update";
    char format[9];
    sprintf(format, "%08x", this);
    format[8] = 0;
    name.append(format);
	if (ptr->getUsecase() == CURL_USECASE_OTA_VERISON)
	{
		name.append(".xml");
		 output = new SkyOtaFileOutputStream(name);
	} 
	else
	{
		//name.append(".zip");
		//filepath.append(name);
		DEBUGPRINT(DEBUG_INFO,("download filepath:%s\n",filepath.c_str()));
		SkyLocalMachineInfo::getInstance().setDownloadPath(filepath);
		 output = new SkyOtaFileOutputStream(filepath);
	}
#endif
    curl_easy_setopt(curl, CURLOPT_URL, url.getUrl().c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L); 

#if SKYOTA_CURL_DEBUG
	DEBUGPRINT(DEBUG_INFO,("hi guys! %s\n", url.getUrl().c_str()));
#endif
#if SKYOTA_CURL_VERBOSE
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); 
#endif  

	CurlUsecaseOta usecase = ptr->getUsecase();
	if (usecase == CURL_USECASE_OTA_VERISON)
	{
		headers = NULL; /* init to NULL is important */

		std::string szMachinInfo;
		szMachinInfo = "MAC: ";
		szMachinInfo += SkyLocalMachineInfo::getInstance().getMac();
		headers = curl_slist_append(headers, szMachinInfo.c_str());

		szMachinInfo = "cModel: ";
		szMachinInfo += SkyLocalMachineInfo::getInstance().getModel();
		headers = curl_slist_append(headers, szMachinInfo.c_str());

		szMachinInfo = "cChip: ";
		szMachinInfo += SkyLocalMachineInfo::getInstance().getChip();
		headers = curl_slist_append(headers, szMachinInfo.c_str());

		szMachinInfo = "cSystemVersion: ";
		szMachinInfo += SkyLocalMachineInfo::getInstance().getVersion();
		headers = curl_slist_append(headers, szMachinInfo.c_str());

		//headers = curl_slist_append(headers, "MAC: B0F1ECFCC2F7");
		//headers = curl_slist_append(headers, "cModel: SBL3S");
		//headers = curl_slist_append(headers, "cChip: 2C01");
		//headers = curl_slist_append(headers, "cSystemVersion: 000000000");

		headers = curl_slist_append(headers, "cSize: 0");
		headers = curl_slist_append(headers, "Resolution: 1920*1080");
		headers = curl_slist_append(headers, "cTcVersion: 999999999");
		headers = curl_slist_append(headers, "cPkg: com.skyworth.aiui.skyotaupdate");
		headers = curl_slist_append(headers, "cBrand: skyworth");
		headers = curl_slist_append(headers, "cOpenId: 0");
		headers = curl_slist_append(headers, "country: CN");
		headers = curl_slist_append(headers, "language: zh");
		headers = curl_slist_append(headers, "cEmmcCID: 1101003030344739301096982c2b2100");
		headers = curl_slist_append(headers, "cDevOwner: system");
		headers = curl_slist_append(headers, "cMall: 1");
		headers = curl_slist_append(headers, "cSystemStatus: 1");

		/* pass our list of custom made headers */
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	}

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpBodyWrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);         
    curl_easy_setopt(curl, CURLOPT_PRIVATE, this);

	if (usecase == CURL_USECASE_OTA_DATA || usecase == CURL_USECASE_OTA_VERISON)
	{
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
		curl_easy_setopt(curl,  CURLOPT_FRESH_CONNECT, 1L);
	}
}

SkyOtaCurl::TaskItem::~TaskItem()
{
	if (headers != NULL)
	{
		curl_slist_free_all(headers); /* free the header list */
	}
	
    if (curl != NULL)
    {
        curl_easy_cleanup(curl);
    }
#if SKYOTA_CURL_DEBUG
    if(output != NULL)
    {
        delete output;
    }
#endif
}

class SkyOtaCurl::TaskManager : public os::Thread{
public:
    int addTask(SkyOtaCurl * ptr);
    void removeTask(int handle);

    int pauseTask(int handle);
    int resumeTask(int handle);    
    void Run();

    static TaskManager * getInstance(){return task;};
private:
    TaskManager();
    ~TaskManager();

    CURLM * handle;
    static TaskManager * task;
    os::Mutex     mutex;
    std::list<TaskItem *>   list;

    void removeItem(TaskItem*item);
};

SkyOtaCurl::TaskManager*  SkyOtaCurl::TaskManager::task(new SkyOtaCurl::TaskManager());


int SkyOtaCurl::TaskManager::addTask(SkyOtaCurl * ptr)
{
    os::ScopedMutex scoped(mutex);
    TaskItem * item = new TaskItem(ptr);
    if (item != NULL)
    {
        list.push_back(item);
        curl_multi_add_handle(handle, item->curl);  
    }
    return (long)item;
}

void SkyOtaCurl::TaskManager::removeItem(SkyOtaCurl::TaskItem*item)
{
    if (item != NULL)
    {
        std::list<TaskItem*>::iterator it;
        for (it = list.begin(); 
            it != list.end(); 
            it++)
        {
            TaskItem*tmp = (*it);
            if(tmp == item)
            {
                list.remove(item);
                curl_multi_remove_handle(this->handle, item->curl);
                delete item;
                break;
            }
        }        
    } 
}


void SkyOtaCurl::TaskManager::removeTask(int handle)
{
    os::ScopedMutex scoped(mutex);
    SkyOtaCurl::TaskItem*item = (SkyOtaCurl::TaskItem*)handle;
    removeItem(item);
}

int SkyOtaCurl::TaskManager::pauseTask(int handle)
{
    SkyOtaCurl::TaskItem*item = (SkyOtaCurl::TaskItem*)handle;
    CURLcode code = curl_easy_pause(item->curl, CURLPAUSE_ALL);    
    return code;
}

int SkyOtaCurl::TaskManager::resumeTask(int handle)
{
    os::ScopedMutex scoped(mutex);
    SkyOtaCurl::TaskItem*item = (SkyOtaCurl::TaskItem*)handle;
    CURLcode code = curl_easy_pause(item->curl, CURLPAUSE_CONT);
    return code;
}

SkyOtaCurl::TaskManager::TaskManager()
{
	curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_multi_init();
    Start();
}

SkyOtaCurl::TaskManager::~TaskManager()
{
    curl_multi_cleanup(handle);
}

void SkyOtaCurl::TaskManager::Run()
{
    int still_running = 0;
    bool delay = false;

    for(;;)
    { 
        {
            os::ScopedMutex scoped(mutex);            
            while(CURLM_CALL_MULTI_PERFORM ==
                curl_multi_perform(handle, &still_running));        
            delay = false;
            if (still_running)
            {
                struct timeval timeout;
                int rc; 

                fd_set fdread;
                fd_set fdwrite;
                fd_set fdexcep;
                int maxfd;

                FD_ZERO(&fdread);
                FD_ZERO(&fdwrite);
                FD_ZERO(&fdexcep);

                /* set a suitable timeout to play around with */
                timeout.tv_sec = 1;
                timeout.tv_usec = 0;

                /* get file descriptors from the transfers */
                curl_multi_fdset(handle, &fdread, &fdwrite, &fdexcep, &maxfd);
                rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
                if (rc <= 0)
                {
                    delay = true;
                }
            }
            else
            { 
                delay = true;
            }
            {
                for (;;)
                {
                    CURLMsg * message;
                    int value;
                    message = curl_multi_info_read(handle, &value);

                    if (message == NULL)
                    {
                        break;
                    }                   

                    if (message->msg == CURLMSG_DONE)
                    {
                        SkyOtaCurlStatusCode response;
                        TaskItem * item = NULL;

                        curl_easy_getinfo(message->easy_handle,  CURLINFO_PRIVATE, &item);
                        curl_easy_getinfo(message->easy_handle, CURLINFO_RESPONSE_CODE, &response);                        

                        if (message->data.result == CURLE_OK)
                        {
							#if SKYOTA_CURL_DEBUG
							item->output->close();
							#endif
                            item->ptr->notify(SkyOtaCurl::End, response, NULL, 0);                            
                        }
                        else
                        {
							#if SKYOTA_CURL_DEBUG
								item->output->close();
							#endif
                            item->ptr->notify(SkyOtaCurl::Abort, response, NULL, 0);                            
                        }
                        removeItem(item);
                    }
                }
            }
        }
        if(delay)
        {
            os::Thread::sleep(100);
        }
    }
}

SkyOtaCurl::SkyOtaCurl()
{
    pause = false;
    handle = 0; 
}

SkyOtaCurl::SkyOtaCurl(skyota::io::SkyOtaUrl & url,
    long long int & start, 
    long long int & end, 
	CurlUsecaseOta & usecase)
    :url(url),
    start(start),
    end(end),
	usecase(usecase)
{
    pause = false;
    handle = 0;    
    if ((url.getProtocol().compare(SkyOtaUrl::Http) == 0) ||
        (url.getProtocol().compare(SkyOtaUrl::Https) == 0))
    {        
        handle = TaskManager::getInstance()->addTask(this);
        if(handle == 0)
        {
            return ;
        }
    }
}

SkyOtaCurl::~SkyOtaCurl()
{ 
    cancel();
}

void SkyOtaCurl::cancel()
{
    if(this->handle != 0)
    {
        TaskManager::getInstance()->removeTask(this->handle);
        this->handle = 0;
    }
}

int SkyOtaCurl::suspend()
{
    int ret = 0;
    if(this->handle != 0)
    {
        ret = TaskManager::getInstance()->pauseTask(this->handle);
        pause = true;
    }
    return ret;
}

int SkyOtaCurl::resume()
{
    int ret = 0;
    if(pause)
    {
        if(this->handle != 0)
        {
            ret = TaskManager::getInstance()->resumeTask(this->handle);            
        }        
    }
    return ret;
}

std::string SkyOtaCurl::escape(const std::string & value)
{
    char*format=curl_escape(value.c_str(), value.length());
    std::string str(format);
    curl_free(format); 
    return str;
}

std::string SkyOtaCurl::unescape(const std::string & value)
{
    char*format=curl_unescape(value.c_str(), value.length());
    std::string str(format);
    curl_free(format); 
    return str;
}



