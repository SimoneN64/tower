//
// Created by simone on 4/27/24.
//
#include "game.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <winsock.h>
#include <math.h>

int (*collision_types[5])() = {
  collision_type0,
  collision_type1,
  collision_type2,
  collision_type3,
  collision_type4,
};

int collision_type;
int itrcheck;
char working_directory[1024];
char replay_directory[1024];
bool dropped_file_is_not_a_replay;
Tscroller greeting_scroller;
DATAFILE* data;
const char* scroller_greetings = "         Welcome to Icy Tower!     Help Harold the Homeboy to climb as high as possible!       Use arrow keys to move and spacebar to jump.      Good luck!";
Tmenu_params menu_params;
bool got_joystick, closeButtonClicked, hasFocus;
bool is_playing_custom_game;
Treplay* demo;
Tmenu main_menu[7];
Toptions options;
BITMAP* swap_screen;
bool init_ok;
Tprofile* profile;
Tcontrol ctrl;
SAMPLE* bg_menu;
Thisc_table* hisc_tables[15];
char* hisc_names[15];
SYSTEM_DRIVER* system_driver;
int (*usetc)(char*, int);
Tmenu_char_selection play_char;
Tmenu_selection eyecandy_selection,scroll_speed_selection,floor_size_selection,gravity_selection;
Tcommandline cmdline;
bool window;
int cycle_count;
char init_string[7] = "qyuj}h";
int player_id;
Tplayer* ply[1000];
BITMAP* gameover_bmp;
DATAFILE* sfx;
char sfx_file[512];
SAMPLE* combo_sound[10];
SAMPLE* speaker[3];
SAMPLE* sounds[9];
SAMPLE* menu_sounds[2];
SAMPLE* jump_sound[3];
SAMPLE* bg_beat;
SAMPLE* bg_menu;
Tmenu_slider snd_volume_slider,msc_volume_slider,eyecandy_selection,gravity_selection,floor_size_selection,scroll_speed_selection;
Tmenu_floor_selection floors;
int seed;
Tcharacter* characters;
Tbeta* testers;
int num_chars;
int progress_count = 0;
bool recording;

Thisc_table * make_hisc_table(char *name) {
  Thisc_table *res;
  Thisc *pTVar1;
  Thisc_table *table;

  res = (Thisc_table *)malloc(sizeof(Thisc_table));
  if (res) {
    pTVar1 = (Thisc *)malloc(5*sizeof(Thisc));
    res->posts = pTVar1;
    if (pTVar1 != NULL) {
      strcpy(res->name,name);
      return res;
    }
    res = NULL;
  }
  return res;
}

void reset_hisc_table(Thisc_table *table,char *name,int hi,int lo) {
  for(int i = 0; i < 180; i += 36) {
    strcpy(table->posts->name + i,name);
    *(int *)(table->posts->name + i + 32) = 0;
  }
}

void reset_options(Toptions *o) {
  uint64_t uVar5;

  o->flash = 0;
  o->full_screen = 0;
  o->jump_hold = 1;
  o->msc_volume = 0x96;
  o->snd_volume = 0x96;
  o->floor_shrink = 1;
  o->speed_increase = 1;
  o->start_speed = 5;
  o->floor_size = 1;
  o->gravity = 1;
  o->timesStarted = 0;
  o->sort_method = 1;
  o->updateDate[0] = 0;
  strncpy(o->updateDate, "2001-12-22", 11);
  *(int *)o->lastProfile = 0x73657567;
  *(short *)(o->lastProfile + 4) = 0x74;
  o->posterDate[0] = 0;
  strncpy(o->posterDate, "1111-22-33", 11);
  o->posterUrl[0] = 0;
  strncpy(o->posterUrl, "http://www.freelunchdesign.com/?src=it14_game", 46);
  o->posterSrc[0] = 0;
  strncpy(o->posterSrc, "default.dat", 12);
  uVar5 = file_size("data/com/default.dat");
  o->posterSize = (int)uVar5;
}

int generate_options_checksum(Toptions *o) {
  unsigned int cs = 0;
  int j;
  int values [11];

  j = o->flash;
  values[1] = o->full_screen;
  values[2] = o->jump_hold;
  values[3] = o->msc_volume;
  values[4] = o->snd_volume;
  values[5] = o->floor_size;
  values[6] = o->floor_shrink;
  values[7] = o->gravity;
  values[8] = o->start_speed;
  values[9] = o->speed_increase;
  values[10] = o->posterSize;

  for(int i = 0; i < 0xb; i++) {
    cs += (j + i) * 0x11;
    j = values[i];
  }

  for(int i = 0; i < 8; i++) {
    cs += (int) o->updateDate[i];
  }

  for(int i = 0; i < 16; i++) {
    cs += (int)o->posterDate[i] + (int)o->updateDate[i];
  }

  for(int i = 0; i < 256; i++) {
    cs += (int)o->posterSrc[i] + (int)o->posterUrl[i];
  }

  cs = (cs ^ 0x3d ^ cs >> 0x10) * 9;
  cs = (cs >> 4 ^ cs) * 0x27d4eb2d;
  return cs >> 0xf ^ cs;
}

void load_options(Toptions *o,PACKFILE *fp) {
  int iVar1;
  int cs;

  pack_fread(o,0x278,fp);
  iVar1 = generate_options_checksum(o);
  if (iVar1 != o->checksum) {
    reset_options(o);
  }
  set_sort_method(o->sort_method);
}

int load_hisc_table(Thisc_table *table,PACKFILE *fp) {
  char cVar1;
  unsigned int uVar2;
  int c_real;
  char *s;
  char *pcVar3;
  Thisc *pTVar5;
  int local_30;
  unsigned int local_20 [4];

  pTVar5 = table->posts;
  local_30 = 1;
  for(int i = 0; i < 180; i += 36) {
    pack_fread(pTVar5->name + i,36,fp);
    pack_fread(local_20,4,fp);
    pTVar5 = table->posts;
    pcVar3 = pTVar5->name + i;
    uVar2 = *(unsigned int *)(pcVar3 + 32);
    cVar1 = *pcVar3;
    while (cVar1 != '\0') {
      uVar2 = (int)cVar1 + uVar2 * 140;
      cVar1 = *++pcVar3;
    }
    if (local_20[0] != uVar2) {
      local_30 = 0;
    }
  }

  return local_30;
}

void datafile_callback(DATAFILE *d) {
  draw_progress_bar();
  return;
}

void datafile_callback_slow(DATAFILE *d) {
  if ((progress_count & 0xfU) == 0) {
    draw_progress_bar();
  }
  progress_count++;
  return;
}

void clickedCloseButton(void) {
  closeButtonClicked = true;
  return;
}

void switchedToProgram(void) {
  hasFocus = true;
  return;
}

void switchedFromProgram(void) {
  hasFocus = false;
  return;
}

