#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
//#include <direct.h>
//#include <windows.h>
//#include <wingdi.h>
#include "game.h"

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
Thisc_table hisc_tables[15];
char* hisc_names[15];
SYSTEM_DRIVER* system_driver;
int (*usetc)(char*, int);
char the_password[256]={0};
Tmenu_char_selection play_char;
Tmenu_selection eyecandy_selection,scroll_speed_selection,floor_size_selection,gravity_selection;
Tcommandline cmdline;

#define log2file(str, ...)

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

int get_logfile_path(char* filename, int len) {
  strncpy(filename,"log.txt",len);
  return 1;
}

int utf8_getx(char **s) {
  char *pbVar1;
  int iVar2;
  uint uVar3;
  int c;
  char bVar4;
  int t;
  int n;
  int iVar5;
  char *pbVar6;

  pbVar1 = (char *)*s;
  bVar4 = *pbVar1;
  uVar3 = (uint)bVar4;
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

Thisc_table * make_hisc_table(char *name) {
  Thisc_table *_Dest;
  Thisc *pTVar1;
  Thisc_table *table;

  _Dest = (Thisc_table *)malloc(0x24);
  if (_Dest != (Thisc_table *)0x0) {
    pTVar1 = (Thisc *)malloc(0xb4);
    _Dest->posts = pTVar1;
    if (pTVar1 != NULL) {
      strcpy((char *)_Dest,name);
      return _Dest;
    }
    _Dest = NULL;
  }
  return _Dest;
}

void reset_hisc_table(Thisc_table *table,char *name,int hi,int lo) {
  for(int i = 0; i < 180; i += 36) {
    strcpy(table->posts->name + i,name);
    *(int *)(table->posts->name + i + 32) = 0;
  }
}

int init_game(int argc, char** argv) {
  BITMAP *pBVar1;
  char *puVar2;
  int iVar3;
  int iVar4;
  Thisc_table *pTVar5;
  PACKFILE *fp_00;
  int (*depth)();
  DATAFILE *dat;
  int whiteColor;
  Tgamepad *pTVar6;
  char *ext;
  Tplayer *pTVar7;
  char *pcVar8;
  //LPWSADATA pWVar9;
  int i;
  DATAFILE *loader;
  int i_1;
  Tgamepad *gp_1;
  PACKFILE *fp;
  BITMAP *fldLogo;
  //LPWSADATA lpWSAData;
  char *checkFile;
  Tgamepad *gp;
  int last_cc;
  char *pcVar10;
  bool bVar11;
  time_t tVar12;
  int iVar13;
  char buf [8];
  //WSADATA wsaData;
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
  /*lpWSAData = (LPWSADATA)&wsaData;
  iVar3 = _WSAStartup@8(0x202,lpWSAData);
  if (iVar3 != 0) {
    log2file(" !!! Failed to setup Winsock");
  }
  pWVar9 = (LPWSADATA)(wsaData._0_4_ & 0xff);
  if (((byte)wsaData._0_4_ < 2) || (SUB41(wsaData._0_4_,1) < 2)) {
    log2file(" !!! Failed to get proper Winsock version (wanted 2.2, got %d.%d)",pWVar9,
             (uint)SUB41(wsaData._0_4_,1));
    lpWSAData = pWVar9;
  }*/
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
      /*lpWSAData = (LPWSADATA)argv[1];
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
        profile = (Tprofile *)0x0;
      }*/
    }
  }
  else {
    //lpWSAData = NULL;
    iVar13 = 0;
    iVar3 = 1;
    do {
      /*pWVar9 = (LPWSADATA)argv[iVar3];
      if (*(char *)&pWVar9->wVersion != '-') {
        lpWSAData = pWVar9;
      }
      iVar4 = stricmp((char *)pWVar9,"-check");
      if (iVar4 == 0) {
        iVar13 = 1;
      }*/
      iVar4 = strcmp(argv[iVar3],"-jumps");
      if (iVar4 == 0) {
        cmdline.jumps = 1;
      }
      iVar4 = strcmp(argv[iVar3],"-combos");
      if (iVar4 == 0) {
        cmdline.combos = 1;
      }
      /*iVar4 = strcmp(argv[iVar3],(char *)NULL);
      if (iVar4 == 0) {
        cmdline.sd = 1;
      }*/
      iVar4 = strcmp(argv[iVar3],"-keys");
      if (iVar4 == 0) {
        cmdline.keys = 1;
      }
      iVar4 = strcmp(argv[iVar3],"-all");
      if (iVar4 == 0) {
        cmdline.keys = 1;
        cmdline.jumps = 1;
        cmdline.combos = 1;
        cmdline.sd = 1;
      }
      iVar4 = strcmp(argv[iVar3],"-tiny");
      if (iVar4 == 0) {
        cmdline.tiny = 1;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < argc);
    if (iVar13 == 0) {
      printf("<%s>\nis not a vaild option.",argv[1]);
      log2file("*** Erroneous option (%s)",argv[1]);
      dropped_file_is_not_a_replay = 1;
      return 0;
    }
    log2file("Loading %s",argv[2]);
    /*demo = load_replay((char *)lpWSAData);
    if (demo == NULL) {
      pcVar8 = get_filename((char *)lpWSAData);
      printf("<itrcheck_results status=\"error\">%s</itrcheck_results>\n",pcVar8);
      log2file("*** Failed!");
      dropped_file_is_not_a_replay = 1;
      return 0;
    }*/
    itrcheck = 1;
    log2file("ITRCHECK activated, checking <%s>",lpWSAData);
  }
  log2file("Creating hiscore tables",lpWSAData);
  iVar3 = 0;
  do {
    pTVar5 = make_hisc_table(hisc_names[iVar3]);
    hisc_tables[iVar3] = pTVar5;
    if (pTVar5 == NULL) {
      log2file(" *** failed")
      printf("Failed reserve memory for highscore table.");
      return 0;
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 != 0xf);
  iVar3 = 0;
  do {
    reset_hisc_table(hisc_tables[iVar3],"Harold",1000,0);
    iVar3 = iVar3 + 1;
  } while (iVar3 != 0xf);
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
    iVar3 = 0;
    for(iVar3 = 0; iVar3 < 0xf; iVar3++) {
      while (load_hisc_table(hisc_tables[iVar3],fp_00) != 0) {
        iVar3++;
        if (iVar3 == 0xf) goto LAB_0040ecb2;
      }
      reset_hisc_table(hisc_tables[iVar3],"FLD",1000,0);
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
  iVar3 = options.timesStarted;
  log2file("Game started %d times",options.timesStarted);
  depth = system_driver->desktop_color_depth;
  if (depth != NULL) {
    depth = (int *)(*depth)();
  }
  set_color_depth((int)depth);
  if (options.full_screen == 0) {
    log2file("Setting windowed mode 640x480",iVar3);
    iVar3 = 0x280;
    iVar13
    if (iVar13 < 0) {
      log2file(" *** failed");
      options.full_screen = -1;
      goto LAB_0040ed10;
    }
    window = 1;
    if (options.full_screen != 0) goto LAB_0040ed10;
  }
  else {
    LAB_0040ed10:
    log2file("Setting fullscreen mode 640x480",iVar3);
    iVar3
    if (iVar3 < 0) {
      log2file(" *** failed")
      printf("Failed to set graphics mode.");
      return 0;
    }
    window = 0;
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
  iVar3 = makecol(180,180,180);
  textprintf_centre_ex(screen,font,320,220,iVar3,-1,"please wait");
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
  iVar3 = makecol(255,255,255);
  (*screen->vtable->clear_to_color)(screen,iVar3);
  iVar3 = 200 - pBVar1->h / 2;
  iVar13 = 320 - pBVar1->w / 2;
  if (pBVar1->vtable->color_depth == 8) {
    (*screen->vtable->draw_256_sprite)(screen,pBVar1,iVar13,iVar3);
  }
  else {
    (*screen->vtable->draw_sprite)(screen,pBVar1,iVar13,iVar3);
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
  iVar3 = install_joystick(-1);
  got_joystick = (int)(iVar3 == 0);
  if (iVar3 == 0) {
    ctrl.use_joy = 1;
    iVar3 = joy[0].num_buttons;
    log2file(" gamepad has %d buttons",joy[0].num_buttons);
    iVar13 = exists("gamepad.txt");
    if (iVar13 == 0) {
      pTVar6 = get_gamepad();
      log2file(" gamepad.txt is missing, setting defaults",iVar3);
      pTVar6->up = 4;
      pTVar6->left = 1;
      pTVar6->right = 2;
      pTVar6->down = 8;
      iVar3 = 0;
      do {
        pTVar6->b[iVar3] = 0x10;
        iVar3 = iVar3 + 1;
      } while (iVar3 != 0x20);
    }
    else {
      pTVar6 = get_gamepad();
      log2file(" getting values from gamepad.txt",iVar3);
      set_config_file("gamepad.txt");
      iVar3 = get_gamepad_value("up");
      pTVar6->up = iVar3;
      iVar3 = get_gamepad_value("left");
      pTVar6->left = iVar3;
      iVar3 = get_gamepad_value("right");
      pTVar6->right = iVar3;
      iVar3 = get_gamepad_value("down");
      pTVar6->down = iVar3;
      iVar3 = 0;
      do {
        iVar4 = iVar3 + 1;
        _sprintf(buf,(char *)&PTR_DAT_004d56ed,iVar4);
        iVar13 = get_gamepad_value(buf);
        pTVar6->b[iVar3] = iVar13;
        iVar3 = iVar4;
      } while (iVar4 != 0x20);
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
  packfile_password((char *)0x0);
  LAB_0040f1dc:
  log2file("Initiating player");
  draw_progress_bar();
  iVar3 = rand();
  player_id = iVar3 % 1000;
  pTVar7 = (Tplayer *)malloc(184);
  ply[iVar3 % 1000] = pTVar7;
  if (ply[player_id] == NULL) {
    log2file(" *** failed")
    printf("Failed to allocate memory for player.");
    iVar3 = 0;
  }
  else {
    if (itrcheck == 0) {
      puVar2 = (undefined *)data->dat;
      puVar2[2] = 0;
      puVar2[1] = 0;
      *puVar2 = 0;
      gameover_bmp = (BITMAP *)data[0x37].dat;
      log2file("Checking profile directory");
      get_profiles_dir(buf,0x400);
      iVar3 = file_exists(buf,-1,NULL);
      if (iVar3 == 0) {
        log2file("  does not exist, trying to create");
        _mkdir(buf);
        iVar3 = file_exists(buf,-1,NULL);
        if (iVar3 == 0) {
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
      iVar3 = check_characters();
      if (iVar3 == 0) {
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
        for (iVar3 = 9; iVar3 > 0; iVar3 -= 1) {
          *pcVar10 = *pcVar8;
          pcVar8++;
          pcVar10++;
        }
        log2file(" no sounds loaded");
      }
      else {
        pcVar8 = "sfx15.dat";
        pcVar10 = sfx_file;
        for (iVar3 = 10; iVar3 != 0; iVar3 = iVar3 + -1) {
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
        buf[0] = '\x01';
        buf[1] = '\0';
        buf[2] = '\0';
        buf[3] = '\0';
        combo_sound[5] = getSampleFromOggDatafile(sfx,1);
        buf._4_4_ = sfx;
        buf[0] = '~';
        buf[1] = -6;
        buf[2] = '@';
        buf[3] = '\0';
        combo_sound[6] = getSampleFromOggDatafile(sfx,5);
        combo_sound[7] = getSampleFromOggDatafile(sfx,6);
        combo_sound[8] = getSampleFromOggDatafile(sfx,0xf);
        combo_sound[9] = getSampleFromOggDatafile(sfx,0x14);
        bg_beat = getSampleFromOggDatafile(sfx,2);
        bg_menu = getSampleFromOggDatafile(sfx,3);
        speaker[0] = getSampleFromOggDatafile(sfx,10);
        speaker[1] = getSampleFromOggDatafile(sfx,7);
        speaker[2] = getSampleFromOggDatafile(sfx,0x13);
        sounds[2] = getSampleFromOggDatafile(sfx,0);
        sounds[4] = getSampleFromOggDatafile(sfx,0xd);
        sounds[6] = getSampleFromOggDatafile(sfx,0xe);
        sounds[7] = getSampleFromOggDatafile(sfx,4);
        sounds[8] = getSampleFromOggDatafile(sfx,0x10);
        menu_sounds[0] = getSampleFromOggDatafile(sfx,0xb);
        menu_sounds[1] = getSampleFromOggDatafile(sfx,0xc);
        jump_sound[0] = NULL;
        jump_sound[1] = NULL;
        jump_sound[2] = NULL;
        sounds[0] = NULL;
        sounds[1] = NULL;
        sounds[3] = NULL;
        sounds[5] = NULL;
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
      iVar3 = 0;
      while ((iVar13 = keypressed(), iVar13 == 0 && (cycle_count < 0x96))) {
        if ((cycle_count % 10 == 0) && (bVar11 = cycle_count != iVar3, iVar3 = cycle_count, bVar11)) {
          draw_progress_bar();
          iVar3 = cycle_count;
        }
        rest(2);
      }
      iVar3 = rand();
      seed = (double)(iVar3 % 2367);
      fadeOut(16);
      clear_bitmap(screen);
      vsync();
      clear_keybuf();
    }
    init_ok = 1;
    iVar3 = -1;
  }
  return iVar3;
}

void uninit_game() {
  Tprofile* p;
  int iVar1;
  int i;
  int iVar2;

  log2file("\nUNINIT");
  if (init_ok) {
    log2file("Saving config");
    save_config();
    log2file("Saving profile \'%s\'",profile->handle);
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
    if (sounds[iVar1] != (SAMPLE *)0x0) {
      destroy_sample(sounds[iVar1]);
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 != 9);
  if (bg_beat != (SAMPLE *)0x0) {
    destroy_sample(bg_beat);
  }
  if (bg_menu != (SAMPLE *)0x0) {
    destroy_sample(bg_menu);
  }
  log2file("Freeing custom character memory");
  _Memory = characters;
  if (0 < num_chars) {
    iVar2 = 0;
    iVar1 = 0;
    do {
      if (*(BITMAP **)(_Memory->name + iVar2 + -8) != (BITMAP *)0x0) {
        destroy_bitmap(*(BITMAP **)(_Memory->name + iVar2 + -8));
        _Memory = characters;
      }
      iVar1 = iVar1 + 1;
      iVar2 = iVar2 + 0x88c;
    } while (iVar1 < num_chars);
  }
  free(_Memory);
  log2file("Unloading datafile");
  if (data != (DATAFILE *)0x0) {
    unload_datafile(data);
  }
  log2file("Free buffer memory");
  if (swap_screen != (BITMAP *)0x0) {
    destroy_bitmap(swap_screen);
  }
  log2file("Free highscore tables");
  iVar1 = 0;
  do {
    if (hisc_tables[iVar1] != (Thisc_table *)0x0) {
      destroy_hisc_table(hisc_tables[iVar1]);
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 != 0xf);
  log2file("Free player");
  if (ply[player_id] != (Tplayer *)0x0) {
    free(ply[player_id]);

  log2file("Exiting Allegro");
  allegro_exit();
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
  
  if (init_game(argc,argv) == 0) {
    if (!dropped_file_is_not_a_replay) {
      log2file("* Failed to initialize the game *");
      //printf("Failed to initialize the game.");
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