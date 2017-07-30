#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <dirent.h>
#include <math.h>
#include <sys/stat.h>

#include "ext.h"
#include "actu.h"

Result writeFilesToExtdata() {	
	Handle extfile;
	FS_Archive archive;
	Result ret;
	
	u64 badgeDataSize = 0xF4DF80;
	u64 badgeMngFileSize = 0xD4A8;
	
	u32 extpath[3] = {MEDIATYPE_SD, 0x000014d1, 0};
	
	u8 *badgeDataBuf = NULL;
	u8 *badgeMngFileBuf = NULL;
	
	badgeDataBuf = (u8*)malloc(badgeDataSize);
	if (badgeDataBuf == NULL) {ret = 1; goto end;}
	
	badgeMngFileBuf = (u8*)malloc(badgeMngFileSize);
	if (badgeMngFileBuf == NULL) {ret = 1; goto end;}
	
	DIR* dir = opendir("/3ds/data/Simple Badge Injector/Badge Files");
	if (!dir) {ret = 2; goto end;}
	closedir(dir);
	
	FILE* file = fopen("/3ds/data/Simple Badge Injector/Badge Files/BadgeData.dat", "r");
	if (!file) {ret = 3; goto end;}
	fread(badgeDataBuf, 1, badgeDataSize, file);
	fclose(file);
	file = fopen("/3ds/data/Simple Badge Injector/Badge Files/BadgeMngFile.dat", "r");
	if (!file) {ret = 3; goto end;}
	fread(badgeMngFileBuf, 1, badgeMngFileSize, file);
	fclose(file);
	
	ret = FSUSER_OpenArchive(&archive, ARCHIVE_EXTDATA, (FS_Path){PATH_BINARY, 0xC, (u8*)extpath});
	if (ret) {ret = 4; goto end;}
	
	FSUSER_CreateFile(archive, fsMakePath(PATH_ASCII, "/BadgeData.dat"), 0, badgeDataSize);
	ret = FSUSER_OpenFile(&extfile, archive, fsMakePath(PATH_ASCII, "/BadgeData.dat"), FS_OPEN_WRITE, 0);
	if (ret) {ret = 5; goto end;}
	
	FSFILE_Write(extfile, NULL, 0, badgeDataBuf, badgeDataSize, FS_WRITE_FLUSH);
	FSFILE_Close(extfile);
	
	FSUSER_CreateFile(archive, fsMakePath(PATH_ASCII, "/BadgeMngFile.dat"), 0, badgeMngFileSize);
	ret = FSUSER_OpenFile(&extfile, archive, fsMakePath(PATH_ASCII, "/BadgeMngFile.dat"), FS_OPEN_WRITE, 0);
	if (ret) {ret = 5; goto end;}
	
	FSFILE_Write(extfile, NULL, 0, badgeMngFileBuf, badgeMngFileSize, FS_WRITE_FLUSH);
	FSFILE_Close(extfile);
	
	ret = 0;
	end:
	free(badgeDataBuf);
	free(badgeMngFileBuf);
	return ret;
}

