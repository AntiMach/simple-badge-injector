#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdlib.h>
#include <3ds.h>

#define BLACK   "0"
#define RED     "1"
#define GREEN   "2"
#define YELLOW  "3"
#define BLUE    "4"
#define MAGENTA "5"
#define CYAN    "6"
#define WHITE   "7"
#define DEFAULT "9"

#define FG(x)       "\e[3"x"m"
#define BG(x)       "\e[4"x"m"
#define RESET       "\e[0m"
#define CLEAR_LINE  "\e[K"

void initConsoles();
void selectTopConsole();
void selectBottomConsole();

#endif
