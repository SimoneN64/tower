#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#define TOWER_VERSION "1.5.1"

int itrcheck;

int main(int argc,char **argv)
{
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
  _install_allegro_version_check(0,errno_ptr,FUN_00401000,0x40401);
  register_png_file_type();
  get_executable_name(full_path,0x400);
  replace_filename(working_directory,full_path,"",0x400);
  chdir(working_directory);
  pcVar5 = logfilename;
  for (iVar4 = 256; iVar4 > 0; iVar4 = iVar4 - 1) {
    *(pcVar5++) = '\0';
  }
  get_logfile_path(logfilename,0x100);
  _File = fopen(logfilename,"wt");
  if (_File != (FILE *)0x0) {
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
    if (dropped_file_is_not_a_replay == 0) {
      log2file("* Failed to initialize the game *");
      allegro_message("Failed to initialize the game.");
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
    if (got_joystick != 0) {
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
        while (demo = replay_selector(&ctrl,replay_directory), demo != NULL) {
          fadeOut(0x10);
          stopMenuMusic();
          run_demo((char *)0x0);
          fadeOut(0x10);
          main_menu_callback();
          draw_menu(swap_screen,main_menu,&menu_params,0x163,0x11d,0);
          fadeIn(swap_screen,0x20);
          if ((bg_menu != NULL) && (options.msc_volume != 0)) {
            play_sample(bg_menu,options.msc_volume,0x80,1000,1);
          }
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