Result dumpExtdataToFiles() {
	Handle extfile;
	FS_Archive archive;
	Result ret;
	
	u64 badgeDataSize = 0xF4DF80;
	u64 badgeMngFileSize = 0xD4A8;
	
	u32 extpath[3] = {MEDIATYPE_SD, 0x000014d1, 0};
	
	u8 *badgeDataBuf = NULL;
	u8 *badgeMngFileBuf = NULL;
	
	badgeDataBuf = (u8*)malloc(badgeDataSize);
	if (badgeDataBuf == NULL) {ret = 1; goto end;}
	
	badgeMngFileBuf = (u8*)malloc(badgeMngFileSize);
	if (badgeMngFileBuf == NULL) {ret = 1; goto end;}
	
	DIR* dir = opendir("/3ds/data/Simple Badge Injector/Dumped Badge Files");
	if (!dir) {ret = mkdir("/3ds/data/Simple Badge Injector/Dumped Badge Files", 0777);}
	closedir(dir);
	
	ret = FSUSER_OpenArchive(&archive, ARCHIVE_EXTDATA, (FS_Path){PATH_BINARY, 0xC, (u8*)extpath});
	if (ret) {ret = 2; goto end;}
	
	ret = FSUSER_OpenFile(&extfile, archive, fsMakePath(PATH_ASCII, "/BadgeData.dat"), FS_OPEN_READ, 0);
	if (ret) {goto end;}
	
	FSFILE_Read(extfile, NULL, 0, badgeDataBuf, badgeDataSize);
	FSFILE_Close(extfile);
	
	ret = FSUSER_OpenFile(&extfile, archive, fsMakePath(PATH_ASCII, "/BadgeMngFile.dat"), FS_OPEN_READ, 0);
	if (ret) {ret = 3; goto end;}
	
	FSFILE_Read(extfile, NULL, 0, badgeMngFileBuf, badgeMngFileSize);
	FSFILE_Close(extfile);
	
	FILE* file = fopen("/3ds/data/Simple Badge Injector/Dumped Badge Files/BadgeData.dat", "w");
	if (!file) {ret = 4; goto end;}
	fwrite(badgeDataBuf, 1, badgeDataSize, file);
	fclose(file);
	
	file = fopen("/3ds/data/Simple Badge Injector/Dumped Badge Files/BadgeMngFile.dat", "w");
	if (!file) {ret = 4; goto end;}
	fwrite(badgeMngFileBuf, 1, badgeMngFileSize, file);
	fclose(file);
	
	ret = 0;
	end:
	free(badgeDataBuf);
	free(badgeMngFileBuf);
	return ret;
}

Result setupExtdata() {
	u32 extdata_archive_lowpathdata[3] = {MEDIATYPE_SD, 0x000014d1, 0};
	FS_Archive extdata_archive;
	
	Result ret = FSUSER_OpenArchive(&extdata_archive, ARCHIVE_EXTDATA, (FS_Path){PATH_BINARY, 0xC, (u8*)extdata_archive_lowpathdata});
	FSUSER_CloseArchive(extdata_archive);
	
	if (!ret) {
		return -1;
	} else {
		return CreateExtSaveData(0x14d1);
	}
}

Result deleteExtdata() {
	u32 extdata_archive_lowpathdata[3] = {MEDIATYPE_SD, 0x000014d1, 0};
	FS_Archive extdata_archive;
	
	Result ret = FSUSER_OpenArchive(&extdata_archive, ARCHIVE_EXTDATA, (FS_Path){PATH_BINARY, 0xC, (u8*)extdata_archive_lowpathdata});
	FSUSER_CloseArchive(extdata_archive);
	
	if (!ret){
		printf("\e[34mAre you sure you want to delete it?\n\e[32mY=Yes\n\e[31mX=No\e[0m\n");
		while(1) {
			hidScanInput();
			u32 option = hidKeysDown();
			if (option & KEY_X) {
				ret = -1;
				break;
			}
			if (option & KEY_Y) {
				ret = DeleteExtSaveData(0x14d1);
				break;
			}
			gspWaitForVBlank();
		}
	} else {
		ret = -2;
	}
	return ret;
}

u32 getNNID() {
	u32 nnidNum = 0xFFFFFFFF;
	
    actInit();
    ACTU_Initialize(0xB0002C8, 0, 0);
    ACTU_GetAccountDataBlock(0xFE, 4, 12, &nnidNum);
    actExit();
	
	return nnidNum;
}