int init_game(int argc, char** argv) {
  BITMAP *pBVar1;
  char *puVar2;
  int i;
  int iVar4;
  Thisc_table *pTVar5;
  PACKFILE *fp_00;
  int depth;
  DATAFILE *dat;
  int whiteColor;
  Tgamepad *pTVar6;
  char *ext;
  Tplayer *pTVar7;
  char *pcVar8;
  LPWSADATA pWVar9;
  DATAFILE *loader;
  int i_1;
  Tgamepad *gp_1;
  PACKFILE *fp;
  BITMAP *fldLogo;
  LPWSADATA lpWSAData;
  char *checkFile;
  Tgamepad *gp;
  int last_cc;
  char *pcVar10;
  bool bVar11;
  time_t tVar12;
  int iVar13;
  char buf [8];
  WSADATA wsaData;
  char cfgfilename [256];
  char title [64];
  char tmpHandle [32];

  tmpHandle[0] = '\0';
  init_ok = 0;
  log2file("\nINIT GAME");
  packfile_password(NULL);
  sprintf(title,"Icy Tower v%s", TOWER_VERSION);
  if (system_driver->set_window_title != NULL) {
    (*system_driver->set_window_title)(title);
  }
  lpWSAData = (LPWSADATA)&wsaData;
  i = WSAStartup(0x202,lpWSAData);
  if (i != 0) {
    log2file(" !!! Failed to setup Winsock");
  }
  
  if (((byte)wsaData.wHighVersion < 2) || (wsaData.wVersion < 2)) {
    log2file(" !!! Failed to get proper Winsock version (wanted 2.2, got %d.%d)", wsaData.wHighVersion, wsaData.wVersion);
  }
  
  play_char.max = 0;
  play_char.value = 0;
  play_char.bmp = NULL;
  eyecandy_selection.value = 0;
  eyecandy_selection.size = 3;
  eyecandy_selection.caption[0] = strdup("Lots");
  eyecandy_selection.caption[1] = strdup("Some");
  eyecandy_selection.caption[2] = strdup("None");
  scroll_speed_selection.value = 0;
  scroll_speed_selection.size = 6;
  scroll_speed_selection.caption[5] = strdup("Normal");
  scroll_speed_selection.caption[4] = strdup("Hasty");
  scroll_speed_selection.caption[3] = strdup("Fast");
  scroll_speed_selection.caption[2] = strdup("Faster");
  scroll_speed_selection.caption[1] = strdup("Fastest");
  scroll_speed_selection.caption[0] = strdup("Insane");
  floor_size_selection.value = 0;
  floor_size_selection.size = 5;
  floor_size_selection.caption[0] = strdup("Wide");
  floor_size_selection.caption[1] = strdup("Normal");
  floor_size_selection.caption[2] = strdup("Shorter");
  floor_size_selection.caption[3] = strdup("Shortest");
  floor_size_selection.caption[4] = strdup("Tiny");
  floor_size_selection.value = 2;
  gravity_selection.value = 0;
  gravity_selection.size = 3;
  gravity_selection.caption[0] = strdup("Helium");
  gravity_selection.caption[1] = strdup("Normal");
  gravity_selection.caption[2] = strdup("Heavy");
  //fldads_start(); Thread dedicated to showing ADS, fuck that!!
  if (argc < 3) {
    if (argc == 2) {
      lpWSAData = (LPWSADATA)argv[1];
      log2file("Loading %s",lpWSAData);
      demo = load_replay(argv[1]);
      if (demo == NULL) {
        lpWSAData = (LPWSADATA)get_filename(argv[1]);
        strcpy(tmpHandle,(char *)lpWSAData);
        pcVar8 = get_extension(tmpHandle);
        pcVar8[-1] = '\0';
        profile = load_profile(tmpHandle);
        if (profile == NULL) {
          tmpHandle[0] = '\0';
          pcVar8 = get_filename(argv[1]);
          printf("The file\n<%s>\nis not a vaild Icy Tower profile.",pcVar8);
          log2file("*** Failed!");
          dropped_file_is_not_a_replay = 1;
          return 0;
        }
        free(profile);
        profile = NULL;
      }
    }
  }
  else {
    lpWSAData = NULL;
    iVar13 = 1;
    for(i = 1; i < argc; i++) {
      pWVar9 = (LPWSADATA)argv[i];
      if (*(char *)&pWVar9->wVersion != '-') {
        lpWSAData = pWVar9;
      }
      iVar4 = stricmp((char *)pWVar9,"-check");
      if (iVar4 == 0) {
        iVar13 = 1;
      }
      iVar4 = strcmp(argv[i],"-jumps");
      if (iVar4 == 0) {
        cmdline.jumps = 1;
      }
      iVar4 = strcmp(argv[i],"-combos");
      if (iVar4 == 0) {
        cmdline.combos = 1;
      }
      iVar4 = strcmp(argv[i],(char *)NULL);
      if (iVar4 == 0) {
        cmdline.sd = 1;
      }
      iVar4 = strcmp(argv[i],"-keys");
      if (iVar4 == 0) {
        cmdline.keys = 1;
      }
      iVar4 = strcmp(argv[i],"-all");
      if (iVar4 == 0) {
        cmdline.keys = 1;
        cmdline.jumps = 1;
        cmdline.combos = 1;
        cmdline.sd = 1;
      }
      iVar4 = strcmp(argv[i],"-tiny");
      if (iVar4 == 0) {
        cmdline.tiny = 1;
      }
    }
    if (iVar13 == 0) {
      printf("<%s>\nis not a vaild option.",argv[1]);
      log2file("*** Erroneous option (%s)",argv[1]);
      dropped_file_is_not_a_replay = 1;
      return 0;
    }
    log2file("Loading %s",argv[2]);
    demo = load_replay((char *)lpWSAData);
    if (demo == NULL) {
      pcVar8 = get_filename((char *)lpWSAData);
      printf("<itrcheck_results status=\"error\">%s</itrcheck_results>\n",pcVar8);
      log2file("*** Failed!");
      dropped_file_is_not_a_replay = 1;
      return 0;
    }
    itrcheck = 1;
    log2file("ITRCHECK activated, checking <%s>",lpWSAData);
  }
  log2file("Creating hiscore tables",lpWSAData);;
  for(i = 0; i < 0xf; i++) {
    pTVar5 = make_hisc_table(hisc_names[i]);
    hisc_tables[i] = pTVar5;
    if (pTVar5 == NULL) {
      log2file(" *** failed")
      printf("Failed reserve memory for highscore table.");
      return 0;
    }
  }
  for(i = 0; i < 0xf; i++) {
    reset_hisc_table(hisc_tables[i],"Harold",1000,0);
  }
  log2file("Initiating controls");
  init_control(&ctrl);
  get_configfile_path(cfgfilename,256);
  log2file("Loading config file");
  fp_00 = pack_fopen(cfgfilename,"rp");
  if (fp_00 == NULL) {
    log2file(" *** failed");
    log2file("Resetting to default config");
    reset_options(&options);
  }
  else {
    load_options(&options,fp_00);
    for(i = 0; i < 0xf; i++) {
      while (load_hisc_table(hisc_tables[i],fp_00) != 0) {
        i++;
        if (i == 0xf) goto LAB_0040ecb2;
      }
      reset_hisc_table(hisc_tables[i],"FLD",1000,0);
    }
    LAB_0040ecb2:
    pack_fclose(fp_00);
  }
  if (tmpHandle[0] != '\0') {
    log2file("Setting last profile");
    strcpy(options.lastProfile,tmpHandle);
  }
  if (itrcheck != 0) goto LAB_0040f1dc;
  options.timesStarted = options.timesStarted + 1;
  i = options.timesStarted;
  log2file("Game started %d times",options.timesStarted);
  if (system_driver->desktop_color_depth != NULL) {
    depth = system_driver->desktop_color_depth();
  }
  set_color_depth(depth);
  if (options.full_screen == 0) {
    log2file("Setting windowed mode 640x480",i);
    if (set_gfx_mode(2,640,480,0,0) < 0) {
      log2file(" *** failed");
      options.full_screen = -1;
      goto LAB_0040ed10;
    }
    window = true;
    if (options.full_screen != 0) goto LAB_0040ed10;
  }
  else {
    LAB_0040ed10:
    log2file("Setting fullscreen mode 640x480",i);
    if (set_gfx_mode(1,640,480,0,0) < 0) {
      log2file(" *** failed")
      printf("Failed to set graphics mode.");
      return 0;
    }
    window = false;
  }
  if (screen == NULL) {
    log2file("ERROR: screen was not set")
    printf(
        "For some reason, the game failed to go into\ngraphics mode. Try starting the gam e again.\n\nIf this problem persists,\nplease visit www.freelunchdesign.com."
    );
    return 0;
  }
  install_mouse();
  enable_hardware_cursor();
  select_mouse_cursor(2);
  if (options.full_screen == 0) {
    show_mouse(screen);
  }
  log2file("Graphics mode set. (screen = %d)",screen);
  i = makecol(180,180,180);
  textprintf_centre_ex(screen,font,320,220,i,-1,"please wait");
  set_color_conversion(0);
  packfile_password("(c) Free Lunch Design");
  dat = load_datafile("data/loading.dat");
  log2file("Loading loader");
  if (dat == NULL) {
    log2file(" *** failed")
    printf("Failed to load loader datafile.");
    return 0;
  }
  packfile_password(NULL);
  log2file("Putting FLD Logo on screen");
  pBVar1 = (BITMAP *)dat[1].dat;
  select_palette((RGB *)dat->dat);
  i = makecol(255,255,255);
  (*screen->vtable->clear_to_color)(screen,i);
  i = 200 - pBVar1->bmHeight / 2;
  iVar13 = 320 - pBVar1->bmWidth / 2;
  if (pBVar1->bmBitsPixel == 8) {
    (*screen->vtable->draw_256_sprite)(screen,pBVar1,iVar13,i);
  }
  else {
    (*screen->vtable->draw_sprite)(screen,pBVar1,iVar13,i);
  }
  unload_datafile(dat);
  log2file("Setting focus modes");
  if (options.full_screen == 0) {
    set_display_switch_mode(3);
  }
  else {
    set_display_switch_mode(4);
  }
  log2file("Setting focus callbacks");
  set_display_switch_callback(0,switchedToProgram);
  set_display_switch_callback(1,switchedFromProgram);
  set_close_button_callback(clickedCloseButton);
  srand(time(NULL));
  log2file("Installing timers");
  draw_progress_bar();
  installtimers();
  cycle_count = 0;
  log2file("Installing keyboard");
  draw_progress_bar();
  install_keyboard();
  log2file("Installing sound");
  draw_progress_bar();
  install_sound(-1,-1,NULL);
  log2file("Installing joystick/gamepad");
  draw_progress_bar();
  i = install_joystick(-1);
  got_joystick = (int)(i == 0);
  if (got_joystick) {
    ctrl.use_joy = 1;
    i = joy[0].num_buttons;
    log2file(" gamepad has %d buttons",joy[0].num_buttons);
    iVar13 = exists("gamepad.txt");
    if (iVar13 == 0) {
      pTVar6 = get_gamepad();
      log2file(" gamepad.txt is missing, setting defaults",i);
      pTVar6->up = 4;
      pTVar6->left = 1;
      pTVar6->right = 2;
      pTVar6->down = 8;
      for(i = 0; i < 32; i++) {
        pTVar6->b[i] = 16;
      }
    }
    else {
      pTVar6 = get_gamepad();
      log2file(" getting values from gamepad.txt",i);
      set_config_file("gamepad.txt");
      pTVar6->up = get_gamepad_value("up");
      pTVar6->left = get_gamepad_value("left");
      pTVar6->right = get_gamepad_value("right");
      pTVar6->down = get_gamepad_value("down");

      for(i = 1; i <= 32; i++) {
        sprintf(buf,"b%d",i);
        iVar13 = get_gamepad_value(buf);
        pTVar6->b[i-1] = iVar13;
      }
    }
  }
  else {
    log2file(" no gamepad or joystick found, play with keyboard only");
  }
  log2file("Reserving memory");
  draw_progress_bar();
  swap_screen = create_bitmap(640,480);
  if (swap_screen == NULL) {
    log2file(" *** failed")
    printf("Failed reserve memory screen buffers.");
    return 0;
  }
  pwd_garble_string(init_string,50);
  log2file("Loading data");
  set_color_conversion(0xffffff);
  draw_progress_bar();
  packfile_password(init_string);
  data = load_datafile_callback("data/data.dat",datafile_callback_slow);
  if (data == NULL) {
    log2file(" *** failed")
    printf("Failed to load datafile.");
    return 0;
  }
  packfile_password(NULL);
  LAB_0040f1dc:
  log2file("Initiating player");
  draw_progress_bar();
  i = rand();
  player_id = i % 1000;
  pTVar7 = (Tplayer *)malloc(184);
  ply[i % 1000] = pTVar7;
  if (ply[player_id] == NULL) {
    log2file(" *** failed")
    printf("Failed to allocate memory for player.");
    i = 0;
  }
  else {
    if (itrcheck == 0) {
      puVar2 = (uint32_t *)data->dat;
      puVar2[2] = 0;
      puVar2[1] = 0;
      *puVar2 = 0;
      gameover_bmp = (BITMAP *)data[0x37].dat;
      log2file("Checking profile directory");
      get_profiles_dir(buf,0x400);
      i = file_exists(buf,-1,NULL);
      if (i == 0) {
        log2file("  does not exist, trying to create");
        _mkdir(buf);
        i = file_exists(buf,-1,NULL);
        if (i == 0) {
          log2file("  *** failed!")
          printf("Failed to create profile directory %s",buf);
          return 0;
        }
      }
      log2file("Checking available profiles");
      draw_progress_bar();
      rebuild_profile_list(NULL);
      log2file("Loading profile");
      draw_progress_bar();
      log2file(" loading \'%s\'",0x4fe77c);
      profile = load_profile(options.lastProfile);
      if (profile == NULL) {
        log2file(" profile not found \'%s\'",0x4fe77c);
        log2file(" trying to load default profile \'%s\'","guest");
        profile = load_profile("guest");
        if (profile == NULL) {
          log2file(" profile not found \'%s\'","guest");
          profile = create_profile("guest",1);
          pcVar8 = profile->handle;
          log2file(" created profile \'%s\'",pcVar8);
          if (profile == NULL) {
            log2file("  *** failed!",pcVar8)
            printf("Failed create profile.");
            return 0;
          }
        }
      }
      strcpy(options.lastProfile,profile->handle);
      syncOptionsFromProfile();
      log2file("Checking available characters");
      draw_progress_bar();
      i = check_characters();
      if (i == 0) {
        log2file(" *** no characters available")
        printf(
            "No characters available.\nPlease reinstall game or add custom characters.\nR efer to readme.txt."
        );
        return 0;
      }
      select_palette((RGB *)data->dat);
      log2file("Loading SFX");
      draw_progress_bar();
      log2file(" loading sounds");
      packfile_password(init_string);
      sfx = load_datafile_callback("data/sfx15.dat",datafile_callback);
      if (sfx == NULL) {
        log2file(" could not load data/sfx15.dat");
        pcVar8 = "no sound";
        pcVar10 = sfx_file;
        for (i = 9; i > 0; i--) {
          *pcVar10 = *pcVar8;
          pcVar8++;
          pcVar10++;
        }
        log2file(" no sounds loaded");
      }
      else {
        pcVar8 = "sfx15.dat";
        pcVar10 = sfx_file;
        for (i = 10; i != 0; i--) {
          *pcVar10 = *pcVar8;
          pcVar8 = pcVar8 + 1;
          pcVar10 = pcVar10 + 1;
        }
        log2file(" sfx15.dat loaded");
      }
      packfile_password(NULL);
      if (sfx != NULL) {
        log2file("Getting sounds from data file");
        draw_progress_bar();
        combo_sound[0] = getSampleFromOggDatafile(sfx,8);
        combo_sound[1] = getSampleFromOggDatafile(sfx,0x12);
        combo_sound[2] = getSampleFromOggDatafile(sfx,9);
        combo_sound[3] = getSampleFromOggDatafile(sfx,0x11);
        combo_sound[4] = getSampleFromOggDatafile(sfx,0x15);
        combo_sound[5] = getSampleFromOggDatafile(sfx,1);
        combo_sound[6] = getSampleFromOggDatafile(sfx,5);
        combo_sound[7] = getSampleFromOggDatafile(sfx,6);
        combo_sound[8] = getSampleFromOggDatafile(sfx,0xf);
        combo_sound[9] = getSampleFromOggDatafile(sfx,0x14);
        bg_beat = getSampleFromOggDatafile(sfx,2);
        bg_menu = getSampleFromOggDatafile(sfx,3);
        speaker[0] = getSampleFromOggDatafile(sfx,10);
        speaker[1] = getSampleFromOggDatafile(sfx,7);
        speaker[2] = getSampleFromOggDatafile(sfx,0x13);
        sounds[0] = NULL;
        sounds[1] = NULL;
        sounds[2] = getSampleFromOggDatafile(sfx,0);
        sounds[3] = NULL;
        sounds[4] = getSampleFromOggDatafile(sfx,0xd);
        sounds[5] = NULL;
        sounds[6] = getSampleFromOggDatafile(sfx,0xe);
        sounds[7] = getSampleFromOggDatafile(sfx,4);
        sounds[8] = getSampleFromOggDatafile(sfx,0x10);
        menu_sounds[0] = getSampleFromOggDatafile(sfx,0xb);
        menu_sounds[1] = getSampleFromOggDatafile(sfx,0xc);
        jump_sound[0] = NULL;
        jump_sound[1] = NULL;
        jump_sound[2] = NULL;
        log2file("Releasing ogg datafile.");
        unload_datafile(sfx);
        sfx = NULL;
      }
      log2file("Setting menu values");
      snd_volume_slider.value = options.snd_volume;
      msc_volume_slider.value = options.msc_volume;
      eyecandy_selection.value = options.flash;
      gravity_selection.value = options.gravity;
      floor_size_selection.value = options.floor_size;
      scroll_speed_selection.value = options.start_speed;
      if (profile->best_floor < 1000) {
        floors.max = profile->best_floor / 100;
      }
      else {
        floors.max = 9;
      }
      floors.value = floors.max;
      if (profile->start_floor < floors.max) {
        floors.value = profile->start_floor;
      }
      log2file("Cleaning up");
      draw_progress_bar();
      log2file("Welcome to Icy Tower");
      draw_progress_bar();
      i = 0;
      while ((iVar13 = keypressed(), iVar13 == 0 && (cycle_count < 0x96))) {
        if ((cycle_count % 10 == 0) && (bVar11 = cycle_count != i, i = cycle_count, bVar11)) {
          draw_progress_bar();
          i = cycle_count;
        }
        rest(2);
      }
      i = rand();
      seed = (double)(i % 2367);
      fadeOut(16);
      clear_bitmap(screen);
      vsync();
      clear_keybuf();
    }
    init_ok = 1;
    i = -1;
  }
  return i;
}

