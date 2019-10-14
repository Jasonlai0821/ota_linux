
#ifndef _SKY_OTA_URL_H
#define _SKY_OTA_URL_H

#include <memory>
#include <string>


enum CurlUsecaseOta{
	CURL_USECASE_OTA_VERISON = ((int)0),
	CURL_USECASE_OTA_DATA = (int(1))
};

namespace skyota {

    namespace io {

        class SkyOtaUrl
        {
        public:
            static const std::string Http  ;
            static const std::string File  ;
            static const std::string Rtsp  ;
            static const std::string Https ;

            SkyOtaUrl();
            SkyOtaUrl(std::string & url);
            SkyOtaUrl(std::string & protocol, 
                        std::string & host,
                        int         & port,  
                        std::string & file);
            ~SkyOtaUrl();

            std::string & getUrl(){return url;};
            std::string & getProtocol(){return protocol;};
            std::string & getHost(){return host;};
            int         & getPort(){return port;};
            std::string & getFile(){return file;};
        private:
            std::string url;
            std::string protocol;
            std::string host;
            int port;
            std::string file;
        };
    }    
}




#endif

