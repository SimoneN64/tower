//
// Created by simone on 4/26/24.
//

#pragma once
#define TOWER_VERSION "1.5.1"
#include "gfx.h"
#include "driver.h"
#include <stdbool.h>
#include <bits/stdint-uintn.h>

typedef struct Tbeta {
  char email[128];
  char name[128];
  char code[16];
  struct Tbeta* next;
} Tbeta;

typedef struct Tscroller {
  int horizontal;
  char* text;
  FONT* fnt;
  int font_height;
  int width;
  int height;
  int offset;
  int rows;
  int length;
  char *lines[512];
} Tscroller;


typedef struct Tcontrol {
  int use_joy;
  int key_left;
  int key_right;
  int key_up;
  int key_down;
  int key_fire;
  int key_enter;
  int key_pause;
  unsigned char flags;
} Tcontrol;

typedef struct DATAFILE_PROPERTY {
  char* dat;
  int type;
} DATAFILE_PROPERTY;

typedef struct DATAFILE {
  void* dat;
  int type;
  long size;
  DATAFILE_PROPERTY* prop;
} DATAFILE;

typedef struct SAMPLE {
  int bits;
  int stereo;
  int freq;
  int priority;
  unsigned long len;
  unsigned long loop_start;
  unsigned long loop_end;
  unsigned long param;
  void* data;
} SAMPLE;

typedef struct Tmenu_params {
  FONT* font;
  int font_height;
  Tcontrol ctrl;
  BITMAP* bullet;
  int pos;
  DATAFILE* data;
  int fo;
} Tmenu_params;

typedef struct Trecord {
  unsigned char key_flags;
  int cycle_count;
} Trecord;

typedef struct Treplay {
  char header[6];
  int size;
  char name[32];
  char date[32];
  int checksum;
  int score;
  int floor;
  int combo;
  int no_combo_top_floor;
  int biggest_lost_combo;
  int ccc[5];
  int jc[5];
  int floor_shrink;
  int floor_size;
  int start_speed;
  int speed_increase;
  int gravity;
  int rejump;
  int random_seed;
  char comment[42];
  int tc_posts;
  float tc_c_data[100];
  float tc_q_data[100];
  float tc_t_data[100];
  float tc_s_data[100];
  float tc_f_data[100];
  Trecord* data;
} Treplay;

typedef struct Tmenu {
  char caption[128];
  int return_select;
  int return_left;
  int return_right;
  int flags;
  void* data;
} Tmenu;

typedef struct Toptions {
  int flash;
  int checksum;
  int jump_hold;
  int full_screen;
  int floor_shrink;
  int floor_size;
  int start_speed;
  int speed_increase;
  int gravity;
  int msc_volume;
  int snd_volume;
  int sort_method;
  char updateDate[16];
  char posterDate[16];
  char posterUrl[256];
  char posterSrc[256];
  int posterSize;
  char lastProfile[32];
  int timesStarted;
} Toptions;

typedef struct Tprofile {
  char header[6];
  char handle[32];
  int checksum;
  int games_played;
  int custom_games_played;
  int games_quit;
  int seconds_spent_playing;
  int total_floors;
  int total_score;
  int total_combos;
  int total_combo_floors;
  int best_floor;
  int best_combo;
  int best_score;
  int no_combo_top_floor;
  int biggest_lost_combo;
  int cccNum[5];
  int cccTotal[5];
  int ccc[5];
  int jc[5];
  int rewards[10];
  int total_jumps;
  char best_replay_names[32][32];
  int flash;
  int jump_hold;
  char last_avatar[64];
  int start_floor;
  int msc_volume;
  int snd_volume;
  char creationDate[16];
  char saveDate[16];
} Tprofile;

typedef struct Thisc {
  char name[32];
  unsigned int value;
} Thisc;

typedef struct Thisc_table {
  char name[32];
  Thisc* posts;
} Thisc_table;

typedef struct Tgamepad {
  int up,down,left,right;
  int b[32];
} Tgamepad;

typedef RGB PALETTE[256];

