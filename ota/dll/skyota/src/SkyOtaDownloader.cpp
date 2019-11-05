#include "SkyOtaDownloader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <map>
#include "debug_log.h"

using namespace skyota::facade;
using namespace util;

std::string URL_VERSION = "http://api.upgrade.skysrt.com/ied/v3/getUpgrader";

SkyOtaDownloader::SkyOtaDownloader(std::string mac,
	std::string model,
	std::string chip,
	std::string version):sema_version(os::Semaphore(0))
{
	m_prOtaDownloaderListner = NULL;

	m_prInternalVersionListner = new InternalVersionListner(this);
	m_prInternalDownloadingListner = new InternalDownloadingListner(this);

	SkyLocalMachineInfo::getInstance().set(mac, model, chip, version);

}


SkyOtaDownloader::~SkyOtaDownloader(void)
{
	if (m_prInternalVersionListner != NULL)
	{
		delete m_prInternalVersionListner;
		m_prInternalVersionListner = NULL;
	}
	if (m_prInternalDownloadingListner != NULL)
	{
		delete m_prInternalDownloadingListner;
		m_prInternalDownloadingListner = NULL;
	}
	sema_version.release();
}

class OtaAction{
public:
	SkyOtaConnectionListener * listner;
	int value;

public:
	static const int ACTION_CHECK_VERSION = 0;
	static const int ACTION_START_DOWNLOADING_IMAGE  = 1;
	static const int ACTION_STOP_DOWNLOADING_IMAGE = 2;
	static const int ACTION_STOP_DOWNLOADING_FINISHED = 3;
};

class OtaSyncToAsync : public os::Thread{
public:
	static OtaSyncToAsync & getInstance();
	void addAction(OtaAction * action);
	void removeAction(OtaAction * action);

	void exitNow();
public:
	void Run();
private:
	OtaSyncToAsync();
	~OtaSyncToAsync();
private:
	bool running;
	os::MessageQueue<OtaAction *> queue;
	os::Mutex      mutex;
	std::list<OtaAction*> list;

private:
	void deleteAction(OtaAction * action);
	void clearOtaVersionConnection();
	void clearOtaDataConnection();

	SkyOtaHttpConnection * m_prVersionConnection;
	SkyOtaHttpConnection * m_prDataConnection;
};

OtaSyncToAsync & OtaSyncToAsync::getInstance()
{
	os::PthreadScopedMutex scope;
	static OtaSyncToAsync instance;
	return instance;
}

OtaSyncToAsync::OtaSyncToAsync()
{
	m_prVersionConnection = NULL;
	running = true;
	Start();
}

OtaSyncToAsync::~OtaSyncToAsync()
{
	running = false;    
	Wait();

	std::list<OtaAction*>::iterator it;
	for (it = list.begin(); 
		it != list.end(); 
		it++)
	{
		delete (*it);
		*it = NULL;
	}
	list.clear();
}

void OtaSyncToAsync::exitNow()
{
	os::ScopedMutex scoped(mutex);

	OtaAction * action = new OtaAction();
	action->value = OtaAction::ACTION_STOP_DOWNLOADING_IMAGE;
	action->listner = NULL;
	list.push_back(action);
	DEBUGPRINT(DEBUG_INFO,("exitNow() \n"));
}

void OtaSyncToAsync::clearOtaVersionConnection()
{
	if (m_prVersionConnection != NULL)
	{
		m_prVersionConnection->disconnect();
		m_prVersionConnection->resetListener();
		delete m_prVersionConnection;
		m_prVersionConnection = NULL;
		DEBUGPRINT(DEBUG_INFO,("clearOtaVersionConnection() \n"));
	}
}

void OtaSyncToAsync::clearOtaDataConnection()
{
	if (m_prDataConnection != NULL)
	{
		m_prDataConnection->disconnect();
		m_prDataConnection->resetListener();
		delete m_prDataConnection;
		m_prDataConnection = NULL;
		DEBUGPRINT(DEBUG_INFO,("clearOtaDataConnection() \n"));
	}
}

void OtaSyncToAsync::addAction(OtaAction * action)
{  
	{
		os::ScopedMutex scoped(mutex);
		list.push_back(action);
	}
	queue.send(action);
}

void OtaSyncToAsync::deleteAction(OtaAction * action)
{   
	{  
		os::ScopedMutex scoped(mutex);
		list.remove(action);
	}
	delete action;
	action = NULL;
}

