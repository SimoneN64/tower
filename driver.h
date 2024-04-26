//
// Created by simone on 4/26/24.
//

#pragma once

typedef struct __DRIVER_INFO {
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