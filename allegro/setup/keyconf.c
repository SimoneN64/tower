/*         ______   ___    ___
 *        /\  _  \ /\_ \  /\_ \ 
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___ 
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *
 *      The keyboard configuration utility program.
 *
 *      By Shawn Hargreaves, modified by Peter Pavlovic.
 *
 *      See readme.txt for copyright information.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allegro.h"
#include "allegro/internal/aintern.h"



char *scancode_name[] = 
{
   "(none)",         "KEY_A",          "KEY_B",          "KEY_C",
   "KEY_D",          "KEY_E",          "KEY_F",          "KEY_G",
   "KEY_H",          "KEY_I",          "KEY_J",          "KEY_K",
   "KEY_L",          "KEY_M",          "KEY_N",          "KEY_O",
   "KEY_P",          "KEY_Q",          "KEY_R",          "KEY_S",
   "KEY_T",          "KEY_U",          "KEY_V",          "KEY_W",
   "KEY_X",          "KEY_Y",          "KEY_Z",          "KEY_0",
   "KEY_1",          "KEY_2",          "KEY_3",          "KEY_4",
   "KEY_5",          "KEY_6",          "KEY_7",          "KEY_8",
   "KEY_9",          "KEY_0_PAD",      "KEY_1_PAD",      "KEY_2_PAD",
   "KEY_3_PAD",      "KEY_4_PAD",      "KEY_5_PAD",      "KEY_6_PAD",
   "KEY_7_PAD",      "KEY_8_PAD",      "KEY_9_PAD",      "KEY_F1",
   "KEY_F2",         "KEY_F3",         "KEY_F4",         "KEY_F5",
   "KEY_F6",         "KEY_F7",         "KEY_F8",         "KEY_F9",
   "KEY_F10",        "KEY_F11",        "KEY_F12",        "KEY_ESC",
   "KEY_TILDE",      "KEY_MINUS",      "KEY_EQUALS",     "KEY_BACKSPACE",
   "KEY_TAB",        "KEY_OPENBRACE",  "KEY_CLOSEBRACE", "KEY_ENTER",
   "KEY_COLON",      "KEY_QUOTE",      "KEY_BACKSLASH",  "KEY_BACKSLASH2",
   "KEY_COMMA",      "KEY_STOP",       "KEY_SLASH",      "KEY_SPACE",
   "KEY_INSERT",     "KEY_DEL",        "KEY_HOME",       "KEY_END",
   "KEY_PGUP",       "KEY_PGDN",       "KEY_LEFT",       "KEY_RIGHT",
   "KEY_UP",         "KEY_DOWN",       "KEY_SLASH_PAD",  "KEY_ASTERISK",
   "KEY_MINUS_PAD",  "KEY_PLUS_PAD",   "KEY_DEL_PAD",    "KEY_ENTER_PAD",
   "KEY_PRTSCR",     "KEY_PAUSE",      "KEY_ABNT_C1",    "KEY_YEN",
   "KEY_KANA",       "KEY_CONVERT",    "KEY_NOCONVERT",  "KEY_AT",
   "KEY_CIRCUMFLEX", "KEY_COLON2",     "KEY_KANJI",
   "KEY_LSHIFT",     "KEY_RSHIFT",     "KEY_LCONTROL",   "KEY_RCONTROL",
   "KEY_ALT",        "KEY_ALTGR",      "KEY_LWIN",       "KEY_RWIN",
   "KEY_MENU",       "KEY_SCRLOCK",    "KEY_NUMLOCK",    "KEY_CAPSLOCK",
   "KEY_MAX"
};


char *ascii_name[32] = 
{
   "", "^a", "^b", "^c", "^d", "^e", "^f", "^g",
   "^h", "^i", "^j", "^k", "^l", "^m", "^n", "^o",
   "^p", "^q", "^r", "^s", "^t", "^u", "^v", "^w",
   "^x", "^y", "^z", "27", "28", "29", "30", "31"
};


unsigned short orig_key_ascii_table[KEY_MAX];
unsigned short orig_key_capslock_table[KEY_MAX];
unsigned short orig_key_shift_table[KEY_MAX];
unsigned short orig_key_control_table[KEY_MAX];
unsigned short orig_key_altgr_lower_table[KEY_MAX];
unsigned short orig_key_altgr_upper_table[KEY_MAX];
unsigned short orig_key_accent1_lower_table[KEY_MAX];
unsigned short orig_key_accent1_upper_table[KEY_MAX];
unsigned short orig_key_accent2_lower_table[KEY_MAX];
unsigned short orig_key_accent2_upper_table[KEY_MAX];
unsigned short orig_key_accent3_lower_table[KEY_MAX];
unsigned short orig_key_accent3_upper_table[KEY_MAX];
unsigned short orig_key_accent4_lower_table[KEY_MAX];
unsigned short orig_key_accent4_upper_table[KEY_MAX];


unsigned short my_key_ascii_table[KEY_MAX];
unsigned short my_key_capslock_table[KEY_MAX];
unsigned short my_key_shift_table[KEY_MAX];
unsigned short my_key_control_table[KEY_MAX];
unsigned short my_key_altgr_lower_table[KEY_MAX];
unsigned short my_key_altgr_upper_table[KEY_MAX];
unsigned short my_key_accent1_lower_table[KEY_MAX];
unsigned short my_key_accent1_upper_table[KEY_MAX];
unsigned short my_key_accent2_lower_table[KEY_MAX];
unsigned short my_key_accent2_upper_table[KEY_MAX];
unsigned short my_key_accent3_lower_table[KEY_MAX];
unsigned short my_key_accent3_upper_table[KEY_MAX];
unsigned short my_key_accent4_lower_table[KEY_MAX];
unsigned short my_key_accent4_upper_table[KEY_MAX];


#if (!defined ALLEGRO_DOS) && (!defined ALLEGRO_WINDOWS) && (!defined ALLEGRO_WITH_XWINDOWS) && (!defined ALLEGRO_QNX)

   /* if this platform isn't using pckeys.c, we declare these ourselves */
   int _key_accent1 = 0;
   int _key_accent2 = 0;
   int _key_accent3 = 0;
   int _key_accent4 = 0;
   int _key_accent1_flag = 0;
   int _key_accent2_flag = 0;
   int _key_accent3_flag = 0;
   int _key_accent4_flag = 0;

   int _key_standard_kb;

   unsigned short *_key_ascii_table;
   unsigned short *_key_capslock_table;
   unsigned short *_key_shift_table;
   unsigned short *_key_control_table;
   unsigned short *_key_altgr_lower_table;
   unsigned short *_key_altgr_upper_table;
   unsigned short *_key_accent1_lower_table;
   unsigned short *_key_accent1_upper_table;
   unsigned short *_key_accent2_lower_table;
   unsigned short *_key_accent2_upper_table;
   unsigned short *_key_accent3_lower_table;
   unsigned short *_key_accent3_upper_table;
   unsigned short *_key_accent4_lower_table;
   unsigned short *_key_accent4_upper_table;

