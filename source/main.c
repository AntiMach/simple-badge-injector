#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <dirent.h>
#include <math.h>
#include <sys/stat.h>

#include "ext.h"
#include "actu.h"
#include "console.h"
#include "defs.h"

Result setupExtdata()
{
    FS_Archive extdata_archive;

    Result ret = FSUSER_OpenArchive(&extdata_archive, ARCHIVE_EXTDATA, EXTDATA_PATH);
    FSUSER_CloseArchive(extdata_archive);

    if (!ret)
        return EXTDATA_EXIST;

    return createExtSaveData(0x14d1) ? EXTDATA_NOT_CREATE : EXTDATA_CREATE;
}

Result deleteExtdata()
{
    FS_Archive extdata_archive;

    Result ret = FSUSER_OpenArchive(&extdata_archive, ARCHIVE_EXTDATA, EXTDATA_PATH);
    FSUSER_CloseArchive(extdata_archive);

    if (ret)
        return EXTDATA_NOT_EXIST;

    printf(FG(YELLOW) "Are you sure you want to delete it?\n");
    printf(FG(BLUE) "Y = Yes\n");
    printf(FG(RED) "X = No\n");

    while (1)
    {
        hidScanInput();
        u32 option = hidKeysDown();

        if (option & KEY_X)
            return OPTION_CANCEL;

        if (option & KEY_Y)
            return deleteExtSaveData(0x14d1) ? EXTDATA_NOT_DELETE : EXTDATA_DELETE;

        gspWaitForVBlank();
    }
}

void ensure_dir(char* dir_name) {
    DIR *dir = opendir(dir_name);

    if (dir)
        closedir(dir);
    else
        mkdir(dir_name, 0777);
}

Result writeFilesToExtdata()
{
    Handle extfile;
    FS_Archive archive;
    Result ret;

    u8 *badgeDataBuf = (u8 *)malloc(BADGE_DATA_SIZE);
    if (!badgeDataBuf)
        return NO_MEMORY;

    u8 *badgeMngFileBuf = (u8 *)malloc(BADGE_MNG_SIZE);
    if (!badgeMngFileBuf)
    {
        free(badgeDataBuf);
        return NO_MEMORY;
    }

    ensure_dir(APP_ROOT);

    FILE *file;

    if (!(file = fopen(APP_BADGE_DATA, "r")))
    {
        ret = SD_NOT_READ;
        goto end;
    }
    fread(badgeDataBuf, sizeof(u8), BADGE_DATA_SIZE, file);
    fclose(file);

    if (!(file = fopen(APP_BADGE_MNG, "r")))
    {
        ret = SD_NOT_READ;
        goto end;
    }
    fread(badgeMngFileBuf, sizeof(u8), BADGE_MNG_SIZE, file);
    fclose(file);

    if (FSUSER_OpenArchive(&archive, ARCHIVE_EXTDATA, EXTDATA_PATH))
    {
        ret = EXTDATA_NOT_OPEN;
        goto end;
    }

    FSUSER_CreateFile(archive, fsMakePath(PATH_ASCII, BADGE_DATA), 0, BADGE_DATA_SIZE);

    if (FSUSER_OpenFile(&extfile, archive, fsMakePath(PATH_ASCII, BADGE_DATA), FS_OPEN_WRITE, 0))
    {
        ret = EXTDATA_NOT_WRITE;
        goto end;
    }

    FSFILE_Write(extfile, NULL, 0, badgeDataBuf, BADGE_DATA_SIZE, FS_WRITE_FLUSH);
    FSFILE_Close(extfile);

    FSUSER_CreateFile(archive, fsMakePath(PATH_ASCII, BADGE_MNG), 0, BADGE_MNG_SIZE);

    if (FSUSER_OpenFile(&extfile, archive, fsMakePath(PATH_ASCII, BADGE_MNG), FS_OPEN_WRITE, 0))
    {
        ret = EXTDATA_NOT_WRITE;
        goto end;
    }

    FSFILE_Write(extfile, NULL, 0, badgeMngFileBuf, BADGE_MNG_SIZE, FS_WRITE_FLUSH);
    FSFILE_Close(extfile);

    ret = EXTDATA_WRITE;

end:
    free(badgeDataBuf);
    free(badgeMngFileBuf);

    return ret;
}

