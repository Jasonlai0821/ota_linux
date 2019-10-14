#ifndef _SKY_OTA_CURL_H
#define _SKY_OTA_CURL_H

#include "SkyOtaUrl.h"
#include "Semaphore.h"
#include "ScopedMutex.h"
#include "PthreadScopedMutex.h"
#include <errno.h>

namespace skyota {
    namespace net {

        enum SkyOtaCurlStatusCode {
			HttpUnknown = -1,
            HttpContinue = 100,
            HttpSwitchingProtocols = 101,
            HttpOK = 200,
            HttpCreated = 201,
            HttpAccepted = 202,
            HttpNonAuthoritativeInformation = 203,
            HttpNoContent = 204,
            HttpResetContent = 205,
            HttpPartialContent = 206,
            HttpMultipleChoices = 300,
            HttpMovedPermanently = 301,
            HttpFound = 302,
            HttpSeeOther = 303,
            HttpNotModified = 304,
            HttpUseProxy = 305, 
            HttpTemporaryRedirect = 307,
            HttpBadRequest = 400,
            HttpUnauthorized = 401,
            HttpPaymentRequired = 402,
            HttpForbidden = 403,
            HttpNotFound = 404,
            HttpMethodNotAllowed = 405,
            HttpNotAcceptable = 406,
            HttpProxyAuthenticationRequired = 407,
            HttpRequestTimeout = 408,
            HttpConflict  = 409,
            HttpGone      = 410,
            HttpLengthRequired = 411,
            HttpPreconditionFailed = 412,
            HttpRequestEntityTooLarge = 413,
            HttpRequestURITooLong = 414,
            HttpUnsupportedMediaType = 415,
            HttpRequestedRangeNotSatisfiable = 416,
            HttpExpectationFailed = 417,
            HttpInternalServerError = 500,
            HttpNotImplemented = 501,
            HttpBadGateway = 502,
            HttpServiceUnavailable = 503,
            HttpGatewayTimeout = 504,
            HttpHTTPVersionNotSupported = 505
        };

        class SkyOtaCurl{
        public: 
            static const int Ok    = 0;
            static const int End   = 1;
            static const int Abort = 2;
            static const int Fail = 3;

            static const int CurlOk      = 0;
            static const int CurlFail    = 1;
            static const int CurlPause   = 2;
            static const int CurlResume  = 3;            

            static std::string escape(const std::string & value);
            static std::string unescape(const std::string & value);

            SkyOtaCurl();
            SkyOtaCurl(
                skyota::io::SkyOtaUrl & url,
                long long int & start, 
                long long int & end, 
				CurlUsecaseOta & type);
            virtual ~SkyOtaCurl();
            void cancel();  

            bool suspensive(){return pause;};
            int suspend();
            int resume();

            skyota::io::SkyOtaUrl & getUrl(){return url;};
            long long int                   & getStart(){return start;};
            long long int                   & getEnd(){return end;};
			CurlUsecaseOta &getUsecase(){return usecase;}

            virtual int notify(int action, SkyOtaCurlStatusCode code, char * data, int size)=0;
        protected:              

        private: 
            skyota::io::SkyOtaUrl url;
            long long int start;
            long long int end; 
			CurlUsecaseOta usecase;
            bool pause;
            int handle;
            class TaskItem;
            friend class TaskItem;
            class TaskManager;
        };

    }
}

#endif

