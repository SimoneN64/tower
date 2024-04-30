//
// Created by simone on 4/27/24.
//
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "gfx.h"
#include "game.h"

int get_logfile_path(char* filename, int len) {
  strncpy(filename,"log.txt",len);
  return 1;
}

void get_configfile_path(char* cfgfilename, int len) {
  strncpy(cfgfilename, "tower.cfg", len);
}

int get_sort_method() {
  return sort_method;
}

void set_sort_method(int sm) {
  sort_method = sm;
}