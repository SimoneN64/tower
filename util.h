//
// Created by simone on 4/27/24.
//

#pragma once
int utf8_getx(char **s);
void packfile_password(char *password);
char* replace_filename(char* dest, char* path, char* filename, int size);
void get_executable_name(char* dst, int len);
int get_logfile_path(char* filename, int len);
int text_height(FONT* f);
int text_length(FONT* f, char* str);