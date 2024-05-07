// globals.h

#ifndef GLOBALS_H
#define GLOBALS_H

#include "Cache.h"
#include "Speculator.h"
#include <sys/types.h>
#include <map>

extern std::map<int,  char*> cache;
extern Speculator* speculator;
void initializeGlobalSharedObject() ;


#endif
