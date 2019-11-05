#include "SkyOTAUpgrade.h"
#include "SkyOtaDownloader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debug_log.h"

using namespace skyota::net;
using namespace skyota::io;
using namespace skyota::facade;

static SkyOtaDownloader* prDownloader = NULL;

static int s_i4VersionHasBeenChecked = -1;
/*
-1: not checked
0: newly version existed
1: no newly version existed
*/


typedef void (*cb_notify) (int status, int param1, void* param2);
class MyDownloaderListner : public skyota::facade::SkyOtaDownloaderListener
{
public:
	void notify(int status, int param1, void* param2);

	void callback_notify(void (*cb_notify) (int , int , void* ),int status,int param1,void* param2);
public:
	void (*callback) (int , int , void* );
};

void MyDownloaderListner::callback_notify(void (*cb_notify) (int , int , void* ),int status,int param1,void* param2){
	cb_notify(status,param1,param2);
}

void MyDownloaderListner::notify(int status, int param1, void* param2)
{
	//printf("notify() status:%d, param1:%d\n",status,param1);

	if (status == SkyOtaDownloaderListener::STATUS_NEW_VERSION_NOT_EXISTED)
	{
		callback_notify(callback,status,param1,param2);
	} else if (status == SkyOtaDownloaderListener::STATUS_NEW_VERSION_EXISTED)
	{
		callback_notify(callback,status,param1,param2);
	} else if (status == SkyOtaDownloaderListener::STATUS_DOWNLOAD_STARTED)
	{
		callback_notify(callback,status,param1,param2);
	} else if (status == SkyOtaDownloaderListener::STATUS_DOWNLOAD_PROGRESS)
	{
		callback_notify(callback,status,param1,param2);
	}else if (status == SkyOtaDownloaderListener::STATUS_DOWNLOAD_FINISHED)
	{
		callback_notify(callback,status,param1,param2);
	}else if (status == SkyOtaDownloaderListener::STATUS_DOWNLOAD_ERROR)
	{
		callback_notify(callback,status,param1,param2);
	}
}


void init(SkyOtaRequestInfo *info, unsigned char *path, int path_size)
{
	std::string cs_MAC = (char*)info->mac;
	std::string cs_model = (char*)info->model;
	std::string cs_chip = (char*)info->chip;
	std::string cs_lcversion = (char*)info->lc_version;

	printf("\t**********  SKYWORTH OTA Version:0.1.4  **********\n");

	printf("init()\n");
	if(prDownloader != NULL){
		deInit();
	}
	prDownloader = SkyOtaDownloader::createDownloader(cs_MAC, cs_model, cs_chip, cs_lcversion);

	MyDownloaderListner *Listner = new MyDownloaderListner();
	//set nofity call function
	Listner->callback = info->cb_notify;

	prDownloader->setDownloaderListner(Listner);

	char*  savePath = ( char *)malloc(sizeof(char)*path_size);
	strcpy(savePath,(char *)path);

	std::string cs_path = (char*)savePath;
	prDownloader->setDownloadPath(cs_path);
	free(savePath);
	savePath = NULL;
}

void deInit()
{
	printf("deInit()\n");
	SkyOtaDownloader::releaseDownloader(prDownloader);
}

int isNewlyVersionExisted()
{
	int ret = 0;
	/*if(s_i4VersionHasBeenChecked != -1)
	{
		ret = (s_i4VersionHasBeenChecked == 0) ? true : false;
		return ret;
	}*/

	bool isNewlyVersionExisted = prDownloader->isNewlyVersionExisted();
	printf("isNewlyVersionExisted() is latest Version Existed : %d\n",isNewlyVersionExisted);
	if (isNewlyVersionExisted)
	{
		//s_i4VersionHasBeenChecked = 0;
		ret = 1;
		return ret;
	} 
	else
	{
		//s_i4VersionHasBeenChecked = 1;
		ret = 0;
		return ret;
	}
	ret = 0;
	return ret;
}

void setDownloadPath(unsigned char *path, int path_size)
{
	char*  savePath = ( char *)malloc(sizeof(char)*path_size);
	char* c_path = (char*)path;
	strcpy(savePath,c_path);

	std::string cs_path = (char*)savePath;
	printf("setDownloadPath()\n");
	prDownloader->setDownloadPath(cs_path);
	free(savePath);
	savePath = NULL;
}

void startDownloading()
{
	if (s_i4VersionHasBeenChecked == 1)
	{
		// no newly version existed
		printf("startDownloading() no newly version existed\n");
		return;
	}

	printf("startDownloading() s_i4VersionHasBeenChecked:%d\n",s_i4VersionHasBeenChecked);
	if (s_i4VersionHasBeenChecked == -1)
	{
		bool isNewlyVersionExisted = prDownloader->isNewlyVersionExisted();
		printf("startDownloading() isNewlyVersionExisted = %d\n",isNewlyVersionExisted);
		if (isNewlyVersionExisted)
		{
			//s_i4VersionHasBeenChecked = 0;
		} 
		else
		{
			//s_i4VersionHasBeenChecked = 1;
			return ;
		}
	}

	printf("startDownloading()\n");
	
	prDownloader->startDownloading();

	// TODO: below will be removed
	
}

void stopDownloading()
{
	printf("stopDownloading()\n");
	prDownloader->stopDownloading();
}