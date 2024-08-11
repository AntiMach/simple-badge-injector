#ifndef MAIN_H
#define MAIN_H

#include <3ds.h>
#include <stdio.h>

#include "actu.h"
#include "badge.h"
#include "common.h"
#include "console.h"

int main(int argc, char **argv);
int runCommand(int opt);

int injectBadgeData(BadgeBuffer buf);
int dumpBadgeData(BadgeBuffer buf);
int deleteBadgeData();

int useBuffer(int (*func)(BadgeBuffer buf));

#endif
