
#ifndef _SKY_OTA_CONNECTION_H
#define _SKY_OTA_CONNECTION_H

#include <memory>
#include <string>

#include "SkyOtaUrl.h"


namespace skyota {
    namespace net {
        class SkyOtaConnectionEvent
        {
        public:
            static const int Connected       = 0;
            static const int Disconnected    = 1;
            static const int Abort           = 2;
            static const int Fail            = 3;

            
            static const int CodeOk              = 0;
            static const int CodeFail            = 1;
            static const int CodeClientError     = 2;
            static const int CodeServerError     = 3;


            SkyOtaConnectionEvent(
                int action,
                int errorCode, 
                char * content, 
                int length);

            ~SkyOtaConnectionEvent();

            int    getAction(){return action;};
            int    getErrorCode(){return errorCode;};
            char * getContent(){return content;};
            int    getLength(){return length;};
            
        private:
            int action;
            int errorCode;
            char * content;
            int length;
        };


        class SkyOtaConnectionListener {
        public:
            static const int Ok = 0;
            static const int Fail = 1;
            static const int Pause = 2;

            virtual ~SkyOtaConnectionListener(){};

            virtual int connectAction(SkyOtaConnectionEvent & event)=0;
        };    

        class SkyOtaConnection
        {
        public:
            static const std::string OutputPath;

            virtual ~SkyOtaConnection(){};

            virtual void range(long long int start, long long int end)=0;
            virtual void connect(CurlUsecaseOta & usecase)=0;
            virtual void disconnect()=0;

            virtual bool suspensive()=0;
            virtual int suspend()=0;
            virtual int resume()=0;
            
            void setListener(SkyOtaConnectionListener * listener);
            void resetListener();

        private:
        protected:
            skyota::io::SkyOtaUrl url;
            SkyOtaConnectionListener * listener;
        };
    }    
}

#endif

