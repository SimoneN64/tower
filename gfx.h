//
// Created by simone on 4/26/24.
//

#pragma once

typedef struct _BITMAP {
  int w,h,clip,cl,cr,ct,cb;
  struct _GFX_VTABLE* vtable;
  void* write_bank, *read_bank, *dat;
  unsigned long id;
  void* extra;
  int x_ofs, y_ofs, seg;
  unsigned char* line[0];
} BITMAP;

typedef struct _RLE_SPRITE {
  int w,h,color_depth,size;
  char dat[0];
} RLE_SPRITE;

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

typedef struct _FONT_GLYPH {
  short w, h;
  unsigned char dat[0];
} FONT_GLYPH;

typedef struct _FONT {
  void *data;
  int height;
  struct _FONT_VTABLE *vtable;
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