int main(int argc, char **argv) {
    gfxInitDefault();
    PrintConsole topScreen;
	PrintConsole bottomScreen;
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	Result ret;
	int max = 4;
	int opt = 0;
	u32 NNID = getNNID();
	
	consoleSelect(&bottomScreen);
	printf("\e[30;47mLog\e[K\e[0m\n\n");
	
	if (NNID == 0xFFFFFFFF){
		printf("\e[31mFailed to read NNID.\e[0m\n");
	}
	
	
	
	consoleSelect(&topScreen);
	if (NNID != 0xFFFFFFFF)
		printf("\e[2;1HYour NNID: %08lX", NNID);
	else
		printf("\e[2;1HUnknown NNID");
	
	printf("\e[0;0H\e[30;47m Simple Badge Injector - By TheMachinumps\e[K\e[0m");
	
	printf("\e[4;3HCreate ExtData archive 0x14D1\e[K");
	printf("\e[5;3HDelete ExtData archive 0x14D1\e[K");
	printf("\e[6;3HInject custom badge data\e[K");
	printf("\e[7;3HDump badge data\e[K");
	printf("\e[8;3HExit app\e[K");

    while(aptMainLoop()) {
        hidScanInput();
		u32 kDown = hidKeysDown();
		
		if(kDown & KEY_UP)
			opt--;
		if(kDown & KEY_DOWN)
			opt++;
		
		if(opt > max)
			opt = 0;
		if(opt < 0)
			opt = max;
		
		for(int i = 0; i <= max; i++){
			if(opt == i)
				printf("\e[%d;1H>", i + 4);
			else
				printf("\e[%d;1H ", i + 4);
		}
		
		if(kDown & KEY_A){
			if(opt == 0){
				ret = setupExtdata();
				consoleSelect(&bottomScreen);
				if (ret == -1)
					printf("\e[34mExtData archive already exists.\e[0m\n");
				else if (!ret)
					printf("\e[32mExtData archive created succesfully!\e[0m\n");
				else
					printf("\e[31mFailed to create ExtData archive...\e[0m\n");
				consoleSelect(&topScreen);
			}
			if(opt == 1){
				consoleSelect(&bottomScreen);
				ret = deleteExtdata();
				if (ret == -1)
					printf("\e[34mOption canceled.\e[0m\n");
				else if (ret == -2)
					printf("\e[31mExtData archive doesn't exist!\e[0m\n");
				else if (!ret)
					printf("\e[32mExtData archive deleted succesfully!\e[0m\n");
				else
					printf("\e[31mFailed to delete ExtData archive...\e[0m\n");
				consoleSelect(&topScreen);
			}
			if(opt == 2){
				consoleSelect(&bottomScreen);
				printf("\e[33mAttempting to copy data (this may take a while)\e[0m\n");
				ret = writeFilesToExtdata();
				if (ret == 0)
					printf("\e[32mInjection was succesful!\e[0m\n");
				else if (ret == 1)
					printf("\e[31mNot enough memory to allocate the files.\e[0m\n");
				else if (ret == 2)
					printf("\e[31mFailed to open\"Badge Files\" folder.\e[0m\n");
				else if (ret == 3)
					printf("\e[31mFailed to copy badge files.\e[0m\n");
				else if (ret == 4)
					printf("\e[31mFailed to open ExtData archive.\e[0m\n");
				else
					printf("\e[31mFailed to write to ExtData archive.\e[0m\n");
				consoleSelect(&topScreen);
			}
			if(opt == 3){
				consoleSelect(&bottomScreen);
				printf("\e[33mAttempting to dump data (this may take a while)\e[0m\n");
				ret = dumpExtdataToFiles();
				if (ret == 0)
					printf("\e[32mDump was succesful!\e[0m\n");
				else if (ret == 1)
					printf("\e[31mNot enough memory to allocate the files.\e[0m\n");
				else if (ret == 2)
					printf("\e[31mFailed to open ExtData archive.\e[0m\n");
				else if (ret == 3)
					printf("\e[31mFailed to read from the ExtData archive.\e[0m\n");
				else
					printf("\e[31mFailed to create badge files.\e[0m\n");
				consoleSelect(&topScreen);
			}
			if(opt == 4){
				break;
			}
		}

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
