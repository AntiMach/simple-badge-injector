#ifndef MAIN_H
#define MAIN_H

#include <3ds.h>
#include <stdio.h>

#include "actu.h"
#include "badge.h"
#include "console.h"

int main(int argc, char **argv);
int runCommand(int opt);

void injectBadgeData(BadgeBuffer buf);
void dumpBadgeData(BadgeBuffer buf);
void fixBadgeData(BadgeBuffer buf);
void deleteBadgeData();

void useBuffer(void (*func)(BadgeBuffer buf));

#endif
