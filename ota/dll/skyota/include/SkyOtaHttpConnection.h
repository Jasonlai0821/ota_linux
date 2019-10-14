
#ifndef _SKY_OTA_HTTP_CONNECTION_H
#define _SKY_OTA_HTTP_CONNECTION_H

#include <memory>
#include <string>

#include "SkyOtaConnection.h"

namespace skyota {
    namespace net {
        class SkyOtaHttpConnection : public skyota::net::SkyOtaConnection
        {
        public:
            SkyOtaHttpConnection();
            SkyOtaHttpConnection(skyota::io::SkyOtaUrl & url);
            virtual ~SkyOtaHttpConnection();

            void connect(CurlUsecaseOta & usecase);
            void disconnect();
            void range(long long int start, long long int end);

            bool suspensive();
            int suspend();
            int resume();
            
        protected:
            class Implementation;
            Implementation * impl;
        private:
            long long int start;
            long long int end;
        };
    }    
}




#endif

