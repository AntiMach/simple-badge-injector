#ifndef EXT_H
#define EXT_H

#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

Result createExtSaveData(u32 extdataID);
Result deleteExtSaveData(u32 extdataID);

#endif