#endif


char keyboard_name[64] = EMPTY_STRING;

#define FILENAME_LENGTH (512)
char config_file[FILENAME_LENGTH] = EMPTY_STRING;

unsigned short *editor_table;

int split_altgr = FALSE;

int codepage;

int loader(void);
int saver(void);
int quitter(void);
int editor(void);
int accenter1(void);
int accenter2(void);
int accenter3(void);
int accenter4(void);
int tester(void);


MENU file_menu[] =
{
   { "&Load\t(ctrl+L)",       loader,     NULL,       0,    NULL  },
   { "&Save\t(ctrl+S)",       saver,      NULL,       0,    NULL  },
   { "",                      NULL,       NULL,       0,    NULL  },
   { "&Quit\t(ctrl+Q)",       quitter,    NULL,       0,    NULL  },
   { NULL,                    NULL,       NULL,       0,    NULL  }
};


MENU edit_menu[] =
{
   { "Normal",                editor,     NULL,       0,    NULL  },
   { "Capslock",              editor,     NULL,       0,    NULL  },
   { "Shifted",               editor,     NULL,       0,    NULL  },
   { "Control",               editor,     NULL,       0,    NULL  },
   { "Alt+Gr",                editor,     NULL,       0,    NULL  },
   { "Alt+Gr (caps)",         editor,     NULL,       0,    NULL  },
   { "Accent1",               editor,     NULL,       0,    NULL  },
   { "Accent1 (caps)",        editor,     NULL,       0,    NULL  },
   { "Accent2",               editor,     NULL,       0,    NULL  },
   { "Accent2 (caps)",        editor,     NULL,       0,    NULL  },
   { "Accent3",               editor,     NULL,       0,    NULL  },
   { "Accent3 (caps)",        editor,     NULL,       0,    NULL  },
   { "Accent4",               editor,     NULL,       0,    NULL  },
   { "Accent4 (caps)",        editor,     NULL,       0,    NULL  },
   { NULL,                    NULL,       NULL,       0,    NULL  }
};


MENU misc_menu[] =
{
   { "Accent #&1",            accenter1,  NULL,       0,    NULL  },
   { "Accent #&2",            accenter2,  NULL,       0,    NULL  },
   { "Accent #&3",            accenter3,  NULL,       0,    NULL  },
   { "Accent #&4",            accenter4,  NULL,       0,    NULL  },
   { "",                      NULL,       NULL,       0,    NULL  },
   { "&Test\t(ctrl+T)",       tester,     NULL,       0,    NULL  },
   { NULL,                    NULL,       NULL,       0,    NULL  }
};


MENU menu[] = 
{ 
   { "&File",                 NULL,       file_menu,  0,    NULL  },
   { "&Edit",                 NULL,       edit_menu,  0,    NULL  },
   { "&Misc",                 NULL,       misc_menu,  0,    NULL  },
   { NULL,                    NULL,       NULL,       0,    NULL  }
};



#define C(x)      (x - 'a' + 1)


DIALOG main_dlg[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key)    (flags)     (d1)           (d2)     (dp)     (dp2) (dp3) */
   { d_clear_proc,      0,    0,    0,    0,    0,    8,    0,       0,          0,             0,       NULL,    NULL, NULL  },
   { d_menu_proc,       0,    0,    0,    0,    0,    0,    0,       0,          0,             0,       menu,    NULL, NULL  },
   { d_keyboard_proc,   0,    0,    0,    0,    0,    0,    C('l'),  0,          0,             0,       loader,  NULL, NULL  },
   { d_keyboard_proc,   0,    0,    0,    0,    0,    0,    C('s'),  0,          0,             0,       saver,   NULL, NULL  },
   { d_keyboard_proc,   0,    0,    0,    0,    0,    0,    C('t'),  0,          0,             0,       tester,  NULL, NULL  },
   { d_keyboard_proc,   0,    0,    0,    0,    0,    0,    C('q'),  0,          0,             0,       quitter, NULL, NULL  },
   { d_keyboard_proc,   0,    0,    0,    0,    0,    0,    27,      0,          0,             0,       quitter, NULL, NULL  },
   { d_ctext_proc,      320,  140,  0,    0,    0,    8,    0,       0,          0,             0,       "Allegro Keyboard Setup Utility", NULL, NULL  },
   { d_ctext_proc,      320,  180,  0,    0,    0,    8,    0,       0,          0,             0,       "Version " ALLEGRO_VERSION_STR ", " ALLEGRO_PLATFORM_STR, NULL, NULL  },
   { d_ctext_proc,      320,  250,  0,    0,    0,    8,    0,       0,          0,             0,       "By Shawn Hargreaves, " ALLEGRO_DATE_STR, NULL, NULL  },
   { d_ctext_proc,      320,  270,  0,    0,    0,    8,    0,       0,          0,             0,       "Modified by Peter Pavlovi\xC4\x8D", NULL, NULL },
   { d_text_proc,       136,  380,  0,    0,    0,    8,    0,       0,          0,             0,       "Keyboard mapping name:", NULL, NULL  },
   { d_edit_proc,       320,  380,  192,  8,    0,    16,   0,       0,          23,            0,       keyboard_name, NULL, NULL  },
   { NULL,              0,    0,    0,    0,    0,    0,    0,       0,          0,             0,       NULL,    NULL, NULL }
};



