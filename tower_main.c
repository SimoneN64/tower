#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <direct.h>
#include <windows.h>
#include <wingdi.h>
#define TOWER_VERSION "1.5.1"

typedef struct _FONT {
  void *data;
  int height;
  struct FONT_VTABLE *vtable;
} FONT;

typedef struct _FONT_VTABLE {
  int (*font_height)(FONT *);
  int (*char_length)(FONT *, int);
  int (*text_length)(FONT *, char *);
  int (*render_char)(FONT *, int, int, int, BITMAP *, int, int);
  void (*render)(FONT *, char *, int, int, BITMAP *, int, int);
  void (*destroy)(FONT *);
  int (*get_font_ranges)(FONT *);
  int (*get_font_range_begin)(FONT *, int);
  int (*get_font_range_end)(FONT *, int);
  FONT * (*extract_font_range)(FONT *, int, int);
  FONT * (*merge_fonts)(FONT *, FONT *);
  int (*transpose_font)(FONT *, int);
} FONT_VTABLE;

typedef struct _Tscroller {
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

typedef struct _DATAFILE_PROPERTY {
  char* dat;
  int type;
} DATAFILE_PROPERTY;

typedef struct _DATAFILE {
  void* dat;
  int type;
  long size;
  DATAFILE_PROPERTY* prop;
} DATAFILE;

typedef struct _RGB {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char filler;
} RGB;

typedef struct _V3D {
  float x;
  float y;
  float z;
  float u;
  float v;
  int c;
} V3D;

typedef struct _V3D_f {
  float x;
  float y;
  float z;
  float u;
  float v;
  int c;
} V3D_f;

typedef struct _GFX_MODE {
  int width;
  int height;
  int bpp;
} GFX_MODE;

typedef struct _RLE_SPRITE {
  int w;
  int h;
  int color_depth;
  int size;
} RLE_SPRITE;

typedef struct _FONT_GLYPH {
  short w;
  short h;
} FONT_GLYPH;

typedef struct _GFX_VTABLE {
  int color_depth;
  int mask_color;
  void *unwrite_bank;
  void (*set_clip)(BITMAP *);
  void (*acquire)(BITMAP *);
  void (*release)(BITMAP *);
  BITMAP * (*create_sub_bitmap)(BITMAP *, int, int, int, int);
  void (*created_sub_bitmap)(BITMAP *, BITMAP *);
  int (*getpixel)(BITMAP *, int, int);
  void (*putpixel)(BITMAP *, int, int, int);
  void (*vline)(BITMAP *, int, int, int, int);
  void (*hline)(BITMAP *, int, int, int, int);
  void (*hfill)(BITMAP *, int, int, int, int);
  void (*line)(BITMAP *, int, int, int, int, int);
  void (*fastline)(BITMAP *, int, int, int, int, int);
  void (*rectfill)(BITMAP *, int, int, int, int, int);
  void (*triangle)(BITMAP *, int, int, int, int, int, int, int);
  void (*draw_sprite)(BITMAP *, BITMAP *, int, int);
  void (*draw_256_sprite)(BITMAP *, BITMAP *, int, int);
  void (*draw_sprite_v_flip)(BITMAP *, BITMAP *, int, int);
  void (*draw_sprite_h_flip)(BITMAP *, BITMAP *, int, int);
  void (*draw_sprite_vh_flip)(BITMAP *, BITMAP *, int, int);
  void (*draw_trans_sprite)(BITMAP *, BITMAP *, int, int);
  void (*draw_trans_rgba_sprite)(BITMAP *, BITMAP *, int, int);
  void (*draw_lit_sprite)(BITMAP *, BITMAP *, int, int, int);
  void (*draw_rle_sprite)(BITMAP *, RLE_SPRITE *, int, int);
  void (*draw_trans_rle_sprite)(BITMAP *, RLE_SPRITE *, int, int);
  void (*draw_trans_rgba_rle_sprite)(BITMAP *, RLE_SPRITE *, int, int);
  void (*draw_lit_rle_sprite)(BITMAP *, RLE_SPRITE *, int, int, int);
  void (*draw_character)(BITMAP *, BITMAP *, int, int, int, int);
  void (*draw_glyph)(BITMAP *, FONT_GLYPH *, int, int, int, int);
  void (*blit_from_memory)(BITMAP *, BITMAP *, int, int, int, int, int, int);
  void (*blit_to_memory)(BITMAP *, BITMAP *, int, int, int, int, int, int);
  void (*blit_from_system)(BITMAP *, BITMAP *, int, int, int, int, int, int);
  void (*blit_to_system)(BITMAP *, BITMAP *, int, int, int, int, int, int);
  void (*blit_to_self)(BITMAP *, BITMAP *, int, int, int, int, int, int);
  void (*blit_to_self_forward)(BITMAP *, BITMAP *, int, int, int, int, int, int);
  void (*blit_to_self_backward)(BITMAP *, BITMAP *, int, int, int, int, int, int);
  void (*blit_between_formats)(BITMAP *, BITMAP *, int, int, int, int, int, int);
  void (*masked_blit)(BITMAP *, BITMAP *, int, int, int, int, int, int);
  void (*clear_to_color)(BITMAP *, int);
  void (*pivot_scaled_sprite_flip)(BITMAP *, BITMAP *, float, float, float, float, float, float, int);
  void (*do_stretch_blit)(BITMAP *, BITMAP *, int, int, int, int, int, int, int, int, int);
  void (*draw_gouraud_sprite)(BITMAP *, BITMAP *, int, int, int, int, int, int);
  void (*draw_sprite_end)(void);
  void (*blit_end)(void);
  void (*polygon)(BITMAP *, int, int *, int);
  void (*rect)(BITMAP *, int, int, int, int, int);
  void (*circle)(BITMAP *, int, int, int, int);
  void (*circlefill)(BITMAP *, int, int, int, int);
  void (*ellipse)(BITMAP *, int, int, int, int, int);
  void (*ellipsefill)(BITMAP *, int, int, int, int, int);
  void (*arc)(BITMAP *, int, int, float, float, int, int);
  void (*spline)(BITMAP *, int *, int);
  void (*floodfill)(BITMAP *, int, int, int);
  void (*polygon3d)(BITMAP *, int, BITMAP *, int, V3D **);
  void (*polygon3d_f)(BITMAP *, int, BITMAP *, int, V3D_f **);
  void (*triangle3d)(BITMAP *, int, BITMAP *, V3D *, V3D *, V3D *);
  void (*triangle3d_f)(BITMAP *, int, BITMAP *, V3D_f *, V3D_f *, V3D_f *);
  void (*quad3d)(BITMAP *, int, BITMAP *, V3D *, V3D *, V3D *, V3D *);
  void (*quad3d_f)(BITMAP *, int, BITMAP *, V3D_f *, V3D_f *, V3D_f *, V3D_f *);
  void (*draw_sprite_ex)(BITMAP *, BITMAP *, int, int, int, int);
} GFX_VTABLE;

typedef struct P_DRIVER_INFO {
  int id;
  void* driver;
  int autodetect;
} _DRIVER_INFO;

typedef struct _SYSTEM_DRIVER {
  int id;
  char *name;
  char *desc;
  char *ascii_name;
  int (*init)(void);
  void (*exit)(void);
  void (*get_executable_name)(char *, int);
  int (*find_resource)(char *, char *, int);
  void (*set_window_title)(char *);
  int (*set_close_button_callback)(void (*)(void));
  void (*message)(char *);
  void (*assert)(char *);
  void (*save_console_state)(void);
  void (*restore_console_state)(void);
  BITMAP * (*create_bitmap)(int, int, int);
  void (*created_bitmap)(BITMAP *);
  BITMAP * (*create_sub_bitmap)(BITMAP *, int, int, int, int);
  void (*created_sub_bitmap)(BITMAP *, BITMAP *);
  int (*destroy_bitmap)(BITMAP *);
  void (*read_hardware_palette)(void);
  void (*set_palette_range)(RGB *, int, int, int);
  GFX_VTABLE * (*get_vtable)(int);
  int (*set_display_switch_mode)(int);
  void (*display_switch_lock)(int, int);
  int (*desktop_color_depth)(void);
  int (*get_desktop_resolution)(int *, int *);
  void (*get_gfx_safe_mode)(int *, GFX_MODE *);
  void (*yield_timeslice)(void);
  void * (*create_mutex)(void);
  void (*destroy_mutex)(void *);
  void (*lock_mutex)(void *);
  void (*unlock_mutex)(void *);
  _DRIVER_INFO * (*gfx_drivers)(void);
  _DRIVER_INFO * (*digi_drivers)(void);
  _DRIVER_INFO * (*midi_drivers)(void);
  _DRIVER_INFO * (*keyboard_drivers)(void);
  _DRIVER_INFO * (*mouse_drivers)(void);
  _DRIVER_INFO * (*joystick_drivers)(void);
  _DRIVER_INFO * (*timer_drivers)(void);
} SYSTEM_DRIVER;

typedef struct _Tcontrol {
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

typedef struct _Tmenu_params {
  FONT* font;
  int font_height;
  Tcontrol ctrl;
  BITMAP* bullet;
  int pos;
  DATAFILE* data;
  int fo;
} Tmenu_params;

typedef struct _Trecord {
  unsigned char key_flags;
  int cycle_count;
} Trecord;

typedef struct _Treplay {
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

typedef struct _Tmenu {
  char caption[128];
  int return_select;
  int return_left;
  int return_right;
  int flags;
  void* data;
} Tmenu;

typedef struct _Toptions {
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

typedef struct _Tprofile {
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

typedef struct _Thisc {
  char name[32];
  unsigned int value;
} Thisc;

typedef struct _Thisc_table {
  char name[32];
  Thisc* posts;
} Thisc_table;

typedef struct _SAMPLE {
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

int itrcheck;
char working_directory[1024];
char replay_directory[1024];
bool dropped_file_is_not_a_replay;
Tscroller greeting_scroller;
DATAFILE* data;
const char* scroller_greetings = "         Welcome to Icy Tower!     Help Harold the Homeboy to climb as high as possible!       Use arrow keys to move and spacebar to jump.      Good luck!";
Tmenu_params menu_params;
bool got_joystick, closeButtonClicked;
bool is_playing_custom_game;
Treplay* demo;
Tmenu main_menu[7];
Toptions options;
BITMAP* swap_screen;
Tprofile* profile;
Tcontrol ctrl;
SAMPLE* bg_menu;
Thisc_table hisc_tables[15];
char* hisc_names[15];
SYSTEM_DRIVER* system_driver;
int (*usetc)(char*, int);

void exit_func_00401000(void* func) {
  atexit(func);
}

void get_executable_name(char* dst, int len) {
  int iVar1;
  int iVar2;
  
  if (system_driver->get_executable_name != NULL) {
    /* WARNING: Could not recover jumptable at 0x0043ee0b. Too many branches */
    /* WARNING: Treating indirect jump as call */
    (*system_driver->get_executable_name)(dst,len);
    return;
  }
  iVar1 = (*usetc)(dst,0x2e);
  iVar2 = (*usetc)(dst + iVar1,0x2f);
  (*usetc)(dst + iVar1 + iVar2,0);
}

void replace_filename(char* cwd, char* dst, char* src, int len) {
  // STUB!!
}

void get_logfile_path(char* filename, int len) {
  // STUB!!
}

bool init_game(int argc, char** argv) {
  // STUB!!
  return false;
}

void uninit_game() {

}

int text_height(FONT* f) {
  return ((FONT_VTABLE*)(f->vtable))->font_height(f);
}

int text_length(FONT* f, char* str) {
  return ((FONT_VTABLE*)(f->vtable))->text_length(f, str);
}

void init_scroller(Tscroller* sc, FONT* f, char* t, int w, int h, int horiz) {
  char cVar1;
  int iVar2;
  int i;
  unsigned int uVar3;
  char *pcVar4;
  
  sc->fnt = f;
  iVar2 = text_height(f);
  sc->font_height = iVar2;
  sc->height = h;
  sc->horizontal = horiz;
  sc->text = t;
  sc->width = w;
  if (horiz != 0) {
    iVar2 = text_length(sc->fnt,t);
    sc->length = iVar2;
    sc->offset = sc->width;
    return;
  }
  uVar3 = 0xffffffff;
  pcVar4 = t;
  do {
    if (uVar3 == 0) break;
    uVar3 = uVar3 - 1;
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
  } while (cVar1 != '\0');
  sc->lines[0] = t;
  sc->rows = 1;
  if (0 < (int)(~uVar3 - 1)) {
    iVar2 = 0;
    while( true ) {
      if ((t[iVar2] == '\n') && (sc->rows < 0x200)) {
        sc->lines[sc->rows] = t + iVar2 + 1;
        sc->rows = sc->rows + 1;
        sc->text[iVar2] = '\0';
      }
      if (~uVar3 - 1 == iVar2 + 1) break;
      iVar2 = iVar2 + 1;
      t = sc->text;
    }
    h = sc->height;
  }
  sc->offset = h;
}

void init_control(Tcontrol* ctrl) {

}

void reset_menu(Tmenu main_menu[7], Tmenu_params* menu_params, int idk) {

}

void run_demo(Treplay* demo) {

}

void allegro_exit() {

}

void main_menu_callback() {

}

Treplay* replay_selector(Tcontrol* ctrl, char* path) {

}

#define log2file(str, ...)

int main(int argc,char **argv) {
  bool bVar1;
  void* pHVar2;
  void* hDebugLibrary;
  int *errno_ptr;
  FILE *_File;
  int iVar3;
  int iVar4;
  int ret;
  int must_fade;
  int play_again;
  char *pcVar5;
  FILE *f;
  int i;
  char full_path [1024];
  char logfilename [256];
  
  pHVar2 = LoadLibraryA("exchndl.dll");
  if (pHVar2 == NULL) {
    printf("No exception handler present, RPTs will not be generated");
  }
  errno_ptr = _errno();
  //_install_allegro_version_check(0,errno_ptr,exit_func_00401000,0x40401);
  //register_png_file_type();
  get_executable_name(full_path,0x400);
  replace_filename(working_directory,full_path,"",0x400);
  _chdir(working_directory);
  pcVar5 = logfilename;
  for (iVar4 = 256; iVar4 > 0; iVar4 = iVar4 - 1) {
    *(pcVar5++) = '\0';
  }
  get_logfile_path(logfilename,0x100);
  _File = fopen(logfilename,"wt");
  if (_File != NULL) {
    fprintf(_File,"Icy Tower v%s - log file\n----------------------------\n",TOWER_VERSION);
    fclose(_File);
  }
  if (argc < 1) {
    log2file("Game started with the following commands:");
  }
  else {
    i = 0;
    for(i = 0; i < argc; i++) {
      if (strcmp(argv[i],"-check") == 0) {
        itrcheck = 1;
      }
    }
    
    log2file("Game started with the following commands:");
    
    for(i = 0; i < argc; i++) {
      log2file("   %s",argv[i]);
    }
  }
  log2file("Working directory is:\n   %s",working_directory);
  
  if (!init_game(argc,argv)) {
    if (!dropped_file_is_not_a_replay) {
      log2file("* Failed to initialize the game *");
      //allegro_message("Failed to initialize the game.");
    }
    log2file("Cleaning up Allegro");
    uninit_game();
    log2file("Done...");
    return 1;
  }
  if (itrcheck == 0) {
    log2file("Initiating scroller");
    init_scroller(&greeting_scroller,(FONT *)data[0x36].dat,scroller_greetings,0x280,0x1e,-1);
    menu_params.font = (FONT *)data[0x33].dat;
    menu_params.bullet = (BITMAP *)data[0x48].dat;
    menu_params.pos = 0;
    menu_params.data = data;
    log2file("Initiating menu controls");
    init_control(&menu_params.ctrl);
    if (got_joystick) {
      menu_params.ctrl.use_joy = 1;
    }
    log2file("Resetting menu");
    reset_menu(main_menu,&menu_params,0);
  }
  if (demo != NULL) {
    log2file("Running replay.");
    run_demo(NULL);
    if (itrcheck != 0) {
      log2file("Exiting Allegro");
      allegro_exit();
      log2file("\nDone...");
                    /* WARNING: Subroutine does not return */
      return 0;
    }
  }
  //load_new_ad_image();
  //startMenuMusic();
  log2file("\nMAIN MENU LOOP");
  //clear_keybuf();
  if ((options.timesStarted == 1) && (strcmp("guest",options.lastProfile) == 0)) {
    //main_menu_callback();
    //draw_menu(swap_screen,main_menu,&menu_params,355,285,0);
    //fadeIn(swap_screen,0x10);
    //force_create_profile();
    //syncOptionsFromProfile();
    bVar1 = false;
  }
  else {
    bVar1 = true;
  }
  options.msc_volume = profile->msc_volume;
  options.snd_volume = profile->snd_volume;
  if (!closeButtonClicked) {
    do {
      //main_menu_callback();
      //draw_menu(swap_screen,main_menu,&menu_params,355,285,0);
      if (bVar1) {
        //fadeIn(swap_screen,0x10);
      }
      else {
        //blit_to_screen(swap_screen);
      }
      full_path[4] = '\0';
      full_path[5] = '\0';
      full_path[6] = '\0';
      full_path[7] = '\0';
      full_path[0] = '\x1d';
      full_path[1] = '\x01';
      full_path[2] = '\0';
      full_path[3] = '\0';
      //iVar4 = handle_menu(main_menu,&menu_params,&ctrl,swap_screen,main_menu_callback,0x163,0x11d,0);
      if ((iVar4 == 0x65) || (iVar4 == 0x85)) {
        log2file(" new game selected");
        is_playing_custom_game = (int)(iVar4 != 0x65);
        //fadeOut(0x10);
        //stopMenuMusic();
        do {
          if (demo != NULL) {
            //destroy_replay(demo);
          }
          demo = NULL;
          //iVar3 = new_game();
          if (iVar3 == 0) {
            //fadeOut(0x10);
            break;
          }
          //iVar3 = play();
          //end_game();
          //fadeOut(0x10);
        } while (iVar3 != 0);
        if (bg_menu != NULL) {
          //play_sample(bg_menu,options.msc_volume,0x80,1000,1);
        }
        menu_params.pos = 0;
        bVar1 = true;
      }
      else if (iVar4 == 0x69) {
        log2file(" high scores selected");
        //view_scores(hisc_tables,hisc_names);
        menu_params.pos = 3;
        bVar1 = false;
      }
      else if (iVar4 == 0x68) {
        log2file(" instructions selected");
        //fadeOut(0x10);
        //show_instructions();
        menu_params.pos = 1;
        bVar1 = true;
      }
      else if (iVar4 == 0x7a) {
        log2file(" load replay selected");
        if (demo != NULL) {
          //destroy_replay(demo);
        }
        log2file("   opening %s",replay_directory);
        demo = replay_selector(&ctrl,replay_directory);
        while (demo != NULL) {
          //fadeOut(0x10);
          //stopMenuMusic();
          run_demo(NULL);
          //fadeOut(0x10);
          main_menu_callback();
          //draw_menu(swap_screen,main_menu,&menu_params,0x163,0x11d,0);
          //fadeIn(swap_screen,0x20);
          if ((bg_menu != NULL) && (options.msc_volume != 0)) {
            //play_sample(bg_menu,options.msc_volume,0x80,1000,1);
          }
          demo = replay_selector(&ctrl,replay_directory);
        }
        menu_params.pos = 4;
        bVar1 = false;
      }
      //rest(2);
    } while ((!closeButtonClicked) && (iVar4 != 0x6b));
  }
  //fadeOut(0x10);
  log2file("\nShowing credits");
  //show_credits();
  //stopMenuMusic();
  uninit_game();
  log2file("\nDone...");
  return 0;
}