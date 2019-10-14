#ifndef _X_OTAUPGRADE_H
#define _X_OTAUPGRADE_H
//#include <stdio>

#ifdef __cplusplus
extern "C"{
#endif


// newly version not existed
static const int STATUS_NEW_VERSION_NOT_EXISTED = 0;

// newly version existed
static const int STATUS_NEW_VERSION_EXISTED = 1;

// downloading started
static const int STATUS_DOWNLOAD_STARTED = 2;

// downloading is on progress and param1 is the actually downloaded progress.
static const int STATUS_DOWNLOAD_PROGRESS = 3;

// downloading finished successfully
static const int STATUS_DOWNLOAD_FINISHED = 4;

// error happened when downloading, and param1 is the detailed
static const int STATUS_DOWNLOAD_ERROR = 5;

//status: downloaded state,param1 is the actually downloaded progress.	
//if status is STATUS_NEW_VERSION_EXISTED,param1 will be  filesize, param2 will be filename address.
//filename:»úÐ¾_»úÐÍ_V°æ±¾_ÇøÓòÂë_LinyPlayer°æ±¾ºÅ.zip;
//ÀýÈç£ºMT7688_SkySoundBox_SBO1O_V018.007.040_9_3.6.5903.0023.6.120.1.11.zip
typedef void (*cb_notify) (int status, int param1, int param2);

typedef struct{
	unsigned char lc_version[16];			//format:"018006015",  for example data:2018-06-15

	unsigned char mac[16];						//format:"B0F1ECFCC2F7"

	unsigned char model[32];					//format:"SBL3S"

	unsigned char chip[32];						//format:"2C01"

	void (*cb_notify) (int status, int param1, int param2);

}SkyOtaRequestInfo;

void init(SkyOtaRequestInfo *info,unsigned char *path, int path_size);

/* to judge whether newly version is existed or not ;0: not exist,1: exist*/
int isNewlyVersionExisted();

/* start download */
void startDownloading();

/* stop download */
void stopDownloading();

void deInit();

#ifdef __cplusplus
}
#endif

#endif