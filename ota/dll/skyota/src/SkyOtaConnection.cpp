#include "SkyOtaConnection.h"

using namespace skyota::net;
using namespace skyota::io;

SkyOtaConnectionEvent::SkyOtaConnectionEvent(int action,
                                                 int errorCode, 
                                                 char * content, 
                                                 int length)
                                                 :action(action), errorCode(errorCode), content(content), length(length)
{

}

SkyOtaConnectionEvent::~SkyOtaConnectionEvent()
{

}

const std::string SkyOtaConnection::OutputPath = "/mnt/tmp/output";


void SkyOtaConnection::setListener(SkyOtaConnectionListener * listener)
{
    this->listener = listener;
}

void SkyOtaConnection::resetListener()
{
    listener = NULL;
}

