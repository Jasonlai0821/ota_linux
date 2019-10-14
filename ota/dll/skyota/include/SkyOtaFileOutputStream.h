
#ifndef _SKY_OTA_FILE_OUTPUT_STREAM_H
#define _SKY_OTA_FILE_OUTPUT_STREAM_H

#include <memory>
#include <string>
#include <vector>

#include "SkyOtaOutputStream.h"

namespace skyota {    
    namespace io {
        class SkyOtaFileOutputStream : skyota::io::SkyOtaOutputStream
        {
        public:
            SkyOtaFileOutputStream(const std::string & filename);  
            ~SkyOtaFileOutputStream();
            int close();
            int write(char * buffer, int len);
        private:
            class Implementation;
            Implementation * impl;
        };
    }
}




#endif