void OtaSyncToAsync::Run()
{
	OtaAction * action;

	while (running)
	{    
		action = queue.recv(); 

		os::ScopedMutex scoped(mutex);
		if (action->value == OtaAction::ACTION_CHECK_VERSION)
		{
			SkyOtaUrl url(URL_VERSION);
			CurlUsecaseOta usecase(CURL_USECASE_OTA_VERISON);
			m_prVersionConnection = new SkyOtaHttpConnection(url);
			m_prVersionConnection->setListener(action->listner);
			m_prVersionConnection->connect(usecase);
			DEBUGPRINT(DEBUG_INFO,("OtaAction: ACTION_CHECK_VERSION\n"));
		}else if (action->value == OtaAction::ACTION_START_DOWNLOADING_IMAGE)
		{
			clearOtaVersionConnection();

			SkyOTAInfo rOtaInfo = SkyUpgradeInfo::getInstance().getOtaInfo();
			SkyOtaUrl url(rOtaInfo.downloadUrl);
			CurlUsecaseOta usecase(CURL_USECASE_OTA_DATA);

			m_prDataConnection = new SkyOtaHttpConnection(url);
			m_prDataConnection->setListener(action->listner);
			m_prDataConnection->connect(usecase);
			DEBUGPRINT(DEBUG_INFO,("OtaAction: ACTION_START_DOWNLOADING_IMAGE\n"));
		}else if (action->value == OtaAction::ACTION_STOP_DOWNLOADING_IMAGE)
		{
			clearOtaVersionConnection();
			clearOtaDataConnection();
			deleteAction(action);
			DEBUGPRINT(DEBUG_INFO,("OtaAction: ACTION_STOP_DOWNLOADING_IMAGE\n"));
			break;
		}else if (action->value == OtaAction::ACTION_STOP_DOWNLOADING_FINISHED)
		{
			clearOtaDataConnection();
			DEBUGPRINT(DEBUG_INFO,("OtaAction: ACTION_STOP_DOWNLOADING_FINISHED\n"));
		}

		{
			deleteAction(action);
		}

		os::Thread::sleep(50);
	}
}


SkyOtaDownloader * SkyOtaDownloader::createDownloader(std::string mac,
	std::string model,
	std::string chip,
	std::string version)
{
	SkyOtaDownloader * downloader = new SkyOtaDownloader(mac,model,chip,version);
	DEBUGPRINT(DEBUG_INFO,("createDownloader()\n"));
	return downloader;
}

void SkyOtaDownloader::releaseDownloader(SkyOtaDownloader * downloader)
{
	if (downloader != NULL)
	{
		downloader->stopDownloading();
		DEBUGPRINT(DEBUG_INFO,("releaseDownloader()\n"));
		delete downloader;
		downloader = NULL;
	}
}

bool SkyOtaDownloader::isNewlyVersionExisted()
{
	m_checkNewlyVersion();

	bool isNewlyVersionExisted = m_fgNewlyVersionExisted;

	return isNewlyVersionExisted;
}

void SkyOtaDownloader::setDownloadPath(std::string path)
{
	SkyLocalMachineInfo::getInstance().setDownloadPath(path);
}

void SkyOtaDownloader::startDownloading()
{
	//OtaAction * action = new OtaAction();
	//action->value = OtaAction::ACTION_CHECK_VERSION;
	//action->listner = m_prInternalVersionListner;

	//OtaSyncToAsync::getInstance().addAction(action);

	OtaAction * action = new OtaAction();
	action->value = OtaAction::ACTION_START_DOWNLOADING_IMAGE;
	action->listner = m_prInternalDownloadingListner;

	OtaSyncToAsync::getInstance().addAction(action);
}

void SkyOtaDownloader::m_checkNewlyVersion()
{
	OtaAction * action = new OtaAction();
	action->value = OtaAction::ACTION_CHECK_VERSION;
	action->listner = m_prInternalVersionListner;

	OtaSyncToAsync::getInstance().addAction(action);

	sema_version.acquire();
}

void SkyOtaDownloader::finishDownloading()
{
	OtaAction * action = new OtaAction();
	action->value = OtaAction::ACTION_STOP_DOWNLOADING_FINISHED;
	action->listner = NULL;

	OtaSyncToAsync::getInstance().addAction(action);
}

void SkyOtaDownloader::stopDownloading()
{
	OtaSyncToAsync::getInstance().exitNow();
}