void uninit_game() {
  Tprofile *p;
  int iVar1;
  int i;
  int iVar2;

  log2file("\nUNINIT");
  if (init_ok) {
    log2file("Saving config");
    save_config();
    log2file("Saving profile \'%s\'", profile->handle);
    p = profile;
    profile->msc_volume = options.msc_volume;
    p->snd_volume = options.snd_volume;
    p->jump_hold = options.jump_hold;
    p->flash = options.flash;
    save_profile(p);
  }
  if (testers != NULL) {
    destroy_all(testers);
  }
  log2file("Freeing sound memory");
  iVar1 = 0;
  do {
    if (combo_sound[iVar1] != NULL) {
      destroy_sample(combo_sound[iVar1]);
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 != 10);
  iVar1 = 0;
  do {
    if (jump_sound[iVar1] != NULL) {
      destroy_sample(jump_sound[iVar1]);
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 != 3);
  iVar1 = 0;
  do {
    if (speaker[iVar1] != NULL) {
      destroy_sample(speaker[iVar1]);
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 != 3);
  if (menu_sounds[0] != NULL) {
    destroy_sample(menu_sounds[0]);
  }
  if (menu_sounds[1] != NULL) {
    destroy_sample(menu_sounds[1]);
  }
  iVar1 = 0;
  do {
    if (sounds[iVar1] != NULL) {
      destroy_sample(sounds[iVar1]);
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 != 9);
  if (bg_beat != NULL) {
    destroy_sample(bg_beat);
  }
  if (bg_menu != NULL) {
    destroy_sample(bg_menu);
  }
  log2file("Freeing custom character memory");
  
  if (characters) {
    for(iVar1 = 0, iVar2 = 0; iVar1 < num_chars; iVar1++, iVar2 += sizeof(Tcharacter)) {
      if (characters[iVar1].bmp) {
        destroy_bitmap(characters[iVar1].bmp);
      }
    }
  
    free(characters);
  }
  log2file("Unloading datafile");
  if (data != NULL) {
    unload_datafile(data);
  }
  log2file("Free buffer memory");
  if (swap_screen != NULL) {
    destroy_bitmap(swap_screen);
  }
  log2file("Free highscore tables");
  iVar1 = 0;
  do {
    if (hisc_tables[iVar1] != (Thisc_table *) 0x0) {
      destroy_hisc_table(hisc_tables[iVar1]);
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 != 0xf);
  log2file("Free player");
  if (ply[player_id] != NULL) {
    free(ply[player_id]);

    log2file("Exiting Allegro");
    allegro_exit();
  }
}

int is_any(Tcontrol* c) {
  return ~-(uint32_t)((c->flags & 0xbf) == 0);
}

int play() {
  char cVar1;
  Trecord *pTVar2;
  void (*p_Var3)(struct BITMAP*);
  BITMAP *pBVar4;
  fixed fVar5;
  bool bVar6;
  bool bVar7;
  bool bVar8;
  int iVar9;
  Tgame_data *gd;
  uint32_t uVar10;
  int i;
  int rewResult;
  int iVar11;
  int iVar12;
  int addTime;
  int addTime_1;
  int addTime_2;
  int k;
  char *pcVar13;
  Tprofile *pTVar14;
  Treplay *pTVar15;
  clock_t cVar16;
  int iVar17;
  unsigned int uVar18;
  Tplayer *pTVar19;
  int iVar20;
  int iVar21;
  int k_1;
  int iVar22;
  char uVar23[3];
  int iVar24;
  int scroll_acc;
  Tplayer *pTVar25;
  int iVar26;
  unsigned int uVar27;
  int ffstep;
  int vgp;
  int addTime_3;
  Tparticle *pTVar28;
  int pauseTime;
  char *xmlStr;
  int play_again;
  int diff;
  int p;
  Treplay *rr;
  int qpc_freq;
  Tcontrol *pTVar29;
  int playing;
  int pos;
  int gotHigh;
  unsigned int uVar31;
  uint8_t *puVar32;
  int falling;
  Tcontrol *pTVar33;
  int midY;
  int fc;
  int fc_1;
  int skip_keys;
  int *piVar34;
  char *pcVar35;
  bool bVar36;
  fixed a;
  double clockElapsed;
  time_t tVar37;
  time_t tVar38;
  double dVar39;
  double dVar40;
  double dVar41;
  int *in_stack_fffff650;
  double in_stack_fffff654;
  int in_stack_fffff65c;
  int in_stack_fffff660;
  uint32_t *local_994;
  int local_990;
  int local_98c;
  int local_988;
  int local_984;
  clock_t local_980;
  uint32_t local_97c;
  int local_978;
  int local_974;
  unsigned int local_970;
  int local_96c;
  int local_968;
  int local_960;
  fixed local_95c;
  int local_958;
  int local_954;
  int local_950;
  unsigned int local_94c;
  int local_948;
  int local_944;
  int local_940;
  int local_93c;
  fixed local_938;
  unsigned int local_934;
  int local_92c;
  int qualifyValue [15];
  int qualify [15];
  Tcontrol rec_ctrl;
  int speeds [10] = {
    1500,
    3000,
    4500,
    6000,
    7500,
    9000,
    10500,
    1800000,
    9000000,
    0
  };
  int key_flag [7];
  int last_keys [7];
  int keys_pressed [7];
  Tgd_combo c;
  LARGE_INTEGER li;
  
  /*pTVar29 = &ctrl;
  pTVar33 = &rec_ctrl;
  for (iVar24 = 9; iVar24 != 0; iVar24 = iVar24 + -1) {
    pTVar33->use_joy = pTVar29->use_joy;
    pTVar29 = (Tcontrol *)&pTVar29->key_left;
    pTVar33 = (Tcontrol *)&pTVar33->key_left;
  }*/

  if (itrcheck == 0) {
    //local_994 = (unsigned int *)(profile->best_floor / 100);
    local_990 = get_rank_id(profile);
  }
  else {
    local_994 = NULL;
    local_990 = 0;
  }
  pTVar15 = demo;
  if (recording) {
    demo->tc_posts = 0;
    iVar24 = 0;

    for(int i = 0; i < 100; i++) {
      pTVar15->tc_c_data[i] = 0.0;
      pTVar15->tc_q_data[i] = 0.0;
      pTVar15->tc_t_data[i] = 0.0;
      pTVar15->tc_s_data[i] = 0.0;
      pTVar15->tc_f_data[i] = 0.0;
    }
  }
  log2file(" setting up play data");
  fall_count = 0;
  clock_angle = 0;
  map.offset = 0;
  fast_forward = 0;
  fast_fast_forward = 0;
  update_frame();
  if (itrcheck == 0) {
    draw_frame(swap_screen);
    fadeIn(swap_screen,0x10);
    play_sound(custom.yo,0,0);
    startGameMusic();
    if ((itrcheck == 0) && bg_beat) {
      checkMusicVoiceID = play_sample(bg_beat,0,0x80,1000,1);
    }
  }
  cycle_count = 0;
  log2file(" play started");
  tVar37 = time(NULL);
  local_96c = (int)tVar37;
  QueryPerformanceCounter((LARGE_INTEGER *)&li);
  local_97c = li.u.LowPart;
  QueryPerformanceFrequency((LARGE_INTEGER *)&li);
  local_980 = clock();
  tVar37 = time(NULL);
  local_978 = (int)tVar37;
  if (closeButtonClicked) {
    return 0;
  }
  local_958 = 0;
  local_95c = 0.0;
  local_968 = 0;
  local_93c = 0;
  local_938 = 0.0;
  local_960 = 0;
  local_984 = 0;
  local_98c = 0;
  local_954 = 0;
  local_988 = -1;
  local_94c = 0;
  bVar6 = false;
  local_970 = 0;
  local_92c = 0;
  bVar7 = false;
  bVar36 = true;
  local_950 = 0x32;
  local_974 = 0;
  local_948 = -1;
  bVar8 = false;
LAB_00411c30:
  cycle_count = 0;
  logic_count++;
  local_94c++;
  bVar6 = !bVar6;
  fall_count++;
  local_938++;
  local_93c++;
  if (itrcheck == 0) {
    if (lastFocus != hasFocus) {
      if (hasFocus == 0) {
        if (checkMusicVoiceID >= -1) {
          voice_stop(checkMusicVoiceID);
        }
        checkMusicVoiceID = -1;
        stopGameMusic();
      }
      else {
        if (bg_beat) {
          checkMusicVoiceID = play_sample(bg_beat,0,0x80,1000,1);
        }
        startGameMusic();
        local_958 = 0;
        local_95c = 0.0;
        local_93c = 0;
      }
      lastFocus = hasFocus;
      if (itrcheck != 0) goto LAB_00411d76;
    }
    if (-1 < checkMusicVoiceID) {
      iVar24 = voice_get_position(checkMusicVoiceID);
      if (iVar24 < local_968) {
        local_93c = 0;
      }
      fVar5 = (fixed)iVar24 / 44000.0;
      local_968 = iVar24;
      if (fVar5 <= 0.01) {
        local_95c = ((fixed)local_93c / 50.0) / fVar5 + local_95c;
        local_958++;
      }
    }
  }
LAB_00411d76:
  if ((((recording != 0) && (100 < map.offset)) && (ply[player_id]->dead == 0)) &&
     (local_938 == 1.401298e-42)) {
    cVar16 = _clock();
    fVar5 = ((fixed)(cVar16 - local_980) * 50.0) / 1000.0;
    if (0.0 < fVar5) {
      fVar5 = ((fVar5 * 1000.0) / fVar5) / 20.0;
    }
    else {
      fVar5 = -0.05;
    }
    in_stack_fffff654 = (double)fVar5;
    QueryPerformanceFrequency((LARGE_INTEGER *)&li);
    uVar10 = li.u.LowPart;
    QueryPerformanceCounter((LARGE_INTEGER *)&li);
    dVar41 = (double)((((fixed)(li.u.LowPart - local_97c) * 50.0) / (fixed)uVar10) / 20.0);
    dVar40 = 50.0;
    dVar39 = 20.0;
    tVar37 = time(NULL);
    pTVar15 = demo;
    demo->tc_c_data[demo->tc_posts] = (fixed)in_stack_fffff654 + 0.0;
    pTVar15->tc_q_data[pTVar15->tc_posts] = (fixed)dVar41 + 0.0;
    pTVar15->tc_t_data[pTVar15->tc_posts] =
         ((fixed)dVar40 * (fixed)((int)tVar37 - local_978)) / (fixed)dVar39 + 0.0;
    iVar24 = pTVar15->tc_posts;
    pTVar15->tc_f_data[iVar24] = (fixed)ply[player_id]->level;
    if (local_958 != 0) {
      pTVar15->tc_s_data[iVar24] = ((fixed)dVar40 * local_95c) / (fixed)local_958;
    }
    iVar24 = 99;
    if (pTVar15->tc_posts < 0x62) {
      iVar24 = pTVar15->tc_posts + 1;
    }
    pTVar15->tc_posts = iVar24;
    local_980 = _clock();
    QueryPerformanceCounter((LARGE_INTEGER *)&li);
    local_97c = li.u.LowPart;
    tVar37 = time(NULL);
    local_978 = (int)tVar37;
    local_958 = 0;
    local_95c = 0.0;
    local_938 = 0.0;
  }
  if (debug != 0) {
    if ((key[28] != '\0') && (bVar36)) {
      start_reward(5);
    }
    if ((key[29] != '\0') && (bVar36)) {
      start_reward(7);
    }
    if ((key[30] != '\0') && (bVar36)) {
      start_reward(0xf);
    }
    if ((key[31] != '\0') && (bVar36)) {
      start_reward(0x19);
    }
    if ((key[32] != '\0') && (bVar36)) {
      start_reward(0x23);
    }
    if ((key[33] != '\0') && (bVar36)) {
      start_reward(0x32);
    }
    if ((key[34] != '\0') && (bVar36)) {
      start_reward(0x46);
    }
    if ((key[35] != '\0') && (bVar36)) {
      start_reward(100);
    }
    if ((key[36] != '\0') && (bVar36)) {
      start_reward(0x8c);
    }
    if ((key[27] != '\0') && (bVar36)) {
      start_reward(200);
    }
    if ((((((key[28] != '\0') || (key[29] != '\0')) || (key[30] != '\0')) ||
         (((key[31] != '\0' || (key[32] != '\0')) ||
          ((key[33] != '\0' || ((key[34] != '\0' || (key[35] != '\0')))))))) || (key[36] != '\0'))
       || (bVar36 = true, key[27] != '\0')) {
      bVar36 = false;
    }
  }
  handle_player_input(&ctrl);
  update_player(ply[player_id]);
  if (itrcheck == 0) {
    pTVar19 = ply[player_id];
    if (((pTVar19->rotate != 0) && (pTVar19->in_combo != 0)) && (options.flash == 0)) {
      create_particle(stars,(int)ROUND(pTVar19->x),(int)ROUND(pTVar19->y) + -0x10);
    }
    
    for(int i = 0; stars[i].intensity == 0; i++) {
      if (&stars[i] == (Tparticle *)&characters) break;
      update_particle(stars[i]);
    }
  }
LAB_00411fa4:
  iVar12 = player_id;
  iVar24 = map.offset;
  pTVar19 = ply[player_id];
  dVar41 = pTVar19->y;
  if (dVar41 >= 160.0) {
    if (dVar41 >= 140.0) {
      scroll_acc = 2;
    }
    else {
      scroll_acc = 1;
    }
    if (dVar41 >= 120.0) {
      scroll_acc++;
    }
    if (dVar41 >= 100.0) {
      scroll_acc++;
    }
    if (dVar41 >= 80.0) {
      scroll_acc++;
    }
    if (dVar41 >= 60.0) {
      scroll_acc++;
    }
    if (dVar41 >= 40.0) {
      scroll_acc += 2;
    }
    if (dVar41 >= 20.0) {
      scroll_acc += 2;
    }
    if (dVar41 >= 0.0) {
      scroll_acc += 3;
    }
    map.offset = scroll_acc + map.offset;
    pTVar19 = ply[player_id];
    pTVar19->y = pTVar19->y + (double)scroll_acc;
  }
  else {
    scroll_acc = 0;
  }
  pTVar15 = demo;
  iVar11 = pTVar19->dead;
  if (iVar11 == 0) {
    clock_angle = clock_angle + 1;
    iVar11 = ply[iVar12]->dead;
  }
  if (map.offset < 0x65) {
    if (iVar11 == 0) {
      clock_angle = 0;
      fall_count = 0;
    }
  }
  else if (iVar11 == 0) {
    if (local_948 == -1) {
      local_948 = start_speeds[demo->start_speed];
    }
    if (local_948 == 0) {
      if (bVar6) {
        map.offset = map.offset + 1;
        scroll_acc = scroll_acc + 1;
        ply[iVar12]->y = ply[iVar12]->y + 1.0;
      }
    }
    else {
      map.offset = map.offset + local_948;
      scroll_acc = scroll_acc + local_948;
      ply[iVar12]->y = ply[iVar12]->y + (double)local_948;
    }
  }
  if (hurry_y + 99U < 579) {
    hurry_y -= 2;
  }
  any13 = scroll_acc;
  if (((pTVar15->speed_increase != 0) && (pTVar19 = ply[iVar12], pTVar19->dead == 0)) &&
     ((speeds[local_974] < fall_count && (local_948 < 5)))) {
    pTVar19->ccc[local_974] = pTVar19->level;
    local_974 = local_974 + 1;
    local_948 = local_948 + 1;
    hurry_y = 479;
    play_sound(speaker[0],0,0);
    play_sound(sounds[4],0,0);
  }
  if ((local_948 == 5) && (fall_count -= 45, ply[player_id]->dead == 0)) {
    clock_angle = clock_angle - 45;
    if (-1 < iVar24) goto LAB_004120c1;
LAB_00412aa5:
    uVar31 = ((iVar24 & 0x8000000fU) - 1 | 0xfffffff0) + 1;
  }
  else {
    if (iVar24 < 0) goto LAB_00412aa5;
LAB_004120c1:
    uVar31 = iVar24 & 0x8000000f;
  }
  uVar27 = map.offset & 0x8000000f;
  if ((int)uVar27 < 0) {
    uVar27 = ((map.offset & 0x8000000fU) - 1 | 0xfffffff0) + 1;
  }
  if (((int)uVar27 < (int)uVar31) || (0xf < scroll_acc)) {
    add_floor(&map);
  }
  if ((uint32_t)collision_type < 5) {
                    /* WARNING: Could not recover jumptable at 0x004125a3. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    iVar24 = collision_types[collision_type]();
    return iVar24;
  }
  allegro_message("unknown collision type");
  pTVar19 = ply[player_id];
  if (pTVar19->rotate != 0) {
    pTVar19->angle = pTVar19->angle + 0x80000;
  }
  if (pTVar19->in_combo != 0) {
    pTVar19->in_combo--;
    pTVar19 = ply[player_id];
    if ((pTVar19->in_combo == 0) && (1 < pTVar19->acc_jumps)) {
      iVar24 = pTVar19->acc_level;
      pTVar19->score = pTVar19->score + iVar24 * iVar24;
      iVar24 = start_reward(iVar24);
      if ((recording != 0) && (is_playing_custom_game == 0)) {
        profile->rewards[iVar24] = profile->rewards[iVar24] + 1;
      }
      c.length = ply[player_id]->acc_level;
      local_98c += c.length;
      local_954++;
      c.start = gdComboStart;
      c.end = gdComboStart + c.length;
      add_combo(gameData,&c);
      pTVar19 = ply[player_id];
      iVar24 = pTVar19->acc_level;
      pTVar19->latest_combo = iVar24;
      if (pTVar19->best_combo < iVar24) {
        pTVar19->best_combo = iVar24;
      }
    }
  }
  if (pTVar19->status == 0) {
    iVar24 = get_level(&map,(int)floor(pTVar19->y));
    iVar12 = (iVar24 - 1) / 5;
    pTVar19 = ply[player_id];
    iVar24 = pTVar19->level;
    diff = iVar12 - iVar24;
    if (diff != 0) {
      if (diff == gdLastJumpDiff) {
        jumpSequence.num = jumpSequence.num + 1;
      }
      else {
        jumpSequence.dist = gdLastJumpDiff;
        add_jump_sequence(gameData,&jumpSequence);
        jumpSequence.num = 1;
        jumpSequence.start = iVar12 - diff;
      }
      pTVar19 = ply[player_id];
      iVar24 = pTVar19->level;
      gdLastJumpDiff = diff;
    }
    iVar11 = player_id;
    if (iVar12 < iVar24) {
      if (pTVar19->in_combo != 0) {
        pTVar19->in_combo = 1;
        pTVar19 = ply[player_id];
      }
      iVar22 = player_id;
      iVar21 = 0;
      iVar24 = 40;
      iVar17 = pTVar19->jc[0];
      iVar11 = 32;
      if (iVar17 <= pTVar19->jcTop[0]) goto LAB_00412b2a;
      do {
        pTVar19->ccc[iVar11 + -0x1b] = iVar17;
        pTVar19 = ply[iVar22];
LAB_00412b2a:
        pTVar19->ccc[iVar24 + -0x1e] = 0;
        iVar20 = iVar21;
        while( true ) {
          iVar21 = iVar20 + 1;
          if (iVar21 == 5) {
            pTVar19 = ply[player_id];
            local_960 = 0;
            goto LAB_00412b70;
          }
          pTVar19 = ply[iVar22];
          iVar24 = iVar20 + 0x29;
          iVar17 = pTVar19->jc[iVar20 + 1];
          iVar11 = iVar20 + 0x21;
          if (pTVar19->jcTop[iVar20 + 1] < iVar17) break;
          pTVar19->jc[iVar20 + 1] = 0;
          iVar20 = iVar21;
        }
      } while( true );
    }
    iVar24 = iVar12 - iVar24;
    if (iVar24 == local_960) {
LAB_00413186:
      if (iVar24 < 1) {
        pTVar19 = ply[player_id];
        goto LAB_00412930;
      }
      if (iVar24 < 6) {
        piVar30 = (int *)((int)pTVar19 + iVar24 * 4 + 0x9c);
        *piVar30 = *piVar30 + 1;
        pTVar19 = ply[player_id];
        if (iVar24 != 1) goto LAB_004131be;
        local_960 = 1;
        goto LAB_00412939;
      }
LAB_004131be:
      if (pTVar19->in_combo == 0) {
        pTVar19->acc_level = iVar24;
        pTVar19 = ply[player_id];
        pTVar19->acc_jumps = 1;
        pTVar19->in_combo = 100;
      }
      else {
        pTVar19->acc_level = pTVar19->acc_level + iVar24;
        pTVar19 = ply[player_id];
        piVar30 = &pTVar19->acc_jumps;
        *piVar30 = *piVar30 + 1;
        pTVar19->in_combo = 100;
      }
      pTVar19 = ply[player_id];
      local_960 = iVar24;
LAB_00412953:
      if (pTVar19->in_combo == 0) goto LAB_0041295e;
    }
    else {
      if (iVar24 != 0) {
        iVar26 = 0;
        iVar17 = 40;
        iVar22 = pTVar19->jc[0];
        iVar21 = 32;
        iVar20 = iVar26;
        if (iVar22 <= pTVar19->jcTop[0]) goto LAB_00413746;
        do {
          pTVar19->ccc[iVar21 - 27] = iVar22;
          pTVar19 = ply[iVar11];
          iVar20 = iVar26;
LAB_00413746:
          do {
            pTVar19->ccc[iVar17 - 30] = 0;
            iVar26 = iVar20 + 1;
            if (iVar26 == 5) {
              pTVar19 = ply[player_id];
              local_960 = 0;
              goto LAB_00413186;
            }
            pTVar19 = ply[iVar11];
            iVar17 = iVar20 + 41;
            iVar22 = pTVar19->jc[iVar20 + 1];
            iVar21 = iVar20 + 33;
            iVar9 = iVar20 + 1;
            iVar20 = iVar26;
          } while (iVar22 <= pTVar19->jcTop[iVar9]);
        } while( true );
      }
LAB_00412930:
      if (iVar24 != 1) goto LAB_00412953;
LAB_00412939:
      if (pTVar19->in_combo != 0) {
        pTVar19->in_combo = 1;
        pTVar19 = ply[player_id];
        goto LAB_00412953;
      }
LAB_0041295e:
      pTVar19 = ply[player_id];
      gdComboStart = iVar12;
    }
LAB_00412b70:
    pTVar19->level = iVar12;
    if (local_954 == 0) {
      pTVar19 = ply[player_id];
      if (pTVar19->no_combo_top_floor < pTVar19->level) {
        pTVar19->no_combo_top_floor = gdComboStart;
      }
    }
    else {
      pTVar19 = ply[player_id];
    }
  }
  dVar41 = pTVar19->y;
  if (dVar41 <= 540.0)
  {
    iVar24 = -(uint32_t)(itrcheck == 0);
    if ((pTVar19->in_combo != 0) && (1 < pTVar19->acc_jumps)) {
      pTVar19->biggest_lost_combo = pTVar19->acc_level;
      pTVar19 = ply[player_id];
    }
    pTVar19->in_combo = 0;
    ply[player_id]->dead = 1;
    play_sound(custom.falling,0,1);
    tVar37 = time(NULL);
    iVar12 = player_id;
    local_984 = (int)tVar37;
    pTVar19 = ply[player_id];
    iVar11 = 0;
    
    for(int i = 0; i < 5; i++) {
      if(ply[player_id]->jcTop[i] < ply[player_id]->jc[i]) {
        ply[player_id]->jcTop[i] = ply[player_id]->jc[i];
      }
      ply[player_id]->jc[i] = 0;
    }

    jumpSequence.dist = gdLastJumpDiff;
    add_jump_sequence(gameData,&jumpSequence);
    if (local_954 == 0) {
      pTVar19 = ply[player_id];
      if (pTVar19->no_combo_top_floor < pTVar19->level) {
        pTVar19->no_combo_top_floor = pTVar19->level;
      }
      dVar41 = pTVar19->y;
      local_960 = 0;
      local_92c = 1;
    }
    else {
      pTVar19 = ply[player_id];
      dVar41 = pTVar19->y;
      local_960 = 0;
      local_92c = 1;
    }
  }
  else {
    iVar24 = -1;
  }
  if (dVar41 <= 900.0) {
    play_sound(speaker[1],0,0);
    pTVar19 = ply[player_id];
    bVar7 = true;
  }
  if(local_92c != 0) {
    local_92c++;
  }
  iVar12 = pTVar19->level;
  if ((local_92c >= iVar12 * 5) || (local_92c >= 250)) {
    play_sound(sounds[6],0,1);
    if (custom.falling != NULL) {
      stop_sample(custom.falling);
    }
    pTVar19 = ply[player_id];
    pTVar19->shake = 0x18;
    iVar12 = pTVar19->level;
    local_92c = 0;
  }
  if (local_950 <= iVar12) {
    play_sound(sounds[2],0,0);
    if ((options.flash == 0) && (0 < local_950 / 2)) {
      iVar12 = 0;
      do {
        iVar11 = new_rand();
        iVar11 = create_particle(stars,iVar11 % 600 + 0x14,480);
        iVar17 = new_rand();
        stars[iVar11].sy = -((iVar17 % 200 << 0x10) / 10);
        iVar12 = iVar12 + 1;
      } while (iVar12 < local_950 / 2);
    }
    if (local_950 < 1000) {
      local_950 = local_950 + 0x32;
      pTVar19 = ply[player_id];
    }
    else {
      local_950 = local_950 + 500;
      pTVar19 = ply[player_id];
    }
  }
  if (pTVar19->edge == 0) {
    pTVar19->edge_drawn = 0;
    pTVar19 = ply[player_id];
  }
  iVar12 = pTVar19->edge_drawn;
  if (iVar12 != 0) {
    if (iVar12 == 0xb) {
      if (pTVar19->status != 0) goto LAB_004123d8;
      play_sound(custom.edge,1,1);
      pTVar19 = ply[player_id];
      iVar12 = pTVar19->edge_drawn;
    }
    if (iVar12 == 50) {
      pTVar19->edge_drawn = 0;
    }
  }
LAB_004123d8:
  if (debug == 0) {
    if ((recording != 0) && (100 < ply[player_id]->dead)) goto LAB_00412550;
  }
  else if (ply[player_id]->dead < 100) {
LAB_00412550:
    iVar24 = 0;
  }
  if ((itrcheck == 0) && (key[47] != '\0')) {
    tVar37 = time(NULL);
    take_screenshot(swap_screen);
    do {
    } while (key[47] != '\0');
    tVar38 = time(NULL);
    iVar12 = (int)tVar38 - (int)tVar37;
    if (0 < iVar12) {
      local_96c = local_96c + iVar12;
    }
    if (-1 < checkMusicVoiceID) {
      iVar12 = voice_get_position(checkMusicVoiceID);
      local_93c = (int)ROUND(((fixed)iVar12 * 50.0) / 44000.0);
      local_958 = 0;
      local_95c = 0.0;
    }
    local_980 = _clock();
    QueryPerformanceCounter((LARGE_INTEGER *)&li);
    local_97c = li.u.LowPart;
    tVar37 = time(NULL);
    local_978 = (int)tVar37;
    local_938 = 0.0;
  }
  iVar12 = ply[player_id]->shake;
  if (iVar12 != 0) {
    ply[player_id]->shake = iVar12 + -1;
    local_970 = new_rand();
    local_970 = local_970 & 0x80000007;
    if ((int)local_970 < 0) {
      local_970 = (local_970 - 1 | 0xfffffff8) + 1;
    }
  }
  update_frame();
  if (bVar8) {
    bVar8 = true;
  }
  else if (closeButtonClicked != 0) {
    bVar8 = true;
    iVar24 = 0;
  }
  if (recording != 0) {
    if (key[59] != '\0') {
      if (ply[player_id]->dead == 0) {
        tVar37 = time(NULL);
        iVar11 = clock_angle;
        iVar12 = fall_count;
        log2file("  game paused with esc");
        iVar17 = 0;
        for(int i = 0; i < 640; i += 2) {
          vline(swap_screen,iVar17,0,480,0);
          hline(swap_screen,0,iVar17,640,0);
        }
        textout_centre_ex(swap_screen,(FONT *)data[0x32].dat,"DO YOU REALLY WANT TO EXIT?",320,
                          0xa0,-1,-1);
        textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,"Press any key to resume",320,210,-1
                          ,-1);
        textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,"Press ESC to exit",320,240,-1,-1);
        blit_to_screen(swap_screen);
        in_stack_fffff650 = (int *)0x1;
        play_sound(custom.wazup,0,1);
        poll_control(&ctrl,0);
        while ((is_any(&ctrl) || is_pause(&ctrl)) || (!closeButtonClicked && (key[59] != '\0'))) {
          poll_control(&ctrl,0);
          rest(2);
        }
        clear_keybuf();
        while( true ) {
          iVar17 = keypressed();
          if (iVar17 != 0) goto LAB_00412fa0;
          iVar17 = is_any(&ctrl);
          if (iVar17 != 0) goto LAB_00412fa0;
          iVar17 = is_pause(&ctrl);
          if (iVar17 != 0) goto LAB_00412fa0;
          if (closeButtonClicked != 0) goto LAB_00412fba;
          if (key[59] != '\0') break;
          poll_control(&ctrl,0);
          rest(2);
        }
        do {
          iVar17 = is_pause(&ctrl);
          if (iVar17 == 0) break;
          poll_control(&ctrl,0);
          rest(2);
LAB_00412fa0:
        } while (closeButtonClicked == 0);
LAB_00412fba:
        if (key[59] != '\0') {
          log2file("  game quit from esc pause");
          profile->games_quit = profile->games_quit + 1;
          tVar38 = time(NULL);
          local_984 = (int)tVar38;
          bVar8 = true;
          iVar24 = 0;
        }
        clear_keybuf();
        fall_count = iVar12;
        clock_angle = iVar11;
        log2file("  game unpaused");
        tVar38 = time(NULL);
        iVar12 = (int)tVar38 - (int)tVar37;
        if (0 < iVar12) {
          local_96c = local_96c + iVar12;
        }
        if (-1 < checkMusicVoiceID) {
          iVar12 = voice_get_position(checkMusicVoiceID);
          local_93c = (int)ROUND(((fixed)iVar12 * 50.0) / 44000.0);
          local_958 = 0;
          local_95c = 0.0;
        }
        local_980 = _clock();
        QueryPerformanceCounter((LARGE_INTEGER *)&li);
        local_97c = li.u.LowPart;
        tVar37 = time(NULL);
        local_978 = (int)tVar37;
        local_938 = 0.0;
      }
      else {
        log2file("  player quit after dying");
        iVar24 = 0;
      }
    }
    iVar12 = is_pause(&ctrl);
    if ((iVar12 != 0) && (ply[player_id]->dead == 0)) {
      tVar37 = time(NULL);
      iVar11 = clock_angle;
      iVar12 = fall_count;
      log2file("  game paused with pause key");
      iVar17 = 0;
      for(int i = 0; i < 640; i += 2) {
        vline(swap_screen,iVar17,0,480,0);
        hline(swap_screen,0,iVar17,640,0);
      }
      textout_centre_ex(swap_screen,(FONT *)data[0x32].dat,"Game Paused",0x140,0xa0,-1,-1);
      textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,"Press any key to resume",0x140,0xd2,-1,
                        -1);
      blit_to_screen(swap_screen);
      play_sound(custom.wazup,0,1);
      poll_control(&ctrl,0);
      while ((iVar17 = is_any(&ctrl), iVar17 != 0 || (iVar17 = is_pause(&ctrl), iVar17 != 0))) {
        poll_control(&ctrl,0);
        rest(2);
      }
      clear_keybuf();
      while ((((iVar17 = keypressed(), iVar17 == 0 && (iVar17 = is_any(&ctrl), iVar17 == 0)) &&
              (iVar17 = is_pause(&ctrl), iVar17 == 0)) && (key[59] == '\0'))) {
        poll_control(&ctrl,0);
        rest(2);
      }
      poll_control(&ctrl,0);
      while ((iVar17 = is_pause(&ctrl), iVar17 != 0 || (key[59] != '\0'))) {
        poll_control(&ctrl,0);
        rest(2);
      }
      fall_count = iVar12;
      clock_angle = iVar11;
      log2file("  game unpaused");
      tVar38 = time(NULL);
      iVar12 = (int)tVar38 - (int)tVar37;
      if (0 < iVar12) {
        local_96c = local_96c + iVar12;
      }
      if (-1 < checkMusicVoiceID) {
        iVar12 = voice_get_position(checkMusicVoiceID);
        local_93c = (int)ROUND(((fixed)iVar12 * 50.0) / 44000.0);
        local_958 = 0;
        local_95c = 0.0;
      }
      local_980 = _clock();
      QueryPerformanceCounter((LARGE_INTEGER *)&li);
      local_97c = li.u.LowPart;
      tVar37 = time(NULL);
      local_978 = (int)tVar37;
      local_938 = 0.0;
    }
    goto LAB_004124aa;
  }
  if (itrcheck == 0) {
    poll_control(&rec_ctrl,0);
    if (ply[player_id]->dead != 0) {
      log2file("  replay ended after death");
      iVar24 = 0;
    }
    if (key[59] != '\0') {
      log2file("  quit from replay");
      bVar8 = true;
      iVar24 = 0;
    }
    if (key[75] == '\0') {
LAB_0041314d:
      if (key[83] != '\0') goto LAB_004126cb;
LAB_00413160:
      fast_forward = 0;
    }
    else {
      if (ply[player_id]->dead == 0) {
        log2file("  replay paused");
        while (key[75] != '\0') {
          poll_control(&rec_ctrl,1);
        }
        while (((key[75] == '\0' && (key[83] == '\0')) && ((key[59] == '\0' && (key[84] == '\0')))))
        {
          poll_control(&rec_ctrl,1);
          if (key[47] != '\0') {
            take_screenshot(swap_screen);
            do {
            } while (key[47] != '\0');
          }
        }
        while (key[75] != '\0') {
          poll_control(&rec_ctrl,1);
        }
        fast_forward = 0;
        fast_fast_forward = 0;
        log2file("  replay unpaused");
        goto LAB_0041314d;
      }
      if (key[83] == '\0') goto LAB_00413160;
LAB_004126cb:
      fast_forward = fast_forward + 1;
      fast_fast_forward = 0;
    }
    if (key[84] == '\0') {
      if ((local_988 <= ply[player_id]->level) || (ply[player_id]->dead != 0)) goto LAB_00412754;
    }
    else {
      pTVar19 = ply[player_id];
      if ((pTVar19->dead == 0) && (iVar12 = demo->floor + -10, pTVar19->level < iVar12)) {
        fast_fast_forward = fast_fast_forward + 1;
        fast_forward = 0;
        local_988 = ((pTVar19->level + 100) / 100) * 100;
        if (local_988 - iVar12 != 0 && iVar12 <= local_988) {
          local_988 = iVar12;
        }
      }
      else {
LAB_00412754:
        fast_fast_forward = 0;
        local_988 = -1;
      }
    }
LAB_004124aa:
    if (itrcheck == 0) {
      static int someCounter = 0;
      someCounter++;
      iVar12 = (-(uint32_t)(fast_forward == 0) & 0xfffffffd) + 4;
      if (fast_fast_forward != 0) {
        iVar12 = 0x20;
      }
      if ((!bVar8) && (someCounter % iVar12 == 0)) {
        draw_frame(swap_screen);
        if (ply[player_id]->shake == 0) {
          blit_to_screen(swap_screen);
        }
        else {
          p_Var3 = screen->vtable->acquire;
          if (p_Var3 != NULL) {
            (*p_Var3)(screen);
          }
          blit(swap_screen,swap_screen,0,local_970,0,0,swap_screen->bmWidth,swap_screen->bmHeight);
          blit_to_screen(swap_screen);
          p_Var3 = screen->vtable->release;
          if (p_Var3 != NULL) {
            (*p_Var3)(screen);
          }
        }
        if (debug == 0) {
          while (cycle_count == 0) {
            rest(2);
          }
        }
        else if ((key[64] == '\0') || (key[115] == '\0')) {
          while (cycle_count == 0) {
            rest(2);
          }
        }
        else {
          do {
            if (7 < cycle_count) break;
          } while (cycle_count < 8);
        }
      }
      if (itrcheck == 0) {
        rest(2);
      }
    }
  }
  gd = gameData;
  if (iVar24 != 0) {
    if (closeButtonClicked != 0) {
      return 0;
    }
    goto LAB_00411c30;
  }
  if (recording == 0) {
    pTVar19 = ply[player_id];
    gameData->score = pTVar19->level * 10 + pTVar19->score;
    gd->floor = pTVar19->level;
    gd->combo = pTVar19->best_combo;
    gd->no_combo_top_floor = pTVar19->no_combo_top_floor;
    gd->biggest_lost_combo = pTVar19->biggest_lost_combo;
    gd->ccc[0] = pTVar19->ccc[0];
    gd->ccc[1] = pTVar19->ccc[1];
    gd->ccc[2] = pTVar19->ccc[2];
    gd->ccc[3] = pTVar19->ccc[3];
    gd->ccc[4] = pTVar19->ccc[4];
    gd->jc[0] = pTVar19->jcTop[0];
    gd->jc[1] = pTVar19->jcTop[1];
    gd->jc[2] = pTVar19->jcTop[2];
    gd->jc[3] = pTVar19->jcTop[3];
    gd->jc[4] = pTVar19->jcTop[4];
    
    memset(keys_pressed, 0, sizeof(int)*7);
    
    key_flag[0] = 1;
    key_flag[1] = 2;
    key_flag[2] = 4;
    key_flag[3] = 8;
    key_flag[4] = 16;
    key_flag[5] = 32;
    key_flag[6] = 128;

    iVar24 = demo->size;
    if (0 < iVar24) {
      memset(last_keys, 0, sizeof(int)*7);
      pTVar2 = demo->data;
      iVar12 = 0;
      do {
        uVar31 = (uint)pTVar2[iVar12].key_flags;
        iVar11 = 0;
        do {
          while (last_keys[iVar11] != 0) {
            uVar27 = key_flag[iVar11] & uVar31;
LAB_004138a1:
            last_keys[iVar11] = uVar27;
            iVar11 = iVar11 + 1;
            if (iVar11 == 7) goto LAB_004138c5;
          }
          uVar27 = uVar31 & key_flag[iVar11];
          if (uVar27 == 0) goto LAB_004138a1;
          keys_pressed[iVar11] = keys_pressed[iVar11] + 1;
          last_keys[iVar11] = uVar27;
          iVar11 = iVar11 + 1;
        } while (iVar11 != 7);
LAB_004138c5:
        iVar12 = iVar12 + 1;
      } while (iVar12 != iVar24);
    }
    gd->jump = keys_pressed[0];
    gd->left = keys_pressed[1];
    gd->right = keys_pressed[2];
    if (itrcheck != 0) {
      pcVar13 = getGameDataXML(gd);
      _printf("%s",pcVar13);
      _free(pcVar13);
      if (itrcheck != 0) {
        return 0;
      }
    }
  }
  else {
    addTime_3 = local_984 - local_96c;
    if (0 < addTime_3) {
      profile->seconds_spent_playing = profile->seconds_spent_playing + addTime_3;
    }
  }
  log2file(" play ended");
  pTVar14 = profile;
  pTVar15 = demo;
  fast_forward = 0;
  fast_fast_forward = 0;
  if (recording == 0) {
    profile->msc_volume = options.msc_volume;
    pTVar14->snd_volume = options.snd_volume;
    pTVar14->jump_hold = options.jump_hold;
    pTVar14->flash = options.flash;
    save_profile(pTVar14);
    if (bVar8) {
LAB_00413a3d:
      rr = NULL;
      goto LAB_00413a3f;
    }
LAB_00413e60:
    if (closeButtonClicked != 0) goto LAB_00413a3d;
    iVar24 = 0;
    do {
      qualify[iVar24] = 0;
      iVar24 = iVar24 + 1;
    } while (iVar24 != 0xf);
    pTVar19 = ply[player_id];
    qualifyValue[0] = pTVar19->level * 10 + pTVar19->score;
    qualifyValue[2] = pTVar19->level;
    qualifyValue[1] = pTVar19->best_combo;
    qualifyValue[3] = pTVar19->biggest_lost_combo;
    qualifyValue[4] = pTVar19->no_combo_top_floor;
    iVar24 = 0;
    do {
      qualifyValue[iVar24 + 5] = pTVar19->ccc[iVar24];
      qualifyValue[iVar24 + 10] = pTVar19->jcTop[iVar24];
      iVar24 = iVar24 + 1;
    } while (iVar24 != 5);
    iVar24 = 0;
    local_940 = 0;
    do {
      iVar12 = qualify_hisc_table(hisc_tables[iVar24],qualifyValue[iVar24]);
      qualify[iVar24] = iVar12;
      local_940 = local_940 + iVar12;
      iVar24 = iVar24 + 1;
    } while (iVar24 != 0xf);
    if (recording == 0) {
      local_940 = 0;
      local_934 = 0x37;
    }
    else {
      local_934 = 0x3e;
      if (local_940 < 1) {
        local_934 = 0x37;
      }
    }
    if (is_playing_custom_game != 0) {
      local_934 = 0x37;
    }
    iVar24 = local_934;
    if ((local_940 == 0) || (is_playing_custom_game != 0)) {
      log2file(" player did not qualify for highscore");
      play_sound(speaker[1],0,0);
    }
    else {
      log2file(" player qualified for highscore");
      play_sound(sounds[7],0,0);
    }
    if (debug == 0) {
      puVar32 = &DAT_004d60a4;
      piVar30 = last_keys;
      for (iVar12 = 0x1f; iVar12 != 0; iVar12 = iVar12 + -1) {
        *(undefined1 *)piVar30 = *puVar32;
        puVar32 = puVar32 + 1;
        piVar30 = (int *)((int)piVar30 + 1);
      }
      uVar31 = 0xffffffff;
      piVar30 = last_keys;
      do {
        if (uVar31 == 0) break;
        uVar31 = uVar31 - 1;
        cVar1 = *(char *)piVar30;
        piVar30 = (int *)((int)piVar30 + 1);
      } while (cVar1 != '\0');
      uVar31 = ~uVar31 - 2;
      c.start = 0x2e002e;
      c.end = 0x2e;
      iVar12 = _stricmp(profile->handle,"guest");
      bVar36 = iVar12 == 0;
      local_938 = 480.0;
      do {
        cycle_count = 0;
        ply[player_id]->dead = ply[player_id]->dead + -0x10;
        local_938 = (130.0 - local_938) * 0.1 + local_938;
        update_frame();
        pTVar28 = stars;
        do {
          while (pTVar28->intensity == 0) {
            pTVar28 = pTVar28 + 1;
            if (pTVar28 == (Tparticle *)&characters) goto LAB_00414658;
          }
          update_particle(pTVar28);
          pTVar28 = pTVar28 + 1;
        } while (pTVar28 != (Tparticle *)&characters);
LAB_00414658:
        if (hurry_y + 99U < 0x243) {
          hurry_y = hurry_y + -2;
        }
        draw_frame(swap_screen);
        uVar27 = 0;
        if (is_playing_custom_game == 0) {
          uVar27 = (uint)(recording != 0);
        }
        draw_results(swap_screen,(BITMAP *)data[local_934].dat,(int)ROUND(local_938),qualify,
                     qualifyValue,uVar27);
        if ((((bVar36) && (local_940 != 0)) && (is_playing_custom_game == 0)) && (recording != 0)) {
          textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,"Enter your initials",0x140,
                            (int)ROUND(local_938 + local_938 + 80.0),-1,-1);
        }
        local_92c = (local_92c + 1) - (uint)(local_92c == 0);
        pTVar19 = ply[player_id];
        if ((pTVar19->level * 5 < local_92c) || (0xfa < local_92c)) {
          play_sound(sounds[6],0,1);
          if (custom.falling != (SAMPLE *)0x0) {
            stop_sample(custom.falling);
          }
          pTVar19 = ply[player_id];
          pTVar19->shake = 0x18;
          local_92c = 0;
        }
        if (pTVar19->shake == 0) {
          blit_to_screen(swap_screen);
        }
        else {
          p_Var3 = screen->vtable->acquire;
          if (p_Var3 != (_func_void_BITMAP_ptr *)0x0) {
            (*p_Var3)(screen);
          }
          iVar12 = swap_screen->h;
          iVar11 = swap_screen->w;
          iVar17 = new_rand();
          blit(swap_screen,screen,0,iVar17 % 8,0,0,iVar11,iVar12);
          p_Var3 = screen->vtable->release;
          if (p_Var3 != (_func_void_BITMAP_ptr *)0x0) {
            (*p_Var3)(screen);
          }
          ply[player_id]->shake = ply[player_id]->shake + -1;
        }
        if (key[47] != '\0') {
          take_screenshot(swap_screen);
          do {
          } while (key[47] != '\0');
        }
        if ((key[64] == '\0') || (key[115] == '\0')) {
          while (cycle_count == 0) {
            rest(2);
          }
        }
      } while (140.0 < local_938);
      ply[player_id]->dead = 0;
      clear_keybuf();
      if (recording == 0) {
        summary_scroller_message[0] = '\0';
      }
      else if (is_playing_custom_game == 0) {
        if (local_940 == 0) {
          pcVar13 = "You\'re playing in guest mode. Start a profile and record your progress!";
          if (!bVar36) {
            iVar12 = new_rand();
            pcVar13 = hints[iVar12 % 0x2d];
          }
        }
        else {
          pcVar13 = "New personal records!    ";
          pcVar35 = summary_scroller_message;
          for (iVar12 = 0x1a; iVar12 != 0; iVar12 = iVar12 + -1) {
            *pcVar35 = *pcVar13;
            pcVar13 = pcVar13 + 1;
            pcVar35 = pcVar35 + 1;
          }
          pcVar13 = "You\'re playing in guest mode. Start a profile and record your progress!";
          if (!bVar36) {
            iVar12 = new_rand();
            pcVar13 = hints[iVar12 % 0x2d];
          }
        }
        _strcpy(summary_scroller_message,pcVar13);
      }
      else {
        pcVar13 = 
        "Custom mode is crazy fun but does not add to your profile. Play Classic Mode to compete in the highscore lists and climb in rank!"
        ;
        pcVar35 = summary_scroller_message;
        for (iVar12 = 0x82; iVar12 != 0; iVar12 = iVar12 + -1) {
          *pcVar35 = *pcVar13;
          pcVar13 = pcVar13 + 1;
          pcVar35 = pcVar35 + 1;
        }
      }
      init_scroller(&summary_scroller,(FONT *)data[0x36].dat,summary_scroller_message,640,0x1e,-1)
      ;
      scroll_scroller(&summary_scroller,-0x96);
      iVar11 = get_rank_id(profile);
      local_934 = 0;
      local_944 = 0x244;
      uVar27 = 0;
      iVar17 = 0;
      local_93c = 0x14;
      iVar12 = -0x14;
      do {
        if (closeButtonClicked != 0) {
          return 0;
        }
        cycle_count = 0;
        local_94c = local_94c + 1;
        update_frame();
        if (key[47] != '\0') {
          take_screenshot(swap_screen);
          do {
          } while (key[47] != '\0');
        }
        if (hurry_y + 99U < 0x243) {
          hurry_y = hurry_y + -2;
        }
        draw_frame(swap_screen);
        uVar18 = 0;
        if (is_playing_custom_game == 0) {
          uVar18 = (uint)(recording != 0);
        }
        draw_results(swap_screen,(BITMAP *)data[iVar24].dat,(int)ROUND(local_938),qualify,
                     qualifyValue,uVar18);
        if ((((bVar36) && (local_940 != 0)) && (is_playing_custom_game == 0)) && (recording != 0)) {
          fVar5 = local_938 + local_938;
          textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,"Enter your initials",0x140,
                            (int)ROUND(fVar5 + 80.0),-1,-1);
          if (iVar17 == 0) {
            if ((local_94c & 4) == 0) {
              textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,(char *)((int)&c.start + 2),0x140
                                ,(int)ROUND(fVar5 + 120.0),-1,-1);
LAB_00415a92:
              in_stack_fffff660 = -1;
              in_stack_fffff65c = -1;
              in_stack_fffff654 = (double)CONCAT44((int)ROUND(fVar5 + 120.0),0x154);
              in_stack_fffff650 = &c.end;
            }
            else {
              iVar21 = (int)ROUND(fVar5 + 120.0);
              textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,(char *)&c,300,iVar21,-1,-1);
LAB_0041588b:
              textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,(char *)((int)&c.start + 2),0x140
                                ,iVar21,-1,-1);
LAB_004158a7:
              in_stack_fffff660 = -1;
              in_stack_fffff65c = -1;
              in_stack_fffff654 = (double)CONCAT44((int)ROUND(fVar5 + 120.0),0x154);
              in_stack_fffff650 = &c.end;
              textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,(char *)in_stack_fffff650,0x154,
                                (int)ROUND(fVar5 + 120.0),-1,-1);
              if ((iVar17 != 3) || ((local_94c & 4) == 0)) goto LAB_00414a2d;
              local_990 = 0x168;
              local_994 = &DAT_004d6054;
            }
          }
          else {
            iVar21 = (int)ROUND(fVar5 + 120.0);
            textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,(char *)&c,300,iVar21,-1,-1);
            if (iVar17 != 1) {
              textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,(char *)((int)&c.start + 2),0x140
                                ,iVar21,-1,-1);
              if (iVar17 != 2) goto LAB_004158a7;
              if ((local_94c & 4) != 0) goto LAB_00415a92;
              goto LAB_00414a2d;
            }
            if ((local_94c & 4) != 0) goto LAB_0041588b;
          }
          textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,(char *)in_stack_fffff650,
                            SUB84(in_stack_fffff654,0),(int)((ulonglong)in_stack_fffff654 >> 0x20),
                            in_stack_fffff65c,in_stack_fffff660);
        }
LAB_00414a2d:
        if (local_990 != iVar11) {
          pBVar4 = (BITMAP *)data[iVar11 + 0x4a].dat;
          if (pBVar4->vtable->color_depth == 8) {
            (*swap_screen->vtable->draw_256_sprite)(swap_screen,pBVar4,0x14,local_944);
          }
          else {
            (*swap_screen->vtable->draw_sprite)(swap_screen,pBVar4,0x14,local_944);
          }
          textout_ex(swap_screen,(FONT *)data[0x34].dat,"rank up!",0x14,local_944 + 0x46,-1,-1);
          local_944 = (int)ROUND((double)(0x140 - local_944) * 0.1 + (double)local_944);
        }
        if (summary_scroller_message[0] != '\0') {
          scroll_scroller(&summary_scroller,-2);
          drawing_mode(5,(BITMAP *)0x0,0,0);
          set_trans_blender(0,0,0,0x6e);
          iVar21 = makecol(0,0,0);
          (*swap_screen->vtable->rectfill)(swap_screen,0,iVar12,0x27f,iVar12 + 0x14,iVar21);
          iVar21 = makecol(0,0,0);
          (*swap_screen->vtable->rectfill)(swap_screen,0,iVar12,0x27f,iVar12 + 0x12,iVar21);
          iVar21 = makecol(0,0,0);
          (*swap_screen->vtable->rectfill)(swap_screen,0,iVar12,0x27f,iVar12 + 0x10,iVar21);
          solid_mode();
          iVar21 = makecol(0x96,0x96,0x96);
          draw_scroller(&summary_scroller,swap_screen,1,iVar12,iVar21);
          iVar21 = makecol(200,200,200);
          iVar21 = draw_scroller(&summary_scroller,swap_screen,0,iVar12,iVar21);
          if (iVar21 == 0) {
            restart_scroller(&summary_scroller);
          }
          iVar12 = (int)ROUND((double)-iVar12 * 0.1 + (double)iVar12);
        }
        local_92c = (local_92c + 1) - (uint)(local_92c == 0);
        pTVar19 = ply[player_id];
        if ((pTVar19->level * 5 < local_92c) || (0xfa < local_92c)) {
          play_sound(sounds[6],0,1);
          if (custom.falling != (SAMPLE *)0x0) {
            stop_sample(custom.falling);
          }
          pTVar19 = ply[player_id];
          pTVar19->shake = 0x18;
          local_92c = 0;
        }
        if (pTVar19->shake == 0) {
          blit_to_screen(swap_screen);
        }
        else {
          p_Var3 = screen->vtable->acquire;
          if (p_Var3 != (_func_void_BITMAP_ptr *)0x0) {
            (*p_Var3)(screen);
          }
          iVar21 = swap_screen->h;
          iVar22 = swap_screen->w;
          iVar20 = new_rand();
          blit(swap_screen,screen,0,iVar20 % 8,0,0,iVar22,iVar21);
          p_Var3 = screen->vtable->release;
          if (p_Var3 != (_func_void_BITMAP_ptr *)0x0) {
            (*p_Var3)(screen);
          }
          ply[player_id]->shake = ply[player_id]->shake + -1;
        }
        if ((((bVar36) && (local_940 != 0)) && (is_playing_custom_game == 0)) && (recording != 0)) {
          poll_control(&ctrl,0);
          iVar21 = keypressed();
          if ((iVar21 != 0) && (local_93c == 0x14)) {
            uVar18 = readkey();
            iVar21 = (uVar18 & 0xff) - 0x20;
            uVar23 = (undefined3)((uint)iVar21 >> 8);
            if (iVar21 == -0x18) {
              iVar21 = CONCAT31(uVar23,0xa4);
            }
            else if (iVar21 == 0xe) {
              iVar21 = CONCAT31(uVar23,0x2e);
            }
            else if (iVar21 == 1) {
              iVar21 = CONCAT31(uVar23,0x21);
            }
            else if (iVar21 == 0x20) goto LAB_00415033;
            for (uVar18 = 0; (int)uVar18 < (int)uVar31; uVar18 = uVar18 + 1) {
              cVar1 = *(char *)((int)last_keys + uVar18);
              if (cVar1 == iVar21) {
                *(char *)((int)&c.start + iVar17 * 2) = cVar1;
                iVar17 = iVar17 + 1;
                local_934 = uVar18;
                if (iVar17 == 3) {
                  uVar27 = 100;
                  local_93c = 0x13;
                }
                else {
                  uVar27 = 100;
                }
              }
            }
          }
LAB_00415033:
          if (uVar27 == 0) {
            iVar22 = is_right(&ctrl);
            iVar21 = 0;
            if (iVar22 != 0) {
              iVar21 = 8;
              local_934 = local_934 + 1 & -(uint)((int)(local_934 + 1) <= (int)uVar31);
            }
            iVar22 = is_left(&ctrl);
            if (iVar22 != 0) {
              iVar21 = 8;
              local_934 = local_934 - 1;
              if ((int)local_934 < 0) {
                local_934 = uVar31;
              }
            }
            iVar22 = is_fire(&ctrl);
            if (iVar22 != 0) {
              if (*(char *)((int)last_keys + local_934) == -0x5c) {
                if (iVar17 == 0) {
LAB_00415112:
                  iVar21 = 100;
                }
                else {
                  *(unsigned int *)((int)&c.start + iVar17 * 2) = 0x2e;
                  iVar17 = iVar17 + -1;
                  iVar21 = 100;
                }
              }
              else if (iVar17 < 2) {
                iVar17 = iVar17 + 1;
                iVar21 = 100;
              }
              else {
                if (local_93c != 0x14) goto LAB_00415112;
                iVar17 = iVar17 + 1;
                iVar21 = 100;
                local_93c = 0x13;
              }
            }
            if ((key[77] == '\0') && (key[63] == '\0')) {
              uVar27 = iVar21 + -1 + (uint)(iVar21 == 0);
            }
            else {
              *(unsigned int *)((int)&c.start + iVar17 * 2) = 0x2e;
              uVar27 = 7;
              if (iVar17 != 0) {
                iVar17 = iVar17 + -1;
              }
            }
          }
          else {
            uVar27 = uVar27 - 1;
          }
          iVar21 = is_any(&ctrl);
          if ((iVar21 == 0) && (key[77] == '\0')) {
            uVar27 = uVar27 & -(uint)(key[63] != '\0');
          }
          if (iVar17 < 3) {
            *(unsigned int *)((int)&c.start + iVar17 * 2) = *(unsigned int *)((int)last_keys + local_934);
          }
        }
        local_93c = local_93c - (uint)(local_93c != 0x14);
        poll_control(&ctrl,0);
        if ((((!bVar36) || (local_940 == 0)) || (is_playing_custom_game != 0)) &&
           (((iVar21 = keypressed(), iVar21 != 0 || (iVar21 = is_fire(&ctrl), iVar21 != 0)) &&
            (local_93c == 0x14)))) {
          local_93c = 0xe;
        }
        if ((key[64] == '\0') || (key[115] == '\0')) {
          while (cycle_count == 0) {
            rest(2);
          }
        }
      } while (local_93c != 0);
      if (is_playing_custom_game != 0) goto LAB_00413fa0;
      if (recording != 0) {
        keys_pressed[0] = (int)CONCAT12((unsigned int)c.end,CONCAT11(c.start._2_1_,(char)c.start));
        pcVar13 = (char *)keys_pressed;
        if (!bVar36) {
          pcVar13 = profile->handle;
        }
        _strcpy((char *)key_flag,pcVar13);
        iVar24 = 0;
        do {
          if (0 < qualify[iVar24]) {
            enter_hisc_table(hisc_tables[iVar24],qualifyValue[iVar24],(char *)key_flag);
            sort_hisc_table(hisc_tables[iVar24]);
          }
          iVar24 = iVar24 + 1;
        } while (iVar24 != 0xf);
        goto LAB_00413fa0;
      }
    }
    else {
LAB_00413fa0:
      if ((((recording != 0) && (debug == 0)) && (is_playing_custom_game == 0)) &&
         ((iVar24 = ply[player_id]->level / 100, (int)local_994 < iVar24 && (iVar24 < 10)))) {
        fadeOut(0x10);
        blit((BITMAP *)data[0x7e].dat,swap_screen,0,0,0,0,640,480);
        set_trans_blender(0,0,0,0x9e);
        drawing_mode(5,(BITMAP *)0x0,0,0);
        iVar24 = makecol(0,0,0);
        if (gfx_driver == (GFX_DRIVER *)0x0) {
          iVar12 = 0;
          iVar11 = 0;
        }
        else {
          iVar12 = gfx_driver->h;
          iVar11 = gfx_driver->w;
        }
        (*swap_screen->vtable->rectfill)(swap_screen,0,0,iVar11,iVar12,iVar24);
        solid_mode();
        pBVar4 = (BITMAP *)data[0x3a].dat;
        iVar24 = 0x140 - pBVar4->w / 2;
        if (pBVar4->vtable->color_depth == 8) {
          (*swap_screen->vtable->draw_256_sprite)(swap_screen,pBVar4,iVar24,0x14);
        }
        else {
          (*swap_screen->vtable->draw_sprite)(swap_screen,pBVar4,iVar24,0x14);
        }
        textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,"A new start floor",0x140,300,-1,-1);
        textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,"has been unlocked!",0x140,0x15e,-1,-1)
        ;
        textout_centre_ex(swap_screen,(FONT *)data[0x34].dat,"(Get it in the options menu)",0x140,
                          0x1b8,-1,-1);
        play_sound(sounds[2],0,0);
        fadeIn(swap_screen,0x10);
        do {
          do {
          } while (key[59] != '\0');
        } while ((key[75] != '\0') || (key[67] != '\0'));
        do {
        } while( true );
      }
    }
    save_config();
    stopGameMusic();
    if (-1 < checkMusicVoiceID) {
      voice_stop(checkMusicVoiceID);
    }
    rr = NULL;
    if (recording == 0) goto LAB_00413a4c;
    if (debug == 0) {
      in_replay_menu = 1;
      rr = (Treplay *)do_replay_menu();
      in_replay_menu = 0;
      goto LAB_00413a3f;
    }
  }
  else {
    if (!bVar8) {
      pTVar19 = ply[player_id];
      demo->score = pTVar19->level * 10 + pTVar19->score;
      pTVar15->floor = pTVar19->level;
      pTVar15->combo = pTVar19->best_combo;
      pTVar15->rejump = options.jump_hold;
      pTVar15->no_combo_top_floor = pTVar19->no_combo_top_floor;
      pTVar15->biggest_lost_combo = pTVar19->biggest_lost_combo;
      pTVar15->ccc[0] = pTVar19->ccc[0];
      pTVar15->ccc[1] = pTVar19->ccc[1];
      pTVar15->ccc[2] = pTVar19->ccc[2];
      pTVar15->ccc[3] = pTVar19->ccc[3];
      pTVar15->ccc[4] = pTVar19->ccc[4];
      pTVar15->jc[0] = pTVar19->jcTop[0];
      pTVar15->jc[1] = pTVar19->jcTop[1];
      pTVar15->jc[2] = pTVar19->jcTop[2];
      pTVar15->jc[3] = pTVar19->jcTop[3];
      pTVar15->jc[4] = pTVar19->jcTop[4];
      pTVar14 = profile;
      if (is_playing_custom_game == 0) {
        profile->games_played = profile->games_played + 1;
        pTVar14->total_floors = pTVar14->total_floors + pTVar15->floor;
        pTVar14->total_score = pTVar14->total_score + pTVar15->score;
        pTVar14->total_combos = pTVar14->total_combos + local_954;
        pTVar14->total_combo_floors = pTVar14->total_combo_floors + local_98c;
        iVar24 = 0;
        do {
          if (0 < pTVar15->ccc[iVar24]) {
            pTVar14->cccNum[iVar24] = pTVar14->cccNum[iVar24] + 1;
            pTVar14->cccTotal[iVar24] = pTVar14->cccTotal[iVar24] + pTVar15->ccc[iVar24];
          }
          iVar24 = iVar24 + 1;
        } while (iVar24 != 5);
      }
      else {
        profile->custom_games_played = profile->custom_games_played + 1;
      }
      iVar24 = file_exists(replay_directory,-1,(int *)0x0);
      if (iVar24 == 0) {
        _mkdir(replay_directory);
      }
      pTVar14 = profile;
      if (is_playing_custom_game == 0) {
        if (profile->best_floor < demo->floor) {
          profile->best_floor = demo->floor;
          myDeleteFile(replay_directory,pTVar14->best_replay_names[2]);
          sprintf(profile->best_replay_names[2],"%s_best_floor_%d.itr",profile->handle,demo->floor)
          ;
          save_replay(replay_directory,profile->best_replay_names[2],demo,rec_pos + 2,1);
          new_personal_best[2] = 1;
        }
        pTVar14 = profile;
        if (profile->best_combo < demo->combo) {
          profile->best_combo = demo->combo;
          myDeleteFile(replay_directory,pTVar14->best_replay_names[1]);
          sprintf(profile->best_replay_names[1],"%s_best_combo_%d.itr",profile->handle,demo->combo)
          ;
          save_replay(replay_directory,profile->best_replay_names[1],demo,rec_pos + 2,1);
          new_personal_best[1] = 1;
        }
        pTVar14 = profile;
        if (profile->best_score < demo->score) {
          profile->best_score = demo->score;
          myDeleteFile(replay_directory,pTVar14->best_replay_names[0]);
          sprintf(profile->best_replay_names[0],"%s_best_score_%d.itr",profile->handle,demo->score)
          ;
          save_replay(replay_directory,profile->best_replay_names[0],demo,rec_pos + 2,1);
          new_personal_best[0] = 1;
        }
        pTVar14 = profile;
        pTVar19 = ply[player_id];
        if (profile->no_combo_top_floor < pTVar19->no_combo_top_floor) {
          profile->no_combo_top_floor = pTVar19->no_combo_top_floor;
          myDeleteFile(replay_directory,pTVar14->best_replay_names[4]);
          sprintf(profile->best_replay_names[4],"%s_best_no_combo_%d.itr",profile->handle,
                   demo->no_combo_top_floor);
          save_replay(replay_directory,profile->best_replay_names[4],demo,rec_pos + 2,1);
          new_personal_best[4] = 1;
          pTVar19 = ply[player_id];
        }
        pTVar14 = profile;
        if (profile->biggest_lost_combo < pTVar19->biggest_lost_combo) {
          profile->biggest_lost_combo = pTVar19->biggest_lost_combo;
          myDeleteFile(replay_directory,pTVar14->best_replay_names[3]);
          sprintf(profile->best_replay_names[3],"%s_best_lost_combo_%d.itr",profile->handle,
                   demo->biggest_lost_combo);
          save_replay(replay_directory,profile->best_replay_names[3],demo,rec_pos + 2,1);
          new_personal_best[3] = 1;
          pTVar19 = ply[player_id];
        }
        iVar12 = 0xa0;
        iVar24 = 1;
        pTVar14 = profile;
        local_938 = (fixed)player_id;
        while( true ) {
          if (pTVar14->cccTotal[iVar24 + 4] < pTVar19->ccc[iVar24 + -1]) {
            pTVar14->cccTotal[iVar24 + 4] = pTVar19->ccc[iVar24 + -1];
            myDeleteFile(replay_directory,pTVar14->best_replay_names[0] + iVar12);
            sprintf(profile->best_replay_names[0] + iVar12,"%s_best_cc%d_%d.itr",profile->handle,
                     iVar24,ply[player_id]->ccc[iVar24 + -1]);
            save_replay(replay_directory,profile->best_replay_names[0] + iVar12,demo,rec_pos + 2,1);
            new_personal_best[iVar24 + 4] = 1;
            local_938 = (fixed)player_id;
            pTVar19 = ply[player_id];
            pTVar14 = profile;
          }
          iVar24 = iVar24 + 1;
          iVar12 = iVar12 + 0x20;
          if (iVar24 == 6) break;
          pTVar19 = ply[(int)local_938];
        }
        iVar12 = 0x140;
        iVar24 = 1;
        while( true ) {
          if (pTVar14->ccc[iVar24 + 4] < pTVar19->ccc[iVar24 + 4]) {
            pTVar14->ccc[iVar24 + 4] = pTVar19->ccc[iVar24 + 4];
            myDeleteFile(replay_directory,pTVar14->best_replay_names[0] + iVar12);
            sprintf(profile->best_replay_names[0] + iVar12,"%s_best_jj%d_%d.itr",profile->handle,
                     iVar24,ply[player_id]->ccc[iVar24 + 4]);
            save_replay(replay_directory,profile->best_replay_names[0] + iVar12,demo,rec_pos + 2,1);
            new_personal_best[iVar24 + 9] = 1;
          }
          iVar24 = iVar24 + 1;
          iVar12 = iVar12 + 0x20;
          if (iVar24 == 6) break;
          pTVar19 = ply[player_id];
          pTVar14 = profile;
        }
      }
      iVar24 = save_replay(replay_directory,"last_game.itr",demo,rec_pos + 2,1);
      if (iVar24 < 0) {
        my_alert("Failed to save replay.","(last_game.itr)",0,1);
        uberChecksum = 0;
      }
      else {
        sprintf((char *)qualifyValue,"%slast_game.itr",replay_directory);
        pTVar15 = load_replay((char *)qualifyValue);
        if (pTVar15 != NULL) {
          uberChecksum = calc_replay_checksum(demo);
          destroy_replay(pTVar15);
        }
      }
      pTVar14 = profile;
      profile->msc_volume = options.msc_volume;
      pTVar14->snd_volume = options.snd_volume;
      pTVar14->jump_hold = options.jump_hold;
      pTVar14->flash = options.flash;
      save_profile(pTVar14);
      goto LAB_00413e60;
    }
    profile->msc_volume = options.msc_volume;
    pTVar14->snd_volume = options.snd_volume;
    pTVar14->jump_hold = options.jump_hold;
    pTVar14->flash = options.flash;
    save_profile(pTVar14);
    rr = NULL;
LAB_00413a3f:
    if (recording == 0) goto LAB_00413a4c;
  }
  play_sound(speaker[2],0,0);
LAB_00413a4c:
  stopGameMusic();
  if (-1 < checkMusicVoiceID) {
    voice_stop(checkMusicVoiceID);
  }
  clear_bitmap(screen);
  return (int)rr;
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

void init_control(Tcontrol* c) {
  c->key_up = 0x54;
  c->key_down = 0x55;
  c->key_left = 0x52;
  c->key_right = 0x53;
  c->key_fire = 0x4b;
  c->key_enter = 0x43;
  c->key_pause = 0x10;
  c->flags = 0;
  c->use_joy = 0;
}

void reset_menu(Tmenu main_menu[7], Tmenu_params* menu_params, int sel_pos) {
  int theight;
  Tmenu *pMainMenu = main_menu;

  do {
    pMainMenu->flags &= 0xfffffffe;
    pMainMenu++;
  } while ((char)pMainMenu->flags > -1);
  main_menu[sel_pos].flags = main_menu[sel_pos].flags | 1;
  theight = text_height(menu_params->font);
  menu_params->font_height = theight;
}

void run_demo(char* filename) {
  bool gameStartedSuccesfully;
  int startFloor;
  
  if (filename != NULL) {
    if (demo != NULL) {
      destroy_replay(demo);
    }
    demo = load_replay(filename);
  }
  if (demo != NULL) {
    if (itrcheck == 0) {
      startFloor = profile->start_floor;
      profile->start_floor = 0;
      gameStartedSuccesfully = new_game();
    }
    else {
      startFloor = 0;
      gameStartedSuccesfully = new_game();
    }

    if (gameStartedSuccesfully) {
      play();
      end_game();
    }
    if (itrcheck == 0) {
      profile->start_floor = startFloor;
      floors.value = startFloor;
    }
  }
}

void main_menu_callback() {

}

Treplay* replay_selector(Tcontrol* ctrl, char* path) {

}