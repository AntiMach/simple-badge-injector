#include "badge.h"

static FS_Archive badgeArchive = 0;

const u32 EXTDATA_LOW[] = {MEDIATYPE_SD, BADGE_ARCHIVE, 0};
const FS_Path EXTDATA_PATH = (FS_Path){PATH_BINARY, 0xC, EXTDATA_LOW};

// SD card prepartion

void ensureDir(char *dir_name)
{
    DIR *dir = opendir(dir_name);

    if (dir)
        closedir(dir);
    else
        mkdir(dir_name, 0777);
}

// Badge archive operations

Result openBadgeArchive()
{
    return FSUSER_OpenArchive(&badgeArchive, ARCHIVE_EXTDATA, EXTDATA_PATH);
}

Result closeBadgeArchive()
{
    return FSUSER_CloseArchive(badgeArchive);
}

Result ensureBadgeArchive()
{
    Result res;

    // Try to open archive
    res = openBadgeArchive();
    if (!R_FAILED(res))
        return res;

    if (R_SUMMARY(res) != RS_NOTFOUND)
        return res;

    // Failed to open because it doesn't exist, create it.
    res = createExtSaveData(BADGE_ARCHIVE);
    if (R_FAILED(res))
        return res;

    // Try opening again
    return openBadgeArchive();
}

Result deleteBadgeArchive()
{
    return deleteExtSaveData(BADGE_ARCHIVE);
}

Result checkBadgeArchive()
{
    Result res = openBadgeArchive();
    closeBadgeArchive();
    return res;
}

Result writeToBadgeArchive(FS_Path path, size_t size, void *data)
{
    Result res;
    Handle extfile;

    FSUSER_CreateFile(badgeArchive, path, 0, size);

    res = FSUSER_OpenFile(&extfile, badgeArchive, path, FS_OPEN_WRITE, 0);
    if (R_FAILED(res))
        return res;

    res = FSFILE_Write(extfile, NULL, 0, data, size, FS_WRITE_FLUSH);
    if (R_FAILED(res))
        return res;

    res = FSFILE_Close(extfile);
    if (R_FAILED(res))
        return res;

    return 0;
}

Result readFromBadgeArchive(FS_Path path, size_t size, void *data)
{
    Result res;
    Handle extfile;

    res = FSUSER_OpenFile(&extfile, badgeArchive, path, FS_OPEN_READ, 0);
    if (R_FAILED(res))
        return res;

    res = FSFILE_Read(extfile, NULL, 0, data, size);
    if (R_FAILED(res))
        return res;

    res = FSFILE_Close(extfile);
    if (R_FAILED(res))
        return res;

    return 0;
}

// Badge data operations

BadgeBuffer initBadgeData()
{
    return malloc(sizeof(BadgeBufferStruct));
}

void destroyBadgeData(BadgeBuffer buf)
{
    free(buf);
}

// Badge data injection/dump

Result writeBadgeDataToArchive(BadgeBuffer buf, int mngFileOnly)
{
    Result res;

    // Open archive
    res = ensureBadgeArchive();
    if (R_FAILED(res))
    {
        DEBUG_ERROR("Failed to open ExtData archive (%08lX)", res);
        return res;
    }

    // Read data from archive
    if (!mngFileOnly)
    {
        res = writeToBadgeArchive(fsMakePath(PATH_ASCII, BADGE_DATA), BADGE_DATA_SIZE, buf->data);
        if (R_FAILED(res))
        {
            DEBUG_ERROR("Failed to write to ExtData archive (%08lX)", res);
            goto cleanupArchive;
        }
    }

    res = writeToBadgeArchive(fsMakePath(PATH_ASCII, BADGE_MNG), BADGE_MNG_SIZE, &buf->mngFile);
    if (R_FAILED(res))
    {
        DEBUG_ERROR("Failed to write to ExtData archive (%08lX)", res);
        goto cleanupArchive;
    }

cleanupArchive:
    closeBadgeArchive();

    return res;
}

Result readBadgeDataFromArchive(BadgeBuffer buf, int mngFileOnly)
{
    Result res;

    // Open archive
    res = openBadgeArchive();
    if (R_FAILED(res))
    {
        if (R_SUMMARY(res) == RS_NOTFOUND)
            DEBUG_ERROR("ExtData archive does not exist");
        else
            DEBUG_ERROR("Failed to open ExtData archive (%08lX)", res);

        return res;
    }

    // Read data from archive
    if (!mngFileOnly)
    {
        res = readFromBadgeArchive(fsMakePath(PATH_ASCII, BADGE_DATA), BADGE_DATA_SIZE, buf->data);
        if (R_FAILED(res))
        {
            DEBUG_ERROR("Failed to read from ExtData archive (%08lX)", res);
            goto cleanupArchive;
        }
    }

    res = readFromBadgeArchive(fsMakePath(PATH_ASCII, BADGE_MNG), BADGE_MNG_SIZE, &buf->mngFile);
    if (R_FAILED(res))
    {
        DEBUG_ERROR("Failed to read from ExtData archive (%08lX)", res);
        goto cleanupArchive;
    }

cleanupArchive:
    closeBadgeArchive();

    return res;
}

int writeBadgeDataToSD(BadgeBuffer buf)
{
    FILE *file;

    ensureDir(APP_ROOT);
    ensureDir(DUMPED_DIR);

    file = fopen(DUMPED_BADGE_DATA, "w");
    if (!file)
    {
        DEBUG_ERROR("Failed to open file " DUMPED_BADGE_DATA);
        return 1;
    }

    fwrite(buf->data, sizeof(u8), BADGE_DATA_SIZE, file);
    fclose(file);

    file = fopen(DUMPED_BADGE_MNG, "w");
    if (!file)
    {
        DEBUG_ERROR("Failed to open file " DUMPED_BADGE_MNG);
        return 1;
    }

    fwrite(&buf->mngFile, sizeof(u8), BADGE_MNG_SIZE, file);
    fclose(file);

    return 0;
}

int readBadgeDataFromSD(BadgeBuffer buf)
{
    FILE *file;

    ensureDir(APP_ROOT);

    file = fopen(APP_BADGE_DATA, "r");
    if (!file)
    {
        DEBUG_ERROR("Failed to open file " APP_BADGE_DATA);
        return 1;
    }

    fread(buf->data, sizeof(u8), BADGE_DATA_SIZE, file);
    fclose(file);

    file = fopen(APP_BADGE_MNG, "r");
    if (!file)
    {
        DEBUG_ERROR("Failed to open file " APP_BADGE_MNG);
        return 1;
    }

    fread(&buf->mngFile, sizeof(u8), BADGE_MNG_SIZE, file);
    fclose(file);

    return 0;
}
