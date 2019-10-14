
#ifndef _TYPES_H
#define _TYPES_H

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

#endif

/*! @} */
