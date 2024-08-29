#include "main.h"

static u32 NNID = 0xFFFFFFFF;

int main(int argc, char **argv)
{
    gfxInitDefault();
    initConsoles();

    selectBottomConsole();

    int opt = 0;
    int max = 4;
    Result res = getNNID(&NNID);

    TITLE("Log");
    DEBUG_OK("App root: " APP_ROOT);

    if (R_FAILED(res) || NNID == 0xFFFFFFFF)
        DEBUG_ERROR("Failed to read NNID (%08lX)", res);

    selectTopConsole();

    TITLE(" " APP_TITLE);

    if (R_FAILED(res) || NNID == 0xFFFFFFFF)
        DEBUG_ERROR(" Unknown NNID");
    else
        printf(" Your NNID: " FG(GREEN) "%08lX" RESET "\n", NNID);

    printf("\n");
    printf("   Inject badge data\n");
    printf("   Dump badge data\n");
    printf("   Fix NNID/PNID\n");
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
            if (runCommand(opt))
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
    int ret = 0;

    selectBottomConsole();

    switch (opt)
    {
    case 0:
        DEBUG_WARNING("Injecting data (this may take a while)");
        useBuffer(&injectBadgeData);
        break;
    case 1:
        DEBUG_WARNING("Dumping data (this may take a while)");
        useBuffer(&dumpBadgeData);
        break;
    case 2:
        DEBUG_WARNING("Fixing NNID/PNID");
        useBuffer(&fixBadgeData);
        break;
    case 3:
        deleteBadgeData();
        break;
    default:
        ret = 1;
        break;
    }

    selectTopConsole();

    return ret;
}

void injectBadgeData(BadgeBuffer buf)
{
    DEBUG_OK("Reading from SD card...");
    if (readBadgeDataFromSD(buf))
        return;

    if (NNID != 0xFFFFFFFF)
        buf->mngFile.NNID = NNID;

    DEBUG_OK("Writing to ExtData archive...");
    if (R_FAILED(writeBadgeDataToArchive(buf, 0)))
        return;

    DEBUG_SUCCESS("Injection was succesful");
}

void dumpBadgeData(BadgeBuffer buf)
{
    DEBUG_OK("Reading from ExtData archive...");
    if (R_FAILED(readBadgeDataFromArchive(buf, 0)))
        return;

    DEBUG_OK("Writing to SD card...");
    if (writeBadgeDataToSD(buf))
        return;

    DEBUG_SUCCESS("Dump was succesful");
}

void fixBadgeData(BadgeBuffer buf)
{
    if (NNID == 0xFFFFFFFF)
    {
        DEBUG_ERROR("NNID is unknown");
        return;
    }

    DEBUG_OK("Reading from ExtData archive...");
    if (R_FAILED(readBadgeDataFromArchive(buf, 1)))
        return;

    buf->mngFile.NNID = NNID;

    DEBUG_OK("Writing to ExtData archive...");
    if (writeBadgeDataToArchive(buf, 1))
        return;

    DEBUG_SUCCESS("Done fixing");
}

void deleteBadgeData()
{
    if (R_FAILED(checkBadgeArchive()))
    {
        DEBUG_ERROR("ExtData archive doesn't exist");
        return;
    }

    DEBUG_WARNING("Are you sure you want to delete it?");
    DEBUG_EXTRA("Y = Yes");
    DEBUG_EXTRA("X = No");

    while (1)
    {
        hidScanInput();
        u32 option = hidKeysDown();

        if (option & KEY_X)
        {
            DEBUG_OK("Option cancelled");
            return;
        }

        if (option & KEY_Y)
            break;

        gspWaitForVBlank();
    }

    Result res;

    if (R_FAILED(res = deleteBadgeArchive()))
        DEBUG_ERROR("Failed to delete ExtData archive (%08lX)", res);
    else
        DEBUG_SUCCESS("ExtData archive deleted succesfully");
}

void useBuffer(void (*func)(BadgeBuffer buf))
{
    BadgeBuffer buf = NULL;

    buf = initBadgeData();
    if (!buf)
    {
        DEBUG_ERROR("Failed to allocate memory");
        return;
    }

    DEBUG_EXTRA("Memory allocated");

    func(buf);

    destroyBadgeData(buf);

    DEBUG_EXTRA("Memory disposed");
}