int SkyOtaDownloader::InternalVersionListner::connectAction(SkyOtaConnectionEvent & event)
{
	int action = event.getAction();
	SkyOtaDownloaderListener * prListner = m_prOwner->m_prOtaDownloaderListner;
	//if (prListner == NULL)
	//{
	//	return SkyOtaConnectionListener::Pause;
	//}

	if (action == SkyOtaConnectionEvent::Connected)
	{
		if (event.getErrorCode() == SkyOtaConnectionEvent::CodeOk)
		{
			SkyUpgradeInfo::getInstance().parseJsonData(event.getContent());
		}

		if(SkyUpgradeInfo::getInstance().getOtaInfo().downloadUrl.empty()){
			DEBUGPRINT(DEBUG_INFO,("InternalVersionListner() status:STATUS_NEW_VERSION_NOT_EXISTED\n"));
			if (prListner != NULL){
				prListner->notify(SkyOtaDownloaderListener::STATUS_NEW_VERSION_NOT_EXISTED, 0, 0);
			}
			
			m_prOwner->m_fgNewlyVersionExisted = false;
		}else{
			DEBUGPRINT(DEBUG_INFO,("InternalVersionListner() status:STATUS_NEW_VERSION_EXISTED\n"));
			if (prListner != NULL){
				long filesize = SkyUpgradeInfo::getInstance().getOtaInfo().filesize;
				long version = SkyUpgradeInfo::getInstance().getOtaInfo().version;
				int length = SkyUpgradeInfo::getInstance().getOtaInfo().fileName.length();

				char*  filename = ( char *)malloc(sizeof(char)*(length+1));
				strcpy(filename,(char *)SkyUpgradeInfo::getInstance().getOtaInfo().fileName.c_str());

				long long int lc_version =  Convert::atoll(SkyLocalMachineInfo::getInstance().getVersion().c_str());
				DEBUGPRINT(DEBUG_INFO,("InternalVersionListner() version:%d,lc_version:%d\n",version,lc_version));
				DEBUGPRINT(DEBUG_INFO,("InternalVersionListner() filename:%s \n",filename));
				if(version > lc_version){
					prListner->notify(SkyOtaDownloaderListener::STATUS_NEW_VERSION_EXISTED, filesize, (void*)filename);
				}else{
					prListner->notify(SkyOtaDownloaderListener::STATUS_NEW_VERSION_NOT_EXISTED, 0, 0);
				}
				free(filename);
				filename = NULL;
			}
			m_prOwner->m_fgNewlyVersionExisted = true;
		}

		m_prOwner->sema_version.release();
	}else if (event.getAction() == SkyOtaConnectionEvent::Disconnected)
	{
		// Disconnected normally, nothing needed to do. 
	}else if ((action == SkyOtaConnectionEvent::Abort) ||
		(event.getAction() == SkyOtaConnectionEvent::Fail))
	{
		prListner->notify(SkyOtaDownloaderListener::STATUS_DOWNLOAD_ERROR, 0, 0);
		m_prOwner->m_fgNewlyVersionExisted = false;
		m_prOwner->sema_version.release();
	}

	return SkyOtaConnectionListener::Ok;
}

int SkyOtaDownloader::InternalDownloadingListner::connectAction(SkyOtaConnectionEvent & event)
{
	SkyOtaDownloaderListener * prListner = m_prOwner->m_prOtaDownloaderListner;
	if (prListner == NULL)
	{
		return SkyOtaConnectionListener::Fail;
	}

	if (event.getAction() == SkyOtaConnectionEvent::Connected)
	{
		char * response = event.getContent();
		int length = event.getLength();
		if (event.getErrorCode() == SkyOtaConnectionEvent::CodeOk)
		{
			//TODO: ring buffer
			prListner->notify(SkyOtaDownloaderListener::STATUS_DOWNLOAD_PROGRESS, length, (void*)response);
		}
	}else if (event.getAction() == SkyOtaConnectionEvent::Disconnected)
	{
		if(SkyUpgradeInfo::getInstance().checkDownloadFileMD5Value() == true){
			DEBUGPRINT(DEBUG_INFO,("InternalDownloadingListner() status:STATUS_DOWNLOAD_FINISHED\n"));
			prListner->notify(SkyOtaDownloaderListener::STATUS_DOWNLOAD_FINISHED, 0, 0);
		}else{
			DEBUGPRINT(DEBUG_INFO,("InternalDownloadingListner() status:STATUS_DOWNLOAD_ERROR\n"));
			prListner->notify(SkyOtaDownloaderListener::STATUS_DOWNLOAD_ERROR, 0, 0);
		}
		m_prOwner->finishDownloading();
	}else if (event.getAction() == SkyOtaConnectionEvent::Abort)
	{
		DEBUGPRINT(DEBUG_INFO,("InternalDownloadingListner() status:STATUS_DOWNLOAD_ERROR\n"));
		prListner->notify(SkyOtaDownloaderListener::STATUS_DOWNLOAD_ERROR, 0, 0);
		m_prOwner->finishDownloading();
	}else if (event.getAction() == SkyOtaConnectionEvent::Fail)
	{
		DEBUGPRINT(DEBUG_INFO,("InternalDownloadingListner() status:STATUS_DOWNLOAD_ERROR\n"));
		prListner->notify(SkyOtaDownloaderListener::STATUS_DOWNLOAD_ERROR, 0, 0);
		m_prOwner->finishDownloading();
	}

	return SkyOtaConnectionListener::Ok;
}



