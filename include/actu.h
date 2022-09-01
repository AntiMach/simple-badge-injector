#ifndef ACTU_H
#define ACTU_H

#include <3ds.h>
#include <stdio.h>

Result actInit(void);
Result actExit(void);
Result actuInit(u32 sdkVersion, u32 unknown, Handle handle);
Result actuGetAccountDataBlock(u32 unknown, u32 size, u32 blockId, void* output);

#endif