Result dumpExtdataToFiles()
{
    Handle extfile;
    FS_Archive archive;
    Result ret;

    u8 *badgeDataBuf = (u8 *)malloc(BADGE_DATA_SIZE);
    if (!badgeDataBuf)
        return NO_MEMORY;

    u8 *badgeMngFileBuf = (u8 *)malloc(BADGE_MNG_SIZE);
    if (!badgeMngFileBuf)
    {
        free(badgeDataBuf);
        return NO_MEMORY;
    }

    ensure_dir(APP_ROOT);
    ensure_dir(DUMPED_DIR);

    if (FSUSER_OpenArchive(&archive, ARCHIVE_EXTDATA, EXTDATA_PATH))
    {
        ret = EXTDATA_NOT_OPEN;
        goto end;
    }

    ret = FSUSER_OpenFile(&extfile, archive, fsMakePath(PATH_ASCII, BADGE_DATA), FS_OPEN_READ, 0);
    if (ret)
    {
        ret = EXTDATA_NOT_READ;
        goto end;
    }

    FSFILE_Read(extfile, NULL, 0, badgeDataBuf, BADGE_DATA_SIZE);
    FSFILE_Close(extfile);

    ret = FSUSER_OpenFile(&extfile, archive, fsMakePath(PATH_ASCII, BADGE_MNG), FS_OPEN_READ, 0);
    if (ret)
    {
        ret = EXTDATA_NOT_READ;
        goto end;
    }

    FSFILE_Read(extfile, NULL, 0, badgeMngFileBuf, BADGE_MNG_SIZE);
    FSFILE_Close(extfile);

    FILE *file;
    if (!(file = fopen(DUMPED_BADGE_DATA, "w")))
    {
        ret = SD_NOT_WRITE;
        goto end;
    }
    fwrite(badgeDataBuf, 1, BADGE_DATA_SIZE, file);
    fclose(file);

    if (!(file = fopen(DUMPED_BADGE_MNG, "w")))
    {
        ret = SD_NOT_WRITE;
        goto end;
    }
    fwrite(badgeMngFileBuf, 1, BADGE_MNG_SIZE, file);
    fclose(file);

    ret = SD_WRITE;

end:
    free(badgeDataBuf);
    free(badgeMngFileBuf);

    return ret;
}

Result runCommand(int opt)
{
    Result ret = EXIT;

    selectBottomConsole();

    switch (opt)
    {
    case 0:
        ret = setupExtdata();
        break;
    case 1:
        ret = deleteExtdata();
        break;
    case 2:
        printf(FG(YELLOW) "Attempting to copy data (this may take a while)" RESET "\n");
        ret = writeFilesToExtdata();
        break;
    case 3:
        printf(FG(YELLOW) "Attempting to dump data (this may take a while)" RESET "\n");
        ret = dumpExtdataToFiles();
        break;
    default:
        break;
    }

    switch (STATUS(ret))
    {
    case OK:
        printf(FG(BLUE));
        break;
    case SUCCESS:
        printf(FG(GREEN));
        break;
    case FAIL:
        printf(FG(RED));
        break;
    }

    switch (ret)
    {
    case EXIT:
        break;

        STATUS_CASE(OPTION_CANCEL, "Option cancelled.")
        STATUS_CASE(NO_MEMORY, "Not enough memory to allocate file data.")
        STATUS_CASE(EXTDATA_EXIST, "ExtData archive already exists.")
        STATUS_CASE(EXTDATA_NOT_EXIST, "ExtData archive doesn't exist!")
        STATUS_CASE(EXTDATA_CREATE, "ExtData archive created succesfully!")
        STATUS_CASE(EXTDATA_NOT_CREATE, "Failed to create ExtData archive...")
        STATUS_CASE(EXTDATA_DELETE, "ExtData archive deleted succesfully!")
        STATUS_CASE(EXTDATA_NOT_DELETE, "Failed to delete ExtData archive...")
        STATUS_CASE(EXTDATA_WRITE, "Injection was succesful!")
        STATUS_CASE(EXTDATA_NOT_OPEN, "Failed to open ExtData archive.")
        STATUS_CASE(EXTDATA_NOT_READ, "Failed to read from the ExtData archive.")
        STATUS_CASE(EXTDATA_NOT_WRITE, "Failed to write to ExtData archive.")
        STATUS_CASE(SD_WRITE, "Dump was succesful!")
        STATUS_CASE(SD_NOT_OPEN, "Failed to open app data folder.")
        STATUS_CASE(SD_NOT_READ, "Failed to copy badge files.")
        STATUS_CASE(SD_NOT_WRITE, "Failed to create badge files.")

    default:
        printf("Status code %08lx\n", ret);
        break;
    }

    printf(RESET);

    selectTopConsole();

    return ret;
}

int main(int argc, char **argv)
{
    gfxInitDefault();
    initConsoles();

    int max = 4;
    int opt = 0;

    u32 NNID = -1;

    actInit();
    actuInit(0xB0002C8, 0, 0);
    actuGetAccountDataBlock(0xFE, 4, 12, &NNID);
    actExit();

    selectBottomConsole();
    printf(FG(BLACK) BG(WHITE) "Log" CLEAR_LINE RESET "\n\n");

    if (NNID == 0xFFFFFFFF)
        printf(FG(RED) "Failed to read NNID." RESET "\n");

    selectTopConsole();
    printf(FG(BLACK) BG(WHITE) " Simple Badge Injector" CLEAR_LINE RESET "\n\n");

    if (NNID != 0xFFFFFFFF)
        printf(" Your NNID: %08lX\n\n", NNID);
    else
        printf(" Unknown NNID\n\n");

    printf("   Create ExtData archive 0x14D1" CLEAR_LINE "\n");
    printf("   Delete ExtData archive 0x14D1" CLEAR_LINE "\n");
    printf("   Inject custom badge data" CLEAR_LINE "\n");
    printf("   Dump badge data" CLEAR_LINE "\n");
    printf("   Exit app" CLEAR_LINE "\n");

    while (aptMainLoop())
    {
        hidScanInput();
        u32 kDown = hidKeysDown();

        printf("\e[%d;1H ", opt + 5);

        opt += -!!(kDown & KEY_UP) + !!(kDown & KEY_DOWN);
        opt = opt > max ? 0 : opt < 0 ? max : opt;

        printf("\e[%d;1H>", opt + 5);

        if ((kDown & KEY_A) && runCommand(opt) == EXIT)
            break;

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
