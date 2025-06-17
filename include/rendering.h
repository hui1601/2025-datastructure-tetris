#pragma once
#ifndef RENDERING_H
#define RENDERING_H

#define RENDER_CONFIG_FILE "tetris_config.dat"

typedef enum {
  RENDER_MODE_UNICODE_BLOCK_ELEMENTS,
  RENDER_MODE_BACKGROUND_COLOR,
  RENDER_MODE_EMOJI,
  RENDER_MODE_PLATFORM_COLOR_CODES,
  RENDER_MODE_ASCII_ONLY,
} render_mode_t;

extern render_mode_t current_render_mode;

// 블록 색상 정의(실제 색상은 플랫폼 정의)
#define RENDER_BLOCK_COLOR_I 0
#define RENDER_BLOCK_COLOR_T 1
#define RENDER_BLOCK_COLOR_S 2
#define RENDER_BLOCK_COLOR_Z 3
#define RENDER_BLOCK_COLOR_L 4
#define RENDER_BLOCK_COLOR_J 5
#define RENDER_BLOCK_COLOR_O 6
#define RENDER_BLOCK_COLOR_WALL 7
// 나머지 기본 블록 색상 + 특수 색상
#define RENDER_BLOCK_COLOR_GHOST 8
#define RENDER_BLOCK_COLOR_DEFAULT 9

void render_init(void);
void render_ask_user_preference(void);
void render_save_settings(render_mode_t mode);
render_mode_t render_load_settings(void);

void render_set_color_for_cell_value(int cell_value);
void render_reset_color(void);
void render_print_block_segment(int cell_value);
void render_print_empty_segment(void);
void render_print_ghost_segment(void);

void render_set_color_for_block_type(int block_type);
void render_print_preview_segment(int block_type);

#endif  // RENDERING_H
