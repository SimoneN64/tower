//
// Created by simone on 4/27/24.
//
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "gfx.h"

char the_password[256]={0};

int utf8_getx(char **s) {
  char *pbVar1;
  int iVar2;
  unsigned int uVar3;
  int c;
  char bVar4;
  int t;
  int n;
  int iVar5;
  char *pbVar6;

  pbVar1 = (char *)*s;
  bVar4 = *pbVar1;
  uVar3 = (unsigned int)bVar4;
  pbVar6 = pbVar1 + 1;
  *s = (char *)pbVar6;
  if ((char)bVar4 < '\0') {
    if ((bVar4 & 0x40) == 0) {
      uVar3 = uVar3 & 0x7f;
    }
    else {
      iVar2 = 1;
      do {
        iVar5 = iVar2;
        bVar4 = (char)(iVar5 + 1);
        iVar2 = iVar5 + 1;
      } while ((0x80 >> (bVar4 & 0x1f) & uVar3) != 0);
      uVar3 = uVar3 & (1 << (8 - bVar4 & 0x1f)) - 1U;
      bVar4 = pbVar1[1];
      while( true ) {
        *s = (char *)(pbVar6 + 1);
        if ((bVar4 & 0xc0) != 0x80) break;
        uVar3 = bVar4 & 0x3f | uVar3 << 6;
        iVar5 = iVar5 + -1;
        if (iVar5 < 1) {
          return uVar3;
        }
        pbVar6 = (char *)*s;
        bVar4 = *pbVar6;
      }
      *s = (char *)pbVar6;
      uVar3 = 0x5e;
    }
  }
  return uVar3;
}

void packfile_password(char *password) {
  int c;
  int iVar1;
  int iVar2;
  int i;

  if (password == NULL) {
    the_password[0] = '\0';
    return;
  }
  for(iVar2 = 0; iVar2 < 0xff; iVar2++) {
    iVar1 = utf8_getx(&password);
    if (iVar1 == 0) break;
    the_password[iVar2] = (char)iVar1;
  }
  the_password[iVar2] = '\0';
}

char* replace_filename(char* dest, char* path, char* filename, int size) {
  int iVar1;
  int c;
  int iVar2;
  int pos;
  int n;
  char tmp [1024];

  iVar1 = strlen(path);
  while (n = iVar1, 0 < n) {
    iVar2 = path[n-1];
    if (((iVar2 == 0x2f) || (iVar2 == 0x5c)) || (iVar1 = n + -1, iVar2 == 0x3a)) break;
  }
  strncpy(tmp,path,n);
  strcat(tmp,filename);
  strcpy(dest,tmp);
  return dest;
}

void get_executable_name(char* dst, int len) {
  int iVar1;
  int iVar2;

  if (system_driver->get_executable_name != NULL) {
    (*system_driver->get_executable_name)(dst,len);
    return;
  }
  iVar1 = (*usetc)(dst,46);
  iVar2 = (*usetc)(dst + iVar1,47);
  (*usetc)(dst + iVar1 + iVar2,0);
}

int get_logfile_path(char* filename, int len) {
  strncpy(filename,"log.txt",len);
  return 1;
}

int text_height(FONT* f) {
  return ((FONT_VTABLE*)(f->vtable))->font_height(f);
}

int text_length(FONT* f, char* str) {
  return ((FONT_VTABLE*)(f->vtable))->text_length(f, str);
}