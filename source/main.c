#include "main.h"

static u32 NNID = 0xFFFFFFFF;

int main(int argc, char **argv)
{
    gfxInitDefault();
    initConsoles();

    int max = 3;
    int opt = 0;

    actInit();
    actuInit(0xB0002C8, 0, 0);
    actuGetAccountDataBlock(0xFE, 4, 12, &NNID);
    actExit();

    selectBottomConsole();
    printf(FG(BLACK) BG(WHITE) "Log" CLEAR_LINE RESET "\n\n");

    printf(FG(BLUE) "App root: " APP_ROOT "\n");

    if (NNID == 0xFFFFFFFF)
        printf(FG(RED) "Failed to read NNID.\n");

    selectTopConsole();
    printf(FG(BLACK) BG(WHITE) " " APP_TITLE CLEAR_LINE RESET "\n\n");

    if (NNID != 0xFFFFFFFF)
        printf(" Your NNID: " FG(GREEN) "%08lX" RESET "\n\n", NNID);
    else
        printf(FG(RED) " Unknown NNID" RESET "\n\n");

    printf("   Inject badge data\n");
    printf("   Dump badge data\n");
    printf("   Delete badge data\n");
    printf("   Exit app\n");

    while (aptMainLoop())
    {
        hidScanInput();
        u32 kDown = hidKeysDown();

        printf("\e[%d;1H ", opt + 5);

        if (kDown & KEY_UP)
            opt--;

        if (kDown & KEY_DOWN)
            opt++;

        if (opt > max)
            opt = 0;

        if (opt < 0)
            opt = max;

        printf("\e[%d;1H>", opt + 5);

        if (kDown & KEY_A)
            if (runCommand(opt) == EXIT)
                break;

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}

int runCommand(int opt)
{
    int ret = EXIT;

    selectBottomConsole();

    switch (opt)
    {
    case 0:
        printf(FG(YELLOW) "Injecting data (this may take a while)" RESET "\n");
        ret = useBuffer(&injectBadgeData);
        break;
    case 1:
        printf(FG(YELLOW) "Dumping data (this may take a while)" RESET "\n");
        ret = useBuffer(&dumpBadgeData);
        break;
    case 2:
        ret = deleteBadgeData();
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
        printf("Status code %08x\n", ret);
        break;
    }

    printf(RESET);

    selectTopConsole();

    return ret;
}

int injectBadgeData(BadgeBuffer buf)
{
    Result res;

    if (readBadgeDataFromSD(buf))
        return SD_NOT_READ;

    if (NNID != 0xFFFFFFFF)
        buf->mngFile.NNID = NNID;

    if (R_FAILED(res = writeBadgeDataToArchive(buf)))
    {
        printf("%08lX\n\n", res);
        return EXTDATA_NOT_WRITE;
    }

    return EXTDATA_WRITE;
}

int dumpBadgeData(BadgeBuffer buf)
{
    Result res;

    if (R_FAILED(res = readBadgeDataFromArchive(buf)))
    {
        printf("%08lX\n\n", res);
        return EXTDATA_NOT_READ;
    }

    if (writeBadgeDataToSD(buf))
        return SD_NOT_WRITE;

    return SD_WRITE;
}

int deleteBadgeData()
{
    if (R_FAILED(checkBadgeArchive()))
        return EXTDATA_NOT_EXIST;

    printf(FG(YELLOW) "Are you sure you want to delete it?\n");
    printf(FG(MAGENTA) "Y = Yes\n");
    printf(FG(MAGENTA) "X = No\n" RESET);

    while (1)
    {
        hidScanInput();
        u32 option = hidKeysDown();

        if (option & KEY_X)
            return OPTION_CANCEL;

        if (option & KEY_Y)
            break;

        gspWaitForVBlank();
    }

    if (R_FAILED(deleteBadgeArchive()))
        return EXTDATA_NOT_DELETE;

    return EXTDATA_DELETE;
}

int useBuffer(int (*func)(BadgeBuffer buf))
{
    BadgeBuffer buf = NULL;

    buf = initBadgeData();
    if (!buf)
        return NO_MEMORY;

    int ret = func(buf);

    destroyBadgeData(buf);

    return ret;
}