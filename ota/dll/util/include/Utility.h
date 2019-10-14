#ifndef _UTILITY_H
#define _UTILITY_H

#include <string>
#include "debug_log.h"
#ifndef _USE_LINUX
//#include <vld.h>
#endif


enum {
    ReturnOk                  =  ((int)  0)   ,       
    ReturnArg                 =  ((int) -1)   ,       
    ReturnHandle              =  ((int) -2)   ,       
    ReturnInval               =  ((int) -3)   ,       
    ReturnCore                =  ((int) -4)   ,       
    ReturnExist               =  ((int) -5)   ,       
    ReturnNoEntry             =  ((int) -6)   ,       
    ReturnNotDir              =  ((int) -7)   ,       
    ReturnIsDir               =  ((int) -8)   ,       
    ReturnDirNotEmpty         =  ((int) -9)   ,       
    ReturnNameTooLong         =  ((int) -10)  ,       
    ReturnFileTooLarge        =  ((int) -11)  ,       
    ReturnBusy                =  ((int) -12)  ,       
    ReturnEof                 =  ((int) -13)  ,       
    ReturnLockFail            =  ((int) -14)  ,       
    ReturnWouldBlock          =  ((int) -15)  ,       
    ReturnPermDeny            =  ((int) -16)  ,       
    ReturnAccess              =  ((int) -17)  ,       
    ReturnNotInit             =  ((int) -18)  ,       
    ReturnAlignment           =  ((int) -19)  ,       
    ReturnAsyncNotSupport     =  ((int) -20)  ,       
    ReturnDeviceError         =  ((int) -21)  ,       
    ReturnNoSuchDevice        =  ((int) -22)  ,       
    ReturnNotEnoughSpace      =  ((int) -23)  ,       
    ReturnFileSystemFull      =  ((int) -24)  ,       
    ReturnFileSystemCrash     =  ((int) -25)  ,       
    ReturnFileSystemOther     =  ((int) -26)  ,       
    ReturnCliError            =  ((int) -27)  ,       
    ReturnNoMbr               =  ((int) -28)  ,       
    ReturnEoc                 =  ((int) -29)  ,       
    ReturnFatError            =  ((int) -30)  ,       
    ReturnDeletingEntry       =  ((int) -31)  ,       
    ReturnOutOfRange          =  ((int) -32)  ,       
    ReturnOverValidRange      =  ((int) -33)  ,       
    ReturnAbnormalEnded       =  ((int) -34)  ,       
    ReturnBlankSector         =  ((int) -35)  ,       
    ReturnCmdTimeout          =  ((int) -36)  ,       
    ReturnError               =  ((int) -99)  ,       
    ReturnNotImplement        =  ((int) -100)         
};

namespace debug{
    class DebugInfo {
    public:
        static void log(const char *format, ...);   
        static void time(const char * log);

    public:
        static int initialize();
        static int uninitialize();
    };      
}

namespace util {
    class Convert{
    public:
    	static int atol(const char*value);
    	static long long int atoll(const char * value);
    	static bool is(const char * value);
    };

	class SkyOTAInfo{
	public:
		std::string msg;
		int code;
		int dependTcVersion;
		int initVersion;
		int id;
		int dependSysVersoin;
		std::string downloadUrl;
		long filesize;
		int marketTypeId;
		int packageId;
		int upgradeType;
		std::string md5;
		int versionType;
		int policyId;
		int isPin;
		std::string chip;
		std::string packageOwnerName;
		std::string fileName;
		std::string model;
		std::string publishTime;
		long version;
		std::string remark;
		long handlerFlag;
		long tvVersion;
	};

	
	class SkyUpgradeInfo{
	public:
		static SkyUpgradeInfo & getInstance(){
			static SkyUpgradeInfo instance;
			return instance;
		}
		int parseJsonData(char* jsondata);
		bool checkDownloadFileMD5Value();
		SkyOTAInfo & getOtaInfo();

	private:
		SkyOTAInfo mSkyOtaInfo;
	};

	class SkyLocalMachineInfo{
	public:
		static SkyLocalMachineInfo & getInstance(){
			static SkyLocalMachineInfo instance;
			return instance;
		}

		void set(std::string mac,
			std::string model,
			std::string chip,
			std::string version)
		{
				m_szMac = mac;
				m_szModel = model;
				m_szChip = chip;
				m_szVersion = version;
		}

		void setDownloadPath(std::string path){
			m_szPath = path;
		};

		std::string & getMac(){return m_szMac;}
		std::string & getModel(){return m_szModel;}
		std::string & getChip(){return m_szChip;}
		std::string & getVersion(){return m_szVersion;}
		std::string & getPath(){return m_szPath;}

	private:
		std::string m_szMac;
		std::string m_szModel;
		std::string m_szChip;
		std::string m_szVersion;
		std::string m_szPath;
	};
}

#endif

/*! @} */
