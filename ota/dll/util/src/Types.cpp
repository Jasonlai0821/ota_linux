#include "Types.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>


using namespace debug;

static time_t current = 0;
static time_t base = 0;


#if 0
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
    va_list va;
    va_start(va, format);
    vfprintf(stdout, format, va);    
    va_end(va);
    fflush(stdout);
}

void debug::DebugInfo::time(const char *log) 
{
    current = ::time(&current);
    debug::DebugInfo::log(" %s : %f \n", log, difftime(current, base));    
}
#endif

