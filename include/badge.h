#ifndef BADGE_H
#define BADGE_H

#include <3ds.h>
#include <stdio.h>
#include <dirent.h>

#include "ext.h"
#include "console.h"

#define BADGE_DATA_SIZE 0xF4DF80
#define BADGE_MNG_SIZE (sizeof(BadgeMngFile))

#define BADGE_ARCHIVE 0x14d1

#define BADGE_DATA "/BadgeData.dat"
#define BADGE_MNG "/BadgeMngFile.dat"

#define APP_BADGE_DATA APP_ROOT BADGE_DATA
#define APP_BADGE_MNG APP_ROOT BADGE_MNG

#define DUMPED_DIR APP_ROOT "/Dumped"
#define DUMPED_BADGE_DATA DUMPED_DIR BADGE_DATA
#define DUMPED_BADGE_MNG DUMPED_DIR BADGE_MNG

typedef struct
{
    u32 zeros;
    u32 badgeSets;
    u32 uniqueBadges;
    u32 placedBadges;
    u32 selectedBadgeSet;
    u32 selectedBadgeColumn;
    u32 totalBadges;
    u32 NNID;
    u8 unknown[0x338];
    u8 usedBadgeSlots[0x80];
    u8 usedBadgeSetSlots[0x10];
    u8 badgeInfoEntries[1000 * 0x28];
    u8 badgeSetInfoEntries[100 * 0x30];
    u8 badgeLayoutSlotEntries[360 * 0x18];
} BadgeMngFile;

typedef struct
{
    u8 data[BADGE_DATA_SIZE];
    BadgeMngFile mngFile;
} BadgeBufferStruct, *BadgeBuffer;

Result checkBadgeArchive();
Result deleteBadgeArchive();

BadgeBuffer initBadgeData();
void destroyBadgeData(BadgeBuffer buf);

Result readBadgeDataFromArchive(BadgeBuffer buf);
Result writeBadgeDataToArchive(BadgeBuffer buf);
int writeBadgeDataToSD(BadgeBuffer buf);
int readBadgeDataFromSD(BadgeBuffer buf);

#endif