/* reads a specific keymapping table from the config file */
void load_table(unsigned short *table, char *section)
{
   char name[80];
   int i;

   for (i=0; i<KEY_MAX; i++) {
      sprintf(name, "key%d", i);
      table[i] = get_config_int(section, name, table[i]);
   }
}



/* handle the load command */
int loader()
{
   char buf[FILENAME_LENGTH];

   strcpy(buf, config_file);

   if (file_select_ex("Load Keyboard Config", buf, "CFG", sizeof(buf), 0, 0)) {
      strlwr(buf);
      strcpy(config_file, buf);

      memcpy(_key_ascii_table, orig_key_ascii_table, sizeof(orig_key_ascii_table));
      memcpy(_key_capslock_table, orig_key_capslock_table, sizeof(orig_key_capslock_table));
      memcpy(_key_shift_table, orig_key_shift_table, sizeof(orig_key_shift_table));
      memcpy(_key_control_table, orig_key_control_table, sizeof(orig_key_control_table));
      memcpy(_key_altgr_lower_table, orig_key_altgr_lower_table, sizeof(orig_key_altgr_lower_table));
      memcpy(_key_altgr_upper_table, orig_key_altgr_upper_table, sizeof(orig_key_altgr_upper_table));
      memcpy(_key_accent1_lower_table, orig_key_accent1_lower_table, sizeof(orig_key_accent1_lower_table));
      memcpy(_key_accent1_upper_table, orig_key_accent1_upper_table, sizeof(orig_key_accent1_upper_table));
      memcpy(_key_accent2_lower_table, orig_key_accent2_lower_table, sizeof(orig_key_accent2_lower_table));
      memcpy(_key_accent2_upper_table, orig_key_accent2_upper_table, sizeof(orig_key_accent2_upper_table));
      memcpy(_key_accent3_lower_table, orig_key_accent3_lower_table, sizeof(orig_key_accent3_lower_table));
      memcpy(_key_accent3_upper_table, orig_key_accent3_upper_table, sizeof(orig_key_accent3_upper_table));
      memcpy(_key_accent4_lower_table, orig_key_accent4_lower_table, sizeof(orig_key_accent4_lower_table));
      memcpy(_key_accent4_upper_table, orig_key_accent4_upper_table, sizeof(orig_key_accent4_upper_table));

      push_config_state();
      set_config_file(buf);

      load_table(_key_ascii_table,               "key_ascii");
      load_table(_key_capslock_table,            "key_capslock");
      load_table(_key_shift_table,               "key_shift");
      load_table(_key_control_table,             "key_control");

      load_table(_key_altgr_lower_table,         "key_altgr");
      load_table(_key_altgr_upper_table,         "key_altgr");

      load_table(_key_altgr_lower_table,         "key_altgr_lower");
      load_table(_key_altgr_upper_table,         "key_altgr_upper");

      load_table(_key_accent1_lower_table,       "key_accent1_lower");
      load_table(_key_accent1_upper_table,       "key_accent1_upper");
      load_table(_key_accent2_lower_table,       "key_accent2_lower");
      load_table(_key_accent2_upper_table,       "key_accent2_upper");
      load_table(_key_accent3_lower_table,       "key_accent3_lower");
      load_table(_key_accent3_upper_table,       "key_accent3_upper");
      load_table(_key_accent4_lower_table,       "key_accent4_lower");
      load_table(_key_accent4_upper_table,       "key_accent4_upper");

      _key_accent1 = get_config_int("key_escape", "accent1", 0);
      _key_accent2 = get_config_int("key_escape", "accent2", 0);
      _key_accent3 = get_config_int("key_escape", "accent3", 0);
      _key_accent4 = get_config_int("key_escape", "accent4", 0);
      _key_accent1_flag = get_config_int("key_escape", "accent1_flag", 0);
      _key_accent2_flag = get_config_int("key_escape", "accent2_flag", 0);
      _key_accent3_flag = get_config_int("key_escape", "accent3_flag", 0);
      _key_accent4_flag = get_config_int("key_escape", "accent4_flag", 0);

      strcpy(keyboard_name, get_config_string(NULL, "keyboard_name", ""));
      main_dlg[11].d2 = strlen(keyboard_name);

      pop_config_state();
   }

   return D_REDRAW;
}



/* writes a specific keymapping table to the config file */
void save_table(unsigned short *table, unsigned short *origtable, char *section)
{
   char name[80];
   int i;

   for (i=0; i<KEY_MAX; i++) {
      sprintf(name, "key%d", i);
      if (table[i] != origtable[i])
	 set_config_int(section, name, table[i]);
      else
	 set_config_string(section, name, NULL);
   }
}



