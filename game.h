//
// Created by simone on 4/26/24.
//

#pragma once
#define TOWER_VERSION "1.5.1"
#include <stdbool.h>
#include <stdint.h>
#include "allegro/include/allegro/gfx.h"
#include "allegro/include/allegro/text.h"
#include "allegro/include/allegro/datafile.h"
#include "allegro/include/allegro/palette.h"
#include "allegro/include/allegro/sound.h"
#include "allegro/include/allegro/file.h"
#include "allegro/include/allegro/system.h"
#include "allegro/include/allegro/joystick.h"

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
  fixed tc_c_data[100];
  fixed tc_q_data[100];
  fixed tc_t_data[100];
  fixed tc_s_data[100];
  fixed tc_f_data[100];
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

typedef struct Tmenu_char_selection {
  int value,max;
  BITMAP* bmp;
  PALETTE pal;
} Tmenu_char_selection;

typedef struct Tmenu_selection {
  int value,size;
  char* caption[32];
} Tmenu_selection;

typedef struct Tmenu_slider {
  int value,min,max,step;
} Tmenu_slider;

typedef struct Tmenu_floor_selection {
  int value,max;
} Tmenu_floor_selection;

typedef struct Tcommandline {
  int jumps,combos,sd,keys,tiny;
} Tcommandline;

typedef struct Tcharacter {
  char filename[1024];
  BITMAP* bmp;
  bool ok, uses_datafile;
  char name[128];
  PALETTE pal;
} Tcharacter;

typedef struct Tplayer {
  double x,y,sx,sy,max_s;
  int level,score,best_combo,status,jump_key,frame,in_combo,acc_level,acc_jumps,dead,rotate;
  fixed angle;
  int edge,edge_drawn,bounce,shake,latest_combo,show_combo,no_combo_top_floor,biggest_lost_combo,ccc[5],jcTop[5],jc[5];
} Tplayer;

typedef struct Tgd_combo {
  int start, end, length;
} Tgd_combo;

typedef struct Tgd_jump_sequence {
  int start, dist, num;
} Tgd_jump_sequence;

typedef struct Tparticle {
  int intensity;
  fixed x,y,sx,sy;
  int color;
} Tparticle;

typedef struct Tgame_data {
  Treplay* replay;
  int score, floor, combo, no_combo_top_floor, biggest_lost_combo, ccc[5], jc[5], comboPosts;
  Tgd_combo combos[5000];
  int jumpPosts;
  Tgd_jump_sequence jumps[5000];
  int left, right, jump;
} Tgame_data;

typedef struct Tfloor {
  int empty,start_tile,end_tile,level,sign,tiles;
} Tfloor;

typedef struct Tmap {
  Tfloor room[32];
  int offset;
} Tmap;

typedef struct Tcustom {
  char name[128];
  BITMAP* frame;
  PALETTE pal;
  SAMPLE* jump_sound,*falling,*edge,*yo,*wazup,*bg_music;
  MIDI* bg_midi;
  int uses_datafile;
  DATAFILE* df;
  int ok;
} Tcustom;

void init_control(Tcontrol* ctrl);
Thisc_table * make_hisc_table(char *name);
void reset_hisc_table(Thisc_table *table,char *name,int hi,int lo);
int init_game(int argc, char** argv);
void uninit_game();
void reset_menu(Tmenu main_menu[7], Tmenu_params* menu_params, int idk);
void run_demo(char* filename);
void main_menu_callback();
Treplay* replay_selector(Tcontrol* ctrl, char* path);
void init_scroller(Tscroller* sc, FONT* f, char* t, int w, int h, int horiz);
void reset_options(Toptions *o);
int generate_options_checksum(Toptions *o);
int load_hisc_table(Thisc_table *table,PACKFILE *fp);
void clickedCloseButton(void);
void switchedToProgram(void);
void switchedFromProgram(void);
void datafile_callback(DATAFILE *d);
void datafile_callback_slow(DATAFILE *d);
int collision_type0();
int collision_type1();
int collision_type2();
int collision_type3();
int collision_type4();
int play();
#define log2file(str, ...)

extern Tcustom custom;
extern int start_speeds[6];
extern Tparticle stars[512];
extern int fall_count,clock_angle;
extern int collision_type;
extern int (*collision_types[5])();
extern int itrcheck;
extern int sort_method;
extern char working_directory[1024];
extern char replay_directory[1024];
extern bool dropped_file_is_not_a_replay;
extern char key[127];
extern Tscroller greeting_scroller;
extern DATAFILE* data;
extern Tmap map;
extern const char* scroller_greetings;
extern Tmenu_params menu_params;
extern bool got_joystick, closeButtonClicked, hasFocus;
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
extern JOYSTICK_INFO joy[8];
extern char* hisc_names[15];
extern SYSTEM_DRIVER* system_driver;
extern int (*usetc)(char*, int);
extern Tmenu_char_selection play_char;
extern Tmenu_selection eyecandy_selection,scroll_speed_selection,floor_size_selection,gravity_selection;
extern Tcommandline cmdline;
extern int cycle_count;
extern bool window,debug;
extern char init_string[7];
extern int player_id;
extern Tplayer* ply[1000];
extern BITMAP* gameover_bmp;
extern DATAFILE* sfx;
extern char sfx_file[512];
extern SAMPLE* combo_sound[10];
extern SAMPLE* speaker[3];
extern SAMPLE* sounds[9];
extern SAMPLE* menu_sounds[2];
extern SAMPLE* jump_sound[3];
extern SAMPLE* bg_beat;
extern SAMPLE* bg_menu;
extern Tmenu_slider snd_volume_slider,msc_volume_slider,eyecandy_selection,gravity_selection,floor_size_selection,scroll_speed_selection;
extern Tmenu_floor_selection floors;
extern int seed;
extern Tcharacter* characters;
extern Tbeta* testers;
extern int num_chars,logic_count;
extern int progress_count;
extern bool recording,fast_forward,fast_fast_forward;