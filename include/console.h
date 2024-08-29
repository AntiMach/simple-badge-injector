#ifndef CONSOLE_H
#define CONSOLE_H

#include <3ds.h>
#include <stdlib.h>

#define BLACK "0"
#define RED "1"
#define GREEN "2"
#define YELLOW "3"
#define BLUE "4"
#define MAGENTA "5"
#define CYAN "6"
#define WHITE "7"
#define DEFAULT "9"

#define FG(x) "\e[3" x "m"
#define BG(x) "\e[4" x "m"
#define RESET "\e[0m"
#define CLEAR_LINE "\e[K"

#define TITLE(msg, ...) printf(FG(BLACK) BG(WHITE) msg CLEAR_LINE RESET "\n\n", ##__VA_ARGS__)

#define DEBUG_OK(msg, ...) printf(FG(CYAN) msg RESET "\n", ##__VA_ARGS__)
#define DEBUG_WARNING(msg, ...) printf(FG(YELLOW) msg RESET "\n", ##__VA_ARGS__)
#define DEBUG_ERROR(msg, ...) printf(FG(RED) msg RESET "\n", ##__VA_ARGS__)
#define DEBUG_SUCCESS(msg, ...) printf(FG(GREEN) msg RESET "\n", ##__VA_ARGS__)
#define DEBUG_EXTRA(msg, ...) printf(FG(MAGENTA) msg RESET "\n", ##__VA_ARGS__)

void initConsoles();
void selectTopConsole();
void selectBottomConsole();

#endif