/* handle the save command */
int saver()
{
   char buf[FILENAME_LENGTH];

   strcpy(buf, config_file);

   if (file_select_ex("Save Keyboard Config", buf, "CFG", sizeof(buf), 0, 0)) {
      if ((stricmp(config_file, buf) != 0) && (exists(buf))) {
	 if (alert("Overwrite existing file?", NULL, NULL, "Yes", "Cancel", 'y', 27) != 1)
	    return D_REDRAW;
      }

      strlwr(buf);
      strcpy(config_file, buf);

      push_config_state();
      set_config_file(buf);

      set_config_string(NULL, "keyboard_name", keyboard_name);

      set_config_int("key_escape", "accent1", _key_accent1);
      set_config_int("key_escape", "accent2", _key_accent2);
      set_config_int("key_escape", "accent3", _key_accent3);
      set_config_int("key_escape", "accent4", _key_accent4);
      set_config_int("key_escape", "accent1_flag", _key_accent1_flag);
      set_config_int("key_escape", "accent2_flag", _key_accent2_flag);
      set_config_int("key_escape", "accent3_flag", _key_accent3_flag);
      set_config_int("key_escape", "accent4_flag", _key_accent4_flag);

      save_table(_key_ascii_table,               orig_key_ascii_table,               "key_ascii");
      save_table(_key_capslock_table,            orig_key_capslock_table,            "key_capslock");
      save_table(_key_shift_table,               orig_key_shift_table,               "key_shift");
      save_table(_key_control_table,             orig_key_control_table,             "key_control");

      if (split_altgr) {
         save_table(_key_altgr_lower_table,      orig_key_altgr_lower_table,         "key_altgr_lower");
         save_table(_key_altgr_upper_table,      orig_key_altgr_upper_table,         "key_altgr_upper");
      }
      else {
         save_table(_key_altgr_lower_table,      orig_key_altgr_lower_table,         "key_altgr");
      }

      save_table(_key_accent1_lower_table,       orig_key_accent1_lower_table,       "key_accent1_lower");
      save_table(_key_accent1_upper_table,       orig_key_accent1_upper_table,       "key_accent1_upper");
      save_table(_key_accent2_lower_table,       orig_key_accent2_lower_table,       "key_accent2_lower");
      save_table(_key_accent2_upper_table,       orig_key_accent2_upper_table,       "key_accent2_upper");
      save_table(_key_accent3_lower_table,       orig_key_accent3_lower_table,       "key_accent3_lower");
      save_table(_key_accent3_upper_table,       orig_key_accent3_upper_table,       "key_accent3_upper");
      save_table(_key_accent4_lower_table,       orig_key_accent4_lower_table,       "key_accent4_lower");
      save_table(_key_accent4_upper_table,       orig_key_accent4_upper_table,       "key_accent4_upper");

      pop_config_state();
   }

   return D_REDRAW;
}



/* handle the quit command */
int quitter()
{
   if (alert("Really want to quit?", NULL, NULL, "Yes", "Cancel", 'y', 27) == 1)
      return D_CLOSE;
   else
      return D_O_K;
}



/* dialog procedure for ASCII character objects */
int ascii_proc(int msg, DIALOG *d, int c)
{
   int fg, bg, mg, k;

   if (msg == MSG_DRAW) {
      if (d->flags & D_SELECTED) {
	 fg = 16;
	 bg = 255;
	 mg = 8;
      }
      else if (d->flags & D_GOTFOCUS) {
	 fg = 255;
	 bg = 8;
	 mg = 16;
      }
      else {
	 fg = 255;
	 bg = 16;
	 mg = 8;
      }

      rectfill(screen, d->x+1, d->y+1, d->x+d->w-1, d->y+d->h-1, bg);
      rect(screen, d->x, d->y, d->x+d->w, d->y+d->h, fg);
      text_mode(-1);

      textprintf(screen, font, d->x+4, d->y+4, mg, "%02X", d->d1);

      k = (codepage << 8) | d->d1;

      if (k >= ' ')
	 textprintf_centre(screen, font, d->x+d->w-8, d->y+4, fg, "%c", k);
      else
	 textprintf_centre(screen, font, d->x+d->w-8, d->y+4, fg, "%s", ascii_name[k]);

      return D_O_K;
   }

   return d_button_proc(msg, d, c);
}



/* dialog procedure for codepage selection objects */
int codepage_proc(int msg, DIALOG *d, int c)
{
   int fg, bg, ret;

   if (msg == MSG_DRAW) {
      if (d->flags & D_SELECTED) {
	 fg = (d->d1 == codepage) ? 1 : 8;
	 bg = 16;
      }
      else if (d->flags & D_GOTFOCUS) {
	 fg = 16;
	 bg = (d->d1 == codepage) ? 1 : 8;
      }
      else { 
	 fg = 255;
	 bg = (d->d1 == codepage) ? 1 : 8;
      }

      rectfill(screen, d->x+1, d->y+1, d->x+d->w-1, d->y+d->h-1, bg);
      rect(screen, d->x, d->y, d->x+d->w, d->y+d->h, 255);
      text_mode(-1);

      textprintf_centre(screen, font, d->x+d->w/2, d->y+4, fg, "%04X", d->d1<<8);

      return D_O_K;
   }

   ret = d_button_proc(msg, d, c);

   if (ret & D_CLOSE) {
      codepage = d->d1;
      return D_REDRAW;
   }

   return ret;
}



#define ASCII_CHAR(n) \
   { ascii_proc, ((n)&15)*40, ((n)/16)*15, 40, 15, 0, 0, 0, D_EXIT, n, 0, NULL, NULL, NULL }


