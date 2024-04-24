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

typedef struct _Tmenu_params {
  FONT* font;
  int font_height;
  Tcontrol ctrl;
  BITMAP* bullet;
  int pos;
  DATAFILE* data;
  int fo;
} Tmenu_params;

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

int itrcheck;
char working_directory[1024];
bool dropped_file_is_not_a_replay;
Tscroller greeting_scroller;
DATAFILE* data;
const char* scroller_greetings = "         Welcome to Icy Tower!     Help Harold the Homeboy to climb as high as possible!       Use arrow keys to move and spacebar to jump.      Good luck!";
Tmenu_params menu_params;
bool got_joystick;
Treplay* demo;

void exit_func_00401000(void* func) {
  atexit(func);
}

void _install_allegro_version_check(int a, int* b, void* c, int d) {
  // STUB!!
}

void register_png_file_type() {
  // STUB!!
}

void get_executable_name(char* dst, int len) {
  // STUB!!
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
  _install_allegro_version_check(0,errno_ptr,exit_func_00401000,0x40401);
  register_png_file_type();
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
  load_new_ad_image();
  startMenuMusic();
  log2file("\nMAIN MENU LOOP");
  clear_keybuf();
  if ((options.timesStarted == 1) && (strcmp("guest",options.lastProfile) == 0)) {
    main_menu_callback();
    draw_menu(swap_screen,main_menu,&menu_params,0x163,0x11d,0);
    fadeIn(swap_screen,0x10);
    force_create_profile();
    syncOptionsFromProfile();
    bVar1 = false;
  }
  else {
    bVar1 = true;
  }
  options.msc_volume = profile->msc_volume;
  options.snd_volume = profile->snd_volume;
  if (closeButtonClicked == 0) {
    do {
      main_menu_callback();
      draw_menu(swap_screen,main_menu,&menu_params,0x163,0x11d,0);
      if (bVar1) {
        fadeIn(swap_screen,0x10);
      }
      else {
        blit_to_screen(swap_screen);
      }
      full_path[4] = '\0';
      full_path[5] = '\0';
      full_path[6] = '\0';
      full_path[7] = '\0';
      full_path[0] = '\x1d';
      full_path[1] = '\x01';
      full_path[2] = '\0';
      full_path[3] = '\0';
      iVar4 = handle_menu(main_menu,&menu_params,&ctrl,swap_screen,main_menu_callback,0x163,0x11d,0);
      if ((iVar4 == 0x65) || (iVar4 == 0x85)) {
        log2file(" new game selected");
        is_playing_custom_game = (int)(iVar4 != 0x65);
        fadeOut(0x10);
        stopMenuMusic();
        do {
          if (demo != NULL) {
            destroy_replay(demo);
          }
          demo = NULL;
          iVar3 = new_game();
          if (iVar3 == 0) {
            fadeOut(0x10);
            break;
          }
          iVar3 = play();
          end_game();
          fadeOut(0x10);
        } while (iVar3 != 0);
        if (bg_menu != NULL) {
          play_sample(bg_menu,options.msc_volume,0x80,1000,1);
        }
        menu_params.pos = 0;
        bVar1 = true;
      }
      else if (iVar4 == 0x69) {
        log2file(" high scores selected");
        view_scores(hisc_tables,hisc_names);
        menu_params.pos = 3;
        bVar1 = false;
      }
      else if (iVar4 == 0x68) {
        log2file(" instructions selected");
        fadeOut(0x10);
        show_instructions();
        menu_params.pos = 1;
        bVar1 = true;
      }
      else if (iVar4 == 0x7a) {
        log2file(" load replay selected");
        if (demo != NULL) {
          destroy_replay(demo);
        }
        log2file("   opening %s",replay_directory);
        demo = replay_selector(&ctrl,replay_directory);
        while (demo != NULL) {
          fadeOut(0x10);
          stopMenuMusic();
          run_demo(NULL);
          fadeOut(0x10);
          main_menu_callback();
          draw_menu(swap_screen,main_menu,&menu_params,0x163,0x11d,0);
          fadeIn(swap_screen,0x20);
          if ((bg_menu != NULL) && (options.msc_volume != 0)) {
            play_sample(bg_menu,options.msc_volume,0x80,1000,1);
          }
          demo = replay_selector(&ctrl,replay_directory);
        }
        menu_params.pos = 4;
        bVar1 = false;
      }
      rest(2);
    } while ((closeButtonClicked == 0) && (iVar4 != 0x6b));
  }
  fadeOut(0x10);
  log2file("\nShowing credits");
  show_credits();
  stopMenuMusic();
  uninit_game();
  log2file("\nDone...");
  return 0;
}