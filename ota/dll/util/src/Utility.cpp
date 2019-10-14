#include "Utility.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "json/json.h"
#include "json/reader.h"
#include "json/writer.h"
#include "json/value.h"
#include "Md5.h"

//#pragma comment(lib,"json_vc71_libmtd.lib")


using namespace debug;
using namespace util;

#define Utility_DEBUG_OUTPUT_LOG 0

static time_t current = 0;
static time_t base = 0;



int debug::DebugInfo::initialize() 
{
    base = ::time(&base);
    return 0;
}

int debug::DebugInfo::uninitialize() 
{
    return 0;
}


void debug::DebugInfo::log(const char *format, ...) 
{
#if Utility_DEBUG_OUTPUT_LOG
    va_list va;
    va_start(va, format);
    vfprintf(stdout, format, va);    
    va_end(va);
    fflush(stdout);
#endif
}

void debug::DebugInfo::time(const char *log) 
{
    current = ::time(&current);
#if Utility_DEBUG_OUTPUT_LOG
    debug::DebugInfo::log(" %s : %f \n", log, difftime(current, base));    
#endif
}

int util::Convert::atol(const char * valueString)
{
    int value;
    sscanf(valueString, "%d", &value);
    return value;    
}

long long int util::Convert::atoll(const char * valueString)
{
    long long int value;
    sscanf(valueString, "%lld", &value);
    return value; 
}

bool util::Convert::is(const char* valueString)
{
    if (strcmp(valueString, "true") == 0)
    {
        return true;
    }    
    return false; 
}

int SkyUpgradeInfo::parseJsonData(char* jsondata){
	int ret = -1;
	Json::Reader reader;   
	Json::Value value;

	char* jsonData = (char*)malloc(sizeof(char) * (strlen(jsondata)+1));
	strcpy(jsonData,jsondata);

	DEBUGPRINT(DEBUG_INFO,("parseJsonData() json data:%s\n",jsondata));

	if(reader.parse(jsonData, jsonData+ (strlen(jsonData)+1), value)){
		if(value["code"].asInt() == 1){
			mSkyOtaInfo.msg = value["msg"].asString();
			mSkyOtaInfo.code = value["code"].asInt();
			mSkyOtaInfo.dependTcVersion = value["data"]["dependTcVersion"].asInt();
			mSkyOtaInfo.initVersion = value["data"]["initVersion"].asInt();
			mSkyOtaInfo.id = value["data"]["id"].asInt();
			mSkyOtaInfo.downloadUrl = value["data"]["downloadUrl"].asString();
			mSkyOtaInfo.filesize = value["data"]["filesize"].asDouble();
			mSkyOtaInfo.marketTypeId = value["data"]["marketTypeId"].asInt();
			mSkyOtaInfo.packageId = value["data"]["packageId"].asInt();
			mSkyOtaInfo.upgradeType = value["data"]["upgradeType"].asInt();
			mSkyOtaInfo.md5 = value["data"]["md5"].asString();
			mSkyOtaInfo.versionType = value["data"]["versionType"].asInt();
			mSkyOtaInfo.policyId = value["data"]["policyId"].asInt();
			mSkyOtaInfo.isPin = value["data"]["isPin"].asInt();
			mSkyOtaInfo.chip = value["data"]["chip"].asString();
			mSkyOtaInfo.packageOwnerName = value["data"]["packageOwnerName"].asString();
			mSkyOtaInfo.fileName = value["data"]["fileName"].asString();
			mSkyOtaInfo.model = value["data"]["model"].asString();
			mSkyOtaInfo.publishTime = value["data"]["publishTime"].asString();
			mSkyOtaInfo.version = value["data"]["version"].asDouble();
			mSkyOtaInfo.remark = value["data"]["remark"].asString();
			mSkyOtaInfo.handlerFlag = value["data"]["handlerFlag"].asDouble();
			mSkyOtaInfo.tvVersion = value["data"]["tvVersion"].asDouble();

			ret = 0;
		}
	}
	free(jsonData);
	jsonData = NULL;
	return ret;
}

bool SkyUpgradeInfo:: checkDownloadFileMD5Value()
{
	bool ret = false;
	long filesize = 0;
	MD5_CTX md5;
	//unsigned char * buffer;

	FILE *fd = fopen( SkyLocalMachineInfo::getInstance().getPath().c_str(),"rb");
	printf("\nsource file path:%s \n",SkyLocalMachineInfo::getInstance().getPath().c_str());

	if(fd){
		fseek(fd,0,SEEK_END);

		filesize = ftell(fd); 
		fseek(fd,0,SEEK_SET);
	}else{
		return ret;
	}

	if(filesize != getOtaInfo().filesize){
		ret = false;
		fclose(fd);
		return ret;
	}

	unsigned char *digest =  (unsigned char *)malloc(sizeof(unsigned char)*17);
	unsigned char* value = (unsigned char *)malloc(sizeof(unsigned char)*33);

	memset(value,0,sizeof(unsigned char)*33);
	memset(digest,0,sizeof(unsigned char)*17);

	//buffer =  (unsigned char *)malloc(sizeof(unsigned char)*filesize);

	//fread(buffer,1,filesize,fd);

	MD5Init(&md5);
	DEBUGPRINT(DEBUG_INFO,("MD5UpdateFile do MD5!!!\n"));
	//MD5Update(&md5,buffer,filesize);
	MD5UpdateFile(&md5,SkyLocalMachineInfo::getInstance().getPath().c_str(),filesize);
	MD5Final(&md5,digest);

	fclose(fd);
	//free(buffer);
	//buffer = NULL;

	charArrayToHex(value,digest);
	DEBUGPRINT(DEBUG_INFO,("source file MD5 value:%s \n",value));
	DEBUGPRINT(DEBUG_INFO,("Http file MD5 value:%s \n",getOtaInfo().md5.c_str()));
	if(strncmp((const char*)value,getOtaInfo().md5.c_str(),32) == 0){
		ret = true;
	}
	
	free(value);
	value = NULL;
	free(digest);
	digest = NULL;

	return ret;
}

SkyOTAInfo & SkyUpgradeInfo::getOtaInfo()
{
	return mSkyOtaInfo;
}



