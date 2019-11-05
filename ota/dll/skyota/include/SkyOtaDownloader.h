#ifndef _SKY_OTA_DOWNLOADED_H
#define _SKY_OTA_DOWNLOADED_H

#include "SkyOtaConnection.h"
#include "SkyOtaHttpConnection.h"
#include "Thread.h"
#include "Mutex.h"
#include "MessageQueue.h"
#include "Utility.h"
#include "PthreadScopedMutex.h"

#include <string>

using namespace skyota::net;
using namespace skyota::io;
using namespace os;

namespace skyota {
	namespace facade {

		class SkyOtaDownloaderListener {
			public:
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

			virtual ~SkyOtaDownloaderListener(){};

			virtual void notify(int status, int param1, void* param2)=0;
		};   

		class SkyOtaDownloader
		{
		public:
		virtual ~SkyOtaDownloader(void);

		public:
			static SkyOtaDownloader * createDownloader(std::string mac,
				std::string model,
				std::string chip,
				std::string version);

			static void releaseDownloader(SkyOtaDownloader * downloader);

			bool isNewlyVersionExisted();
			void setDownloadPath(std::string path);
			void setDownloaderListner(SkyOtaDownloaderListener * listner){
				m_prOtaDownloaderListner = listner;
			}
			void startDownloading();
			void stopDownloading();
			void finishDownloading();

		private:
			SkyOtaDownloader(std::string mac,
				std::string model,
				std::string chip,
				std::string version);

			SkyOtaDownloaderListener * m_prOtaDownloaderListner;

			class InternalVersionListner : public SkyOtaConnectionListener
			{
			public:
				InternalVersionListner(SkyOtaDownloader * owner){
					m_prOwner = owner;
				}
				virtual int connectAction(SkyOtaConnectionEvent & event);
			private:
				SkyOtaDownloader * m_prOwner;
			};
			InternalVersionListner * m_prInternalVersionListner;

			class InternalDownloadingListner : public SkyOtaConnectionListener
			{
			public:
				InternalDownloadingListner(SkyOtaDownloader * owner){
					m_prOwner = owner;
				}
				virtual int connectAction(SkyOtaConnectionEvent & event);
			private:
				SkyOtaDownloader * m_prOwner;
			};
			InternalDownloadingListner * m_prInternalDownloadingListner;

		private:
			os::Semaphore sema_version;
			bool m_fgNewlyVersionExisted; 

			void m_checkNewlyVersion();
		};
	}
}

#endif

