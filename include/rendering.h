#pragma once
#ifndef RENDERING_H
#define RENDERING_H

#include "tetris.h"
#include "types.h"

#define RENDER_CONFIG_FILE "tetris_config.dat"

typedef enum {
  // "■"+색상 사용
  RENDER_MODE_UNICODE_BLOCK_ELEMENTS,
  // "##"+색상 사용
  RENDER_MODE_PLATFORM_COLOR_CODES,
  // "II", "TT" 등 ASCII art 사용
  RENDER_MODE_ASCII_ONLY
} render_mode_t;

extern render_mode_t current_render_mode;

// 블록 색상 정의(실제 색상은 플랫폼 정의)
#define BLOCK_COLOR_I 0
#define BLOCK_COLOR_T 1
#define BLOCK_COLOR_S 2
#define BLOCK_COLOR_Z 3
#define BLOCK_COLOR_L 4
#define BLOCK_COLOR_J 5
#define BLOCK_COLOR_O 6
#define BLOCK_COLOR_WALL 7
// 나머지 기본 블록 색상
#define BLOCK_COLOR_DEFAULT 8

void init_rendering_system(void);
void ask_user_for_render_preference(void);
void save_render_settings(render_mode_t mode);
render_mode_t load_render_settings(void);

void render_set_color_for_cell_value(int cell_value);
void render_reset_color(void);
void render_print_block_segment(int cell_value);
void render_print_empty_segment(void);

void render_set_color_for_block_type(int block_type);
void render_print_preview_segment(int block_type);

#endif  // RENDERING_H
