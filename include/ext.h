#ifndef EXT_H
#define EXT_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <3ds.h>

Result createExtSaveData(u32 extdataID);
Result deleteExtSaveData(u32 extdataID);

#endif