#define ASCII_ROW(n)                                                         \
   ASCII_CHAR(n),    ASCII_CHAR(n+1),  ASCII_CHAR(n+2),  ASCII_CHAR(n+3),    \
   ASCII_CHAR(n+4),  ASCII_CHAR(n+5),  ASCII_CHAR(n+6),  ASCII_CHAR(n+7),    \
   ASCII_CHAR(n+8),  ASCII_CHAR(n+9),  ASCII_CHAR(n+10), ASCII_CHAR(n+11),   \
   ASCII_CHAR(n+12), ASCII_CHAR(n+13), ASCII_CHAR(n+14), ASCII_CHAR(n+15) 


#define CPAGE_CHAR(n) \
   { codepage_proc, ((n)&15)*40, 240+((n)/16)*15, 40, 15, 0, 0, 0, D_EXIT, n, 0, NULL, NULL, NULL }


#define CPAGE_ROW(n)                                                         \
   CPAGE_CHAR(n),    CPAGE_CHAR(n+1),  CPAGE_CHAR(n+2),  CPAGE_CHAR(n+3),    \
   CPAGE_CHAR(n+4),  CPAGE_CHAR(n+5),  CPAGE_CHAR(n+6),  CPAGE_CHAR(n+7),    \
   CPAGE_CHAR(n+8),  CPAGE_CHAR(n+9),  CPAGE_CHAR(n+10), CPAGE_CHAR(n+11),   \
   CPAGE_CHAR(n+12), CPAGE_CHAR(n+13), CPAGE_CHAR(n+14), CPAGE_CHAR(n+15) 


DIALOG ascii_dlg[] =
{
   ASCII_ROW(0x00),  ASCII_ROW(0x10),  ASCII_ROW(0x20),  ASCII_ROW(0x30),
   ASCII_ROW(0x40),  ASCII_ROW(0x50),  ASCII_ROW(0x60),  ASCII_ROW(0x70),
   ASCII_ROW(0x80),  ASCII_ROW(0x90),  ASCII_ROW(0xA0),  ASCII_ROW(0xB0),
   ASCII_ROW(0xC0),  ASCII_ROW(0xD0),  ASCII_ROW(0xE0),  ASCII_ROW(0xF0),

   CPAGE_ROW(0x00),  CPAGE_ROW(0x10),  CPAGE_ROW(0x20),  CPAGE_ROW(0x30),
   CPAGE_ROW(0x40),  CPAGE_ROW(0x50),  CPAGE_ROW(0x60),  CPAGE_ROW(0x70),
   CPAGE_ROW(0x80),  CPAGE_ROW(0x90),  CPAGE_ROW(0xA0),  CPAGE_ROW(0xB0),
   CPAGE_ROW(0xC0),  CPAGE_ROW(0xD0),  CPAGE_ROW(0xE0),  CPAGE_ROW(0xF0),

   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};



/* inputs an ASCII code */
int get_ascii_code(int *val)
{
   int ox = mouse_x;
   int oy = mouse_y;
   int ret;
   int c;

   codepage = *val >> 8;
   c = *val & 0xFF;

   position_mouse(ascii_dlg[c].x+ascii_dlg[c].w/2, ascii_dlg[c].y+ascii_dlg[c].h/2);

   ret = do_dialog(ascii_dlg, c);

   position_mouse(ox, oy);

   if ((ret >= 0) && (ret < 256)) {
      *val = (codepage << 8) | ret;
      return TRUE;
   }

   return FALSE;
}



/* dialog callback for retrieving information about the keymap list */
char *keymap_list_getter(int index, int *list_size)
{
   static char buf[256];
   int val;

   if (index < 0) {
      if (list_size)
	 *list_size = KEY_MAX;
      return NULL;
   }

   val = editor_table[index];

   if (val >= ' ')
      usprintf(buf, "%-16s ->  U+%04X - '%c'", scancode_name[index], val, val);
   else
      usprintf(buf, "%-16s ->  U+%04X - %s", scancode_name[index], val, ascii_name[val]);

   return buf;
}



/* dialog procedure for the keymap listbox */
int keymap_list_proc(int msg, DIALOG *d, int c)
{
   int ret;

   if (msg == MSG_LOSTFOCUS)
      return D_WANTFOCUS;

   ret = d_list_proc(msg, d, c);

   if (ret & D_CLOSE) {
      int val = editor_table[d->d1];

      if (get_ascii_code(&val))
	 editor_table[d->d1] = val;

      return D_REDRAW;
   }

   return ret;
}



DIALOG editor_dlg[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key)    (flags)     (d1)           (d2)     (dp)                 (dp2) (dp3) */
   { d_clear_proc,      0,    0,    0,    0,    0,    8,    0,       0,          0,             0,       NULL,                NULL, NULL  },
   { d_ctext_proc,      320,  6,    0,    0,    0,    8,    0,       0,          0,             0,       NULL,                NULL, NULL  },
   { keymap_list_proc,  16,   32,   609,  404,  255,  16,   0,       D_EXIT,     0,             0,       keymap_list_getter,  NULL, NULL  },
   { d_button_proc,     280,  448,  81,   17,   255,  16,   0,       D_EXIT,     0,             0,       "Exit",              NULL, NULL  },
   { NULL,              0,    0,    0,    0,    0,    0,    0,       0,          0,             0,       NULL,                NULL, NULL  }
};



/* core keymap editing function */
int editor()
{
   editor_table = active_menu->dp;
   editor_dlg[1].dp = active_menu->text;

   do_dialog(editor_dlg, -1);

   return D_REDRAW;
}



/* dialog callback for retrieving information about the accent key */
char *accent_list_getter(int index, int *list_size)
{
   if (index < 0) {
      if (list_size)
	 *list_size = KEY_MAX;
      return NULL;
   }

   return scancode_name[index];
}



