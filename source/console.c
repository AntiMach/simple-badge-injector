#include "console.h"

static PrintConsole topConsole;
static PrintConsole bottomConsole;

void initConsoles()
{
    consoleInit(GFX_TOP, &topConsole);
    consoleInit(GFX_BOTTOM, &bottomConsole);
}

void selectTopConsole()
{
    consoleSelect(&topConsole);
}

void selectBottomConsole()
{
    consoleSelect(&bottomConsole);
}
