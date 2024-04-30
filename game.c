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
  install_timers();
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

void main_menu_callback() {

}

Treplay* replay_selector(Tcontrol* ctrl, char* path) {

}