DIALOG accent_dlg[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key)    (flags)     (d1)           (d2)     (dp)                 (dp2) (dp3) */
   { d_clear_proc,      0,    0,    0,    0,    0,    8,    0,       0,          0,             0,       NULL,                NULL, NULL  },
   { d_ctext_proc,      320,  6,    0,    0,    255,  8,    0,       0,          0,             0,       NULL,                NULL, NULL  },
   { d_list_proc,       16,   32,   609,  341,  255,  16,   0,       D_EXIT,     0,             0,       accent_list_getter,  NULL, NULL  },
   { d_button_proc,     220,  448,  81,   17,   255,  16,   0,       D_EXIT,     0,             0,       "OK",                NULL, NULL  },
   { d_button_proc,     340,  448,  81,   17,   255,  16,   0,       D_EXIT,     0,             0,       "Cancel",            NULL, NULL  },
   { d_box_proc,        16,   380,  609,  56,   255,  16,   0,       0,          0,             0,       NULL,                NULL, NULL  },
   { d_radio_proc,      36,   413,  51,   11,   255,  16,   0,       0,          0,             0,       "None",              NULL, NULL  },
   { d_radio_proc,      179,  413,  59,   11,   255,  16,   0,       0,          0,             0,       "Shift",             NULL, NULL  },
   { d_radio_proc,      346,  413,  75,   11,   255,  16,   0,       0,          0,             0,       "Control",           NULL, NULL  },
   { d_radio_proc,      516,  413,  43,   11,   255,  16,   0,       0,          0,             0,       "Alt",               NULL, NULL  },
   { d_ctext_proc,      319,  392,  224,  8,    255,  16,   0,       0,          0,             0,       "Set the accent activator key", NULL, NULL },
   { NULL,              0,    0,    0,    0,    0,    0,    0,       0,          0,             0,       NULL,                NULL, NULL  }
};



/* accent setting function */
int accenter(char *title, int flag)
{
   int ret;

   accent_dlg[1].dp = title;
   accent_dlg[2].d2 = 0;
   accent_dlg[6].flags = accent_dlg[7].flags = 0;
   accent_dlg[8].flags = accent_dlg[9].flags = 0;

   if (flag == KB_ACCENT1_FLAG) {
      accent_dlg[2].d1 = _key_accent1;
      if (_key_accent1_flag == 0) accent_dlg[6].flags = D_SELECTED;
      else if (_key_accent1_flag == 1) accent_dlg[7].flags = D_SELECTED;
      else if (_key_accent1_flag == 2) accent_dlg[8].flags = D_SELECTED;
      else if (_key_accent1_flag == 4) accent_dlg[9].flags = D_SELECTED;
   }
   else if (flag == KB_ACCENT2_FLAG) {
      accent_dlg[2].d1 = _key_accent2;
      if (_key_accent2_flag == 0) accent_dlg[6].flags = D_SELECTED;
      else if (_key_accent2_flag == 1) accent_dlg[7].flags = D_SELECTED;
      else if (_key_accent2_flag == 2) accent_dlg[8].flags = D_SELECTED;
      else if (_key_accent2_flag == 4) accent_dlg[9].flags = D_SELECTED;
   }
   else if (flag == KB_ACCENT3_FLAG) {
      accent_dlg[2].d1 = _key_accent3;
      if (_key_accent3_flag == 0) accent_dlg[6].flags = D_SELECTED;
      else if (_key_accent3_flag == 1) accent_dlg[7].flags = D_SELECTED;
      else if (_key_accent3_flag == 2) accent_dlg[8].flags = D_SELECTED;
      else if (_key_accent3_flag == 4) accent_dlg[9].flags = D_SELECTED;
   }
   else if (flag == KB_ACCENT4_FLAG) {
      accent_dlg[2].d1 = _key_accent4;
      if (_key_accent4_flag == 0) accent_dlg[6].flags = D_SELECTED;
      else if (_key_accent4_flag == 1) accent_dlg[7].flags = D_SELECTED;
      else if (_key_accent4_flag == 2) accent_dlg[8].flags = D_SELECTED;
      else if (_key_accent4_flag == 4) accent_dlg[9].flags = D_SELECTED;
   }

   ret = do_dialog(accent_dlg, -1);

   if ((ret == 2) || (ret == 3)) {
      if (flag == KB_ACCENT1_FLAG) {
	 _key_accent1 = accent_dlg[2].d1;
	 if (accent_dlg[6].flags & D_SELECTED) _key_accent1_flag = 0;
	 else if (accent_dlg[7].flags & D_SELECTED) _key_accent1_flag = 1;
	 else if (accent_dlg[8].flags & D_SELECTED) _key_accent1_flag = 2;
	 else if (accent_dlg[9].flags & D_SELECTED) _key_accent1_flag = 4;
      }
      else if (flag == KB_ACCENT2_FLAG) {
	 _key_accent2 = accent_dlg[2].d1;
	 if (accent_dlg[6].flags & D_SELECTED) _key_accent2_flag = 0;
	 else if (accent_dlg[7].flags & D_SELECTED) _key_accent2_flag = 1;
	 else if (accent_dlg[8].flags & D_SELECTED) _key_accent2_flag = 2;
	 else if (accent_dlg[9].flags & D_SELECTED) _key_accent2_flag = 4;
      }
      else if (flag == KB_ACCENT3_FLAG) {
	 _key_accent3 = accent_dlg[2].d1;
	 if (accent_dlg[6].flags & D_SELECTED) _key_accent3_flag = 0;
	 else if (accent_dlg[7].flags & D_SELECTED) _key_accent3_flag = 1;
	 else if (accent_dlg[8].flags & D_SELECTED) _key_accent3_flag = 2;
	 else if (accent_dlg[9].flags & D_SELECTED) _key_accent3_flag = 4;
      }
      else if (flag == KB_ACCENT4_FLAG) {
	 _key_accent4 = accent_dlg[2].d1;
	 if (accent_dlg[6].flags & D_SELECTED) _key_accent4_flag = 0;
	 else if (accent_dlg[7].flags & D_SELECTED) _key_accent4_flag = 1;
	 else if (accent_dlg[8].flags & D_SELECTED) _key_accent4_flag = 2;
	 else if (accent_dlg[9].flags & D_SELECTED) _key_accent4_flag = 4;
      }
   }

   return D_REDRAW;
}



