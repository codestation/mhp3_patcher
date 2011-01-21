/*
 *  MHP3rd patcher EBOOT
 *
 *  Copyright (C) 2010  Codestation
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <string.h>
#include <pspiofilemgr.h>
#include <stdio.h>

PSP_MODULE_INFO("MHP3rd Patcher", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();

#define DATABIN_OFFSET 0x6D50000

unsigned char signature[] = {0xD6, 0xE3, 0x69, 0xA0, 0x53, 0x0E, 0xE5, 0x23, 0x45, 0xB1, 0xA4, 0xCC, 0xC6, 0x79, 0x8E, 0xEC};

SceSize patch_offset[256];
unsigned int patch_size[256];
unsigned int patch_count = 0;
SceSize data_start = 0;

void fill_tables(SceUID transfd) {
    if(transfd < 0)
        return;
    sceIoLseek32(transfd, 0, PSP_SEEK_SET);
    sceIoRead(transfd, &patch_count, 4);
    int i;
    for(i = 0; i < patch_count; i++) {
        sceIoRead(transfd, &patch_offset[i], 4);
        sceIoRead(transfd, &patch_size[i], 4);
    }
    data_start = ((patch_count + 1) * 8);
    if(data_start % 16 > 0)
        data_start += 16 - (data_start % 16);
}


void write_file(SceUID fd, const char *patch_file, int mode) {
	unsigned char buffer[1024];
	char message[64];
	SceUID patch_fd;
	if((patch_fd = sceIoOpen(patch_file, PSP_O_RDONLY, 0777)) >= 0) {
		fill_tables(patch_fd);
		sceIoLseek32(patch_fd, data_start, PSP_SEEK_SET);
		int i;
		for(i = 0;i < patch_count;i++) {
			SceSize patch_len = patch_size[i];
			if(mode == 1)
				sprintf(message, "Aplicando parche (%i/%i)...", i+1, patch_count);
			else
				sprintf(message, "Quitando parche (%i/%i)...", i+1, patch_count);
			pspDebugScreenPrintf(message);
			int j;
			sceIoLseek32(fd, patch_offset[i] + DATABIN_OFFSET, PSP_SEEK_SET);
			for(j = 0; j < patch_len; j+= 1024) {
				sceIoRead(patch_fd, buffer, 1024);
				sceIoWrite(fd, buffer, 1024);
			}
			pspDebugScreenPrintf("OK\n");
		}
		sceIoClose(patch_fd);
	} else {
		pspDebugScreenPrintf("Archivo de parcheo no encontrado\n");
	}
}

void search(const char *patch_file, int mode) {
	SceUID fd;
	unsigned char buffer[16];
	if((fd = sceIoOpen("ms0:/ISO/MHP3.iso", PSP_O_RDWR, 0777)) >= 0) {
		sceIoLseek32(fd, DATABIN_OFFSET, PSP_SEEK_SET);
		sceIoRead(fd, buffer, 16);
		if(!memcmp(buffer, signature, 16)) {
			write_file(fd, patch_file, mode);
		} else {
			pspDebugScreenPrintf("data.bin no encontrado dentro del ISO, no se puede parchear\n");
		}
		sceIoClose(fd);
	} else {
		pspDebugScreenPrintf("ms0:/ISO/MHP3.iso no encontrado\n");
	}
}

int main(int argc, char *argv[])
{
	SceCtrlData pad;
	pspDebugScreenInit();
	pspDebugScreenPrintf("Parche MHP3rd v1.1 - Codestation/Sirius/Bemon!o\n\n");
	pspDebugScreenPrintf("Presione O parchear la ISO\n");
	pspDebugScreenPrintf("Presione [] para quitar el parche\n");
	pspDebugScreenPrintf("Presione X para salir\n");
	while(1) {
		sceCtrlReadBufferPositive(&pad, 1);
		if(pad.Buttons & PSP_CTRL_CROSS) {
			break;
		}
		if(pad.Buttons & PSP_CTRL_CIRCLE) {
			search("ms0:/MHP3RD_DATA.BIN",1);
			break;
		}
		if(pad.Buttons & PSP_CTRL_SQUARE) {
			search("ms0:/MHP3RD_RESTORE.BIN",0);
			break;
		}
	}
	pspDebugScreenPrintf("Saliendo en 5 segundos...\n");
	sceKernelDelayThread(5000000);
	sceKernelExitGame();
    return 0;
}