typedef struct Tmenu_char_selection {
  int value,max;
  BITMAP* bmp;
  PALETTE pal;
} Tmenu_char_selection;

typedef struct Tmenu_selection {
  int value,size;
  char* caption[32];
} Tmenu_selection;

typedef struct Tcommandline {
  int jumps,combos,sd,keys,tiny;
} Tcommandline;

typedef struct Tplayer {
  double x,y,sx,sy,max_s;
  int level,score,best_combo,status,jump_key,frame,in_combo,acc_level,acc_jumps,dead,rotate;
  float angle;
  int edge,edge_drawn,bounce,shake,latest_combo,show_combo,no_combo_top_floor,biggest_lost_combo,ccc[5],jcTop[5],jc[5];
} Tplayer;

typedef struct LZSS_PACK_DATA {
  int state,i,c,len,r,s,last_match_length,code_buf_ptr;
  unsigned char mask;
  char code_buf[17];
  int match_position,match_length;
  int lson[4097], rson[4353], dad[4097];
  unsigned char text_buf[4113];
} LZSS_PACK_DATA;

typedef struct LZSS_UNPACK_DATA {
  int state,i,j,k,r,c,flags;
  unsigned char text_buf[4113];
} LZSS_UNPACK_DATA;

typedef struct al_normal_packfile_details {
  int hndl,flags;
  unsigned char* buf_pos;
  int buf_size;
  long todo;
  struct PACKFILE* parent;
  LZSS_PACK_DATA* pack_data;
  LZSS_UNPACK_DATA* unpack_data;
  char* filename, *passdata, *passpos;
  unsigned char buf[4096];
} al_normal_packfile_details;

typedef struct PACKFILE_VTABLE {
  int (*pf_fclose)();
  int (*pf_getc)();
  int (*pf_ungetc)(int, void*);
  int (*pf_fread)(void*, long, void*);
  int (*pf_putc)(int, void*);
  int (*pf_fwrite)(void*, long, void*);
  int (*pf_fseek)(void*, int);
  int (*pf_feof)(void*);
  int (*pf_ferror)(void*);
} PACKFILE_VTABLE;

typedef struct PACKFILE {
  PACKFILE_VTABLE* vtable;
  void* userdata;
  bool is_normal_packfile;
  al_normal_packfile_details normal;
} PACKFILE;

void init_control(Tcontrol* ctrl);
Thisc_table * make_hisc_table(char *name);
void reset_hisc_table(Thisc_table *table,char *name,int hi,int lo);
int init_game(int argc, char** argv);
void uninit_game();
void reset_menu(Tmenu main_menu[7], Tmenu_params* menu_params, int idk);
void run_demo(Treplay* demo);
void main_menu_callback();
Treplay* replay_selector(Tcontrol* ctrl, char* path);
void init_scroller(Tscroller* sc, FONT* f, char* t, int w, int h, int horiz);
void reset_options(Toptions *o);
int generate_options_checksum(Toptions *o);
uint64_t file_size_ex(char *filename);
int load_hisc_table(Thisc_table *table,PACKFILE *fp);
#define log2file(str, ...)

extern int itrcheck;
extern int sort_method;
extern char working_directory[1024];
extern char replay_directory[1024];
extern bool dropped_file_is_not_a_replay;
extern Tscroller greeting_scroller;
extern DATAFILE* data;
extern const char* scroller_greetings;
extern Tmenu_params menu_params;
extern bool got_joystick, closeButtonClicked;
extern bool is_playing_custom_game;
extern Treplay* demo;
extern Tmenu main_menu[7];
extern Toptions options;
extern BITMAP* swap_screen;
extern bool init_ok;
extern Tprofile* profile;
extern Tcontrol ctrl;
extern SAMPLE* bg_menu;
extern Thisc_table* hisc_tables[15];
extern char* hisc_names[15];
extern SYSTEM_DRIVER* system_driver;
extern int (*usetc)(char*, int);
extern Tmenu_char_selection play_char;
extern Tmenu_selection eyecandy_selection,scroll_speed_selection,floor_size_selection,gravity_selection;
extern Tcommandline cmdline;