int accenter1()
{
   return accenter("Set Accent Key #1", KB_ACCENT1_FLAG);
}



int accenter2()
{
   return accenter("Set Accent Key #2", KB_ACCENT2_FLAG);
}



int accenter3()
{
   return accenter("Set Accent Key #3", KB_ACCENT3_FLAG);
}



int accenter4()
{
   return accenter("Set Accent Key #4", KB_ACCENT4_FLAG);
}



/* handle the test command */
int tester()
{
   char buf[256];
   int a, i;

   show_mouse(NULL);

   acquire_screen();
   clear_to_color(screen, palette_color[8]);

   text_mode(palette_color[8]);

   for (i=0; i<=KEY_MAX; i++)
      textout(screen, font, scancode_name[i], 32+(i%4)*160, 88+(i/4)*14, palette_color[255]);

   release_screen();

   do {
      poll_keyboard();
      poll_mouse();
   } while ((key[KEY_ESC]) || (mouse_b));

   do {
      poll_keyboard();
      poll_mouse();

      acquire_screen();

      for (i=0; i<KEY_MAX; i++)
	 textout(screen, font, key[i] ? "*" : " ", 16+(i%4)*160, 88+(i/4)*14, palette_color[255]);

      buf[0] = 0;

      if (key_shifts & KB_SHIFT_FLAG)
	 strcat(buf, "shift ");

      if (key_shifts & KB_CTRL_FLAG)
	 strcat(buf, "ctrl ");

      if (key_shifts & KB_ALT_FLAG)
	 strcat(buf, "alt ");

      if (key_shifts & KB_LWIN_FLAG)
	 strcat(buf, "lwin ");

      if (key_shifts & KB_RWIN_FLAG)
	 strcat(buf, "rwin ");

      if (key_shifts & KB_MENU_FLAG)
	 strcat(buf, "menu ");

      if (key_shifts & KB_SCROLOCK_FLAG)
	 strcat(buf, "scrolock ");

      if (key_shifts & KB_NUMLOCK_FLAG)
	 strcat(buf, "numlock ");

      if (key_shifts & KB_CAPSLOCK_FLAG)
	 strcat(buf, "capslock ");

      if (key_shifts & KB_INALTSEQ_FLAG)
	 strcat(buf, "inaltseq ");

      if (key_shifts & KB_ACCENT1_FLAG)
	 strcpy(buf, "accent1 ");

      if (key_shifts & KB_ACCENT2_FLAG)
	 strcpy(buf, "accent2 ");

      if (key_shifts & KB_ACCENT3_FLAG)
	 strcpy(buf, "accent3 ");

      if (key_shifts & KB_ACCENT4_FLAG)
	 strcpy(buf, "accent4 ");

      while (strlen(buf) < 128)
	 strcat(buf, " ");

      textout(screen, font, buf, 0, 0, palette_color[255]);

      release_screen();

      if (keypressed()) {
	 a = ureadkey(&i);
	 if (!a)
	    a = ' ';
	 textprintf(screen, font, 32, 48, palette_color[255], "ureadkey() returns scancode 0x%02X, U+0x%04X, '%c'", i, a, a);
      }

   } while ((!key[KEY_ESC]) && (!mouse_b));

   do {
      poll_keyboard();
      poll_mouse();
   } while ((key[KEY_ESC]) || (mouse_b));

   clear_keybuf();

   show_mouse(screen);
   return D_REDRAW;
}



int main(int argc, char *argv[])
{
   static char config_override[] = "[system]\nkeyboard = \n";
   RGB black_rgb = {0, 0, 0, 0};
   DATAFILE *font_data;
   int c;

   allegro_init();

   if (argc > 1) {
      if (strcmp(argv[1], "--split-altgr") == 0) {
         split_altgr = TRUE;
      }
      else {
         allegro_message("Error: unrecognized option\n");
         exit(EXIT_FAILURE);
      }
   }

   install_mouse();
   install_timer();

   push_config_state();
   override_config_data(config_override, sizeof(config_override));
   install_keyboard();
   pop_config_state();

   memcpy(orig_key_ascii_table, _key_ascii_table, sizeof(orig_key_ascii_table));
   memcpy(orig_key_capslock_table, _key_capslock_table, sizeof(orig_key_capslock_table));
   memcpy(orig_key_shift_table, _key_shift_table, sizeof(orig_key_shift_table));
   memcpy(orig_key_control_table, _key_control_table, sizeof(orig_key_control_table));
   memcpy(orig_key_altgr_lower_table, _key_altgr_lower_table, sizeof(orig_key_altgr_lower_table));
   memcpy(orig_key_altgr_upper_table, _key_altgr_upper_table, sizeof(orig_key_altgr_upper_table));
   memcpy(orig_key_accent1_lower_table, _key_accent1_lower_table, sizeof(orig_key_accent1_lower_table));
   memcpy(orig_key_accent1_upper_table, _key_accent1_upper_table, sizeof(orig_key_accent1_upper_table));
   memcpy(orig_key_accent2_lower_table, _key_accent2_lower_table, sizeof(orig_key_accent2_lower_table));
   memcpy(orig_key_accent2_upper_table, _key_accent2_upper_table, sizeof(orig_key_accent2_upper_table));
   memcpy(orig_key_accent3_lower_table, _key_accent3_lower_table, sizeof(orig_key_accent3_lower_table));
   memcpy(orig_key_accent3_upper_table, _key_accent3_upper_table, sizeof(orig_key_accent3_upper_table));
   memcpy(orig_key_accent4_lower_table, _key_accent4_lower_table, sizeof(orig_key_accent4_lower_table));
   memcpy(orig_key_accent4_upper_table, _key_accent4_upper_table, sizeof(orig_key_accent4_upper_table));

   memcpy(my_key_ascii_table, _key_ascii_table, sizeof(my_key_ascii_table));
   memcpy(my_key_capslock_table, _key_capslock_table, sizeof(my_key_capslock_table));
   memcpy(my_key_shift_table, _key_shift_table, sizeof(my_key_shift_table));
   memcpy(my_key_control_table, _key_control_table, sizeof(my_key_control_table));
   memcpy(my_key_altgr_lower_table, _key_altgr_lower_table, sizeof(my_key_altgr_lower_table));
   memcpy(my_key_altgr_upper_table, _key_altgr_upper_table, sizeof(my_key_altgr_upper_table));
   memcpy(my_key_accent1_lower_table, _key_accent1_lower_table, sizeof(my_key_accent1_lower_table));
   memcpy(my_key_accent1_upper_table, _key_accent1_upper_table, sizeof(my_key_accent1_upper_table));
   memcpy(my_key_accent2_lower_table, _key_accent2_lower_table, sizeof(my_key_accent2_lower_table));
   memcpy(my_key_accent2_upper_table, _key_accent2_upper_table, sizeof(my_key_accent2_upper_table));
   memcpy(my_key_accent3_lower_table, _key_accent3_lower_table, sizeof(my_key_accent3_lower_table));
   memcpy(my_key_accent3_upper_table, _key_accent3_upper_table, sizeof(my_key_accent3_upper_table));
   memcpy(my_key_accent4_lower_table, _key_accent4_lower_table, sizeof(my_key_accent4_lower_table));
   memcpy(my_key_accent4_upper_table, _key_accent4_upper_table, sizeof(my_key_accent4_upper_table));

   _key_ascii_table = my_key_ascii_table;
   _key_capslock_table = my_key_capslock_table;
   _key_shift_table = my_key_shift_table;
   _key_control_table = my_key_control_table;
   _key_altgr_lower_table = my_key_altgr_lower_table;
   _key_altgr_upper_table = my_key_altgr_upper_table;
   _key_accent1_lower_table = my_key_accent1_lower_table;
   _key_accent1_upper_table = my_key_accent1_upper_table;
   _key_accent2_lower_table = my_key_accent2_lower_table;
   _key_accent2_upper_table = my_key_accent2_upper_table;
   _key_accent3_lower_table = my_key_accent3_lower_table;
   _key_accent3_upper_table = my_key_accent3_upper_table;
   _key_accent4_lower_table = my_key_accent4_lower_table;
   _key_accent4_upper_table = my_key_accent4_upper_table;

   edit_menu[0].dp = _key_ascii_table;
   edit_menu[1].dp = _key_capslock_table;
   edit_menu[2].dp = _key_shift_table;
   edit_menu[3].dp = _key_control_table;
   edit_menu[4].dp = _key_altgr_lower_table;
   edit_menu[5].dp = _key_altgr_upper_table;
   edit_menu[6].dp = _key_accent1_lower_table;
   edit_menu[7].dp = _key_accent1_upper_table;
   edit_menu[8].dp = _key_accent2_lower_table;
   edit_menu[9].dp = _key_accent2_upper_table;
   edit_menu[10].dp = _key_accent3_lower_table;
   edit_menu[11].dp = _key_accent3_upper_table;
   edit_menu[12].dp = _key_accent4_lower_table;
   edit_menu[13].dp = _key_accent4_upper_table;

   if (!split_altgr)
      edit_menu[5].flags = D_DISABLED;

   if (set_gfx_mode(GFX_SAFE, 640, 480, 0, 0) != 0) {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
      exit(EXIT_FAILURE);
   }
   set_palette(desktop_palette);
   set_color(0, &black_rgb);
   gui_fg_color = palette_color[255];
   gui_bg_color = palette_color[16];

   /* We set up colors to match screen color depth (in case it changed) */
   for (c = 0; main_dlg[c].proc; c++) {
      main_dlg[c].fg = palette_color[main_dlg[c].fg];
      main_dlg[c].bg = palette_color[main_dlg[c].bg];
   }
   for (c = 0; ascii_dlg[c].proc; c++) {
      ascii_dlg[c].fg = palette_color[ascii_dlg[c].fg];
      ascii_dlg[c].bg = palette_color[ascii_dlg[c].bg];
   }
   for (c = 0; editor_dlg[c].proc; c++) {
      editor_dlg[c].fg = palette_color[editor_dlg[c].fg];
      editor_dlg[c].bg = palette_color[editor_dlg[c].bg];
   }
   for (c = 0; accent_dlg[c].proc; c++) {
      accent_dlg[c].fg = palette_color[accent_dlg[c].fg];
      accent_dlg[c].bg = palette_color[accent_dlg[c].bg];
   }

   _key_standard_kb = FALSE;

   font_data = load_datafile_object ("keyconf.dat", "BASE_FONT");

   if (font_data)
      font = font_data->dat;

   do_dialog(main_dlg, -1);

   if (font_data)
      unload_datafile_object(font_data);

   return 0;
}

END_OF_MAIN();
