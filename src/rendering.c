#include "rendering.h"

#include <stdio.h>
#include <stdlib.h>

#include "tetris.h"

#ifdef _WIN32
#include <windows.h>
static HANDLE hConsole = NULL;
static WORD original_attributes = 0;
#define FOREGROUND_CYAN (FOREGROUND_BLUE | FOREGROUND_GREEN)
#define FOREGROUND_MAGENTA (FOREGROUND_RED | FOREGROUND_BLUE)
#define FOREGROUND_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)
#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#endif

render_mode_t current_render_mode = RENDER_MODE_UNICODE_BLOCK_ELEMENTS;

/* 아스키 모드에서 블록 유형에 해당하는 문자 반환 */
char block_type_to_char(int block_type) {
  switch (block_type) {
    case I_BLOCK:
      return 'I';
    case T_BLOCK:
      return 'T';
    case S_BLOCK:
      return 'S';
    case Z_BLOCK:
      return 'Z';
    case L_BLOCK:
      return 'L';
    case J_BLOCK:
      return 'J';
    case O_BLOCK:
      return 'O';
    default:
      return ' ';
  }
}

/* 블록 유형에 해당하는 이모지 문자열 반환 */
const char* get_emoji_for_block_type(int block_type) {
  switch (block_type) {
    case I_BLOCK:
      return "🟫";
    case T_BLOCK:
      return "🟪";
    case S_BLOCK:
      return "🟩";
    case Z_BLOCK:
      return "🟥";
    case L_BLOCK:
      return "🟧";
    case J_BLOCK:
      return "🟦";
    case O_BLOCK:
      return "🟨";
    default:
      return "  ";
  }
}

/* 벽에 해당하는 이모지 문자열 반환 */
const char* get_emoji_for_wall(void) {
  return "⬛";  // Black Square for Wall
}

/* 빈 공간에 해당하는 공백 문자열 반환 */
const char* get_emoji_for_empty(void) {
  return "  ";
}

/* 렌더링 설정 저장 */
void save_render_settings(render_mode_t mode) {
  FILE* f = fopen(RENDER_CONFIG_FILE, "wb");
  if (f) {
    fwrite(&mode, sizeof(render_mode_t), 1, f);
    fclose(f);
  } else {
    fprintf(stderr, "Warning: Could not save render settings to %s\n",
            RENDER_CONFIG_FILE);
  }
}

/* 렌더링 설정 불러오기 */
render_mode_t load_render_settings(void) {
  render_mode_t mode = RENDER_MODE_UNICODE_BLOCK_ELEMENTS;
  FILE* f = fopen(RENDER_CONFIG_FILE, "rb");
  if (f) {
    if (fread(&mode, sizeof(render_mode_t), 1, f) != 1) {
      mode = RENDER_MODE_UNICODE_BLOCK_ELEMENTS;
      fprintf(
          stderr,
          "Warning: Could not read render settings from %s, using default.\n",
          RENDER_CONFIG_FILE);
    }
    fclose(f);
    if (mode < RENDER_MODE_UNICODE_BLOCK_ELEMENTS ||
        mode > RENDER_MODE_ASCII_ONLY) {
      fprintf(
          stderr,
          "Warning: Invalid render setting %d found in %s, using default.\n",
          mode, RENDER_CONFIG_FILE);
      mode = RENDER_MODE_UNICODE_BLOCK_ELEMENTS;
    }
  }
  return mode;
}

/* 콘솔 색상 설정 */
void print_sample_line(const char* mode_name,
                       int block_type_example,
                       const char* block_char_override) {
  printf("\t\t    %s Example: ", mode_name);
  if (current_render_mode == RENDER_MODE_EMOJI) {
    if (block_char_override) {
      printf("%s", block_char_override);
    } else if (block_type_example == -1) {
      printf("%s", get_emoji_for_wall());
    } else {
      printf("%s", get_emoji_for_block_type(block_type_example));
    }
  } else {
    render_set_color_for_block_type(block_type_example);
    printf("%s", block_char_override);
    render_reset_color();
  }
  printf("\n");
}

/* 사용자에게 렌더링 모드 선택 요청 */
void ask_user_for_render_preference(void) {
  int choice = -1;
  render_mode_t old_mode = current_render_mode;

  clear_screen();
  printf("\n\n\t\t[ Display Configuration ]\n");
  printf("\t\t=========================================\n");
  printf("\t\tPlease choose your preferred rendering style:\n\n");

  current_render_mode = RENDER_MODE_UNICODE_BLOCK_ELEMENTS;
  printf("\t\t1. Unicode Block Characters with Color (Recommended)\n");
  print_sample_line("I-Block", I_BLOCK, "■ ");
  print_sample_line("Wall", -1, "■ ");
  printf("\t\t   (Uses ■ character. Looks best on modern terminals.)\n\n");

  current_render_mode = RENDER_MODE_BACKGROUND_COLOR;
  printf("\t\t2. Background Color with Spaces\n");
  print_sample_line("S-Block", S_BLOCK, "  ");
  print_sample_line("Wall", -1, "  ");
  printf(
      "\t\t   (Uses '  ' with colored background. Good for minimalist "
      "style.)\n\n");

  current_render_mode = RENDER_MODE_EMOJI;
  printf("\t\t3. Emoji Characters (Experimental)\n");
  print_sample_line("Z-Block", Z_BLOCK, get_emoji_for_block_type(Z_BLOCK));
  print_sample_line("Wall", -1, get_emoji_for_wall());
  printf(
      "\t\t   (Uses emoji like 🟥, ⬛. Requires good Unicode font "
      "support. Most terminals cannot display full-width emoji "
      "correctly.)\n\n");

  current_render_mode = RENDER_MODE_PLATFORM_COLOR_CODES;
  printf("\t\t4. Standard Terminal Colors with '##'\n");
  print_sample_line("T-Block", T_BLOCK, "##");
  print_sample_line("Wall", -1, "##");
  printf("\t\t   (Uses ## characters. Good if ■ has issues.)\n\n");

  current_render_mode = RENDER_MODE_ASCII_ONLY;
  printf("\t\t5. ASCII Only (Most Compatible)\n");
  print_sample_line("L-Block", L_BLOCK, "LL");
  print_sample_line("Wall", -1, "##");
  printf(
      "\t\t   (Uses letter pairs like II, TT. No special characters or "
      "colors.)\n\n");

  current_render_mode = old_mode;

  printf("\t\t=========================================\n");
  printf("\t\tEnter your choice (1-5): ");

  char buffer[10];
  if (scanf("%9s", buffer) == 1) {
    choice = atoi(buffer);
    CLEAR_INPUT_BUFFER();
  }

  switch (choice) {
    case 1:
      current_render_mode = RENDER_MODE_UNICODE_BLOCK_ELEMENTS;
      break;
    case 2:
      current_render_mode = RENDER_MODE_BACKGROUND_COLOR;
      break;
    case 3:
      current_render_mode = RENDER_MODE_EMOJI;
      break;
    case 4:
      current_render_mode = RENDER_MODE_PLATFORM_COLOR_CODES;
      break;
    case 5:
      current_render_mode = RENDER_MODE_ASCII_ONLY;
      break;
    default:
      printf("\n\t\tInvalid choice. Using default (Unicode Block Elements).\n");
      current_render_mode = RENDER_MODE_UNICODE_BLOCK_ELEMENTS;
      platform_usleep(2000000);
      break;
  }
  save_render_settings(current_render_mode);
}

/* 콘솔 색상 초기화 및 렌더링 시스템 초기화 */
void init_rendering_system(void) {
#ifdef _WIN32
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hConsole != INVALID_HANDLE_VALUE) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    original_attributes = csbi.wAttributes;
  }
#endif

  FILE* f = fopen(RENDER_CONFIG_FILE, "rb");
  if (f) {
    current_render_mode = load_render_settings();
    fclose(f);
  } else {
    ask_user_for_render_preference();
  }
}

/* 콘솔 배경 색상 설정 */
static void internal_set_background_color(int color_id) {
  if (current_render_mode == RENDER_MODE_ASCII_ONLY ||
      current_render_mode == RENDER_MODE_EMOJI ||
      current_render_mode == RENDER_MODE_UNICODE_BLOCK_ELEMENTS ||
      current_render_mode == RENDER_MODE_PLATFORM_COLOR_CODES) {
    return;
  }

#ifdef _WIN32
  if (!hConsole)
    return;
  WORD bg_attribute_bits = 0;
  switch (color_id) {
    case BLOCK_COLOR_I:
      bg_attribute_bits = BACKGROUND_BLUE | BACKGROUND_GREEN;
      break;
    case BLOCK_COLOR_L:
      bg_attribute_bits = BACKGROUND_RED | BACKGROUND_GREEN;
      break;
    case BLOCK_COLOR_J:
      bg_attribute_bits = BACKGROUND_BLUE;
      break;
    case BLOCK_COLOR_S:
      bg_attribute_bits = BACKGROUND_GREEN;
      break;
    case BLOCK_COLOR_Z:
      bg_attribute_bits = BACKGROUND_RED;
      break;
    case BLOCK_COLOR_T:
      bg_attribute_bits = BACKGROUND_RED | BACKGROUND_BLUE;
      break;
    case BLOCK_COLOR_O:
      bg_attribute_bits =
          BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
      break;
    case BLOCK_COLOR_WALL:
      bg_attribute_bits = BACKGROUND_INTENSITY;
      break;
    case BLOCK_COLOR_GHOST:
      bg_attribute_bits = BACKGROUND_INTENSITY;
      break;
    case BLOCK_COLOR_DEFAULT:
    default:
      SetConsoleTextAttribute(hConsole, original_attributes);
      return;
  }
  SetConsoleTextAttribute(hConsole,
                          (original_attributes & 0x0F) | bg_attribute_bits);
#else  // Unix-like systems
  const char* bg_color_code;
  switch (color_id) {
    case BLOCK_COLOR_I:
      bg_color_code = "\033[106m";  // Bright Cyan BG
      break;
    case BLOCK_COLOR_L:
      bg_color_code = "\033[43m";  // Dark Yellow BG (for Orange)
      break;
    case BLOCK_COLOR_J:
      bg_color_code = "\033[104m";  // Bright Blue BG
      break;
    case BLOCK_COLOR_S:
      bg_color_code = "\033[102m";  // Bright Green BG
      break;
    case BLOCK_COLOR_Z:
      bg_color_code = "\033[101m";  // Bright Red BG
      break;
    case BLOCK_COLOR_T:
      bg_color_code = "\033[105m";  // Bright Magenta BG
      break;
    case BLOCK_COLOR_O:
      bg_color_code = "\033[103m";  // Bright Yellow BG
      break;
    case BLOCK_COLOR_WALL:
      bg_color_code = "\033[47m";  // White BG
      break;
    case BLOCK_COLOR_GHOST:
      bg_color_code = "\033[100m";  // Bright Black BG (Dark Gray BG)
      break;
    case BLOCK_COLOR_DEFAULT:
    default:
      bg_color_code = "\033[49m";  // Reset background to default
      break;
  }
  printf("%s", bg_color_code);
#endif
}

/* 콘솔 색상 설정 */
void internal_set_color(int color_id) {
  if (current_render_mode == RENDER_MODE_ASCII_ONLY &&
      color_id != BLOCK_COLOR_DEFAULT && color_id != BLOCK_COLOR_WALL) {
    return;
  }
  if (current_render_mode == RENDER_MODE_EMOJI &&
      color_id != BLOCK_COLOR_DEFAULT) {
    return;
  }
  if (current_render_mode == RENDER_MODE_BACKGROUND_COLOR) {
    if (color_id != BLOCK_COLOR_DEFAULT) {
      return;
    }
  }
#ifdef _WIN32
  if (!hConsole)
    return;
  WORD attribute = original_attributes;
  switch (color_id) {
    case BLOCK_COLOR_I:
      attribute = FOREGROUND_CYAN;
      break;
    case BLOCK_COLOR_L:
      attribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
      break;
    case BLOCK_COLOR_J:
      attribute = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
      break;
    case BLOCK_COLOR_S:
      attribute = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
      break;
    case BLOCK_COLOR_Z:
      attribute = FOREGROUND_RED | FOREGROUND_INTENSITY;
      break;
    case BLOCK_COLOR_T:
      attribute = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
      break;
    case BLOCK_COLOR_O:
      attribute = FOREGROUND_RED | FOREGROUND_GREEN;
      break;
    case BLOCK_COLOR_WALL:
      attribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
      break;
    case BLOCK_COLOR_DEFAULT:
    default:
      attribute = original_attributes;
      break;
  }
  if (color_id != BLOCK_COLOR_DEFAULT && color_id != BLOCK_COLOR_WALL) {
    attribute = (original_attributes & 0xF0) | (attribute & 0x0F);
  } else if (color_id == BLOCK_COLOR_WALL) {
    attribute = (original_attributes & 0xF0) |
                (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  }
  switch (color_id) {
    case BLOCK_COLOR_I:
      SetConsoleTextAttribute(hConsole, FOREGROUND_CYAN | FOREGROUND_INTENSITY);
      break;
    case BLOCK_COLOR_L:
      SetConsoleTextAttribute(hConsole, FOREGROUND_YELLOW);
      break;
    case BLOCK_COLOR_J:
      SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
      break;
    case BLOCK_COLOR_S:
      SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
      break;
    case BLOCK_COLOR_Z:
      SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
      break;
    case BLOCK_COLOR_T:
      SetConsoleTextAttribute(hConsole, FOREGROUND_MAGENTA);
      break;
    case BLOCK_COLOR_O:
      SetConsoleTextAttribute(hConsole,
                              FOREGROUND_YELLOW | FOREGROUND_INTENSITY);
      break;
    case BLOCK_COLOR_WALL:
      SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
      break;
    case BLOCK_COLOR_GHOST:
      SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
      break;
    case BLOCK_COLOR_DEFAULT:
    default:
      SetConsoleTextAttribute(hConsole, original_attributes);
      break;
  }

#else
  // Unix-like systems
  const char* color_code = "\033[0m";
  switch (color_id) {
    case BLOCK_COLOR_I:
      color_code = "\033[96m";  // Bright Cyan
      break;
    case BLOCK_COLOR_L:
      color_code = "\033[33m";  // Dark Yellow (for Orange)
      break;
    case BLOCK_COLOR_J:
      color_code = "\033[94m";  // Bright Blue
      break;
    case BLOCK_COLOR_S:
      color_code = "\033[92m";  // Bright Green
      break;
    case BLOCK_COLOR_Z:
      color_code = "\033[91m";  // Bright Red
      break;
    case BLOCK_COLOR_T:
      color_code = "\033[95m";  // Bright Magenta
      break;
    case BLOCK_COLOR_O:
      color_code = "\033[93m";  // Bright Yellow
      break;
    case BLOCK_COLOR_WALL:
      color_code = "\033[37m";  // White
      break;
    case BLOCK_COLOR_GHOST:
      color_code = "\033[90m";  // Bright Black (Dark Gray)
      break;
    case BLOCK_COLOR_DEFAULT:
    default:
      color_code = "\033[0m";  // Reset
      break;
  }
  printf("%s", color_code);
#endif
}

/* 고스트 블록 렌더링 */
void render_print_ghost_segment(void) {
  if (current_render_mode == RENDER_MODE_BACKGROUND_COLOR) {
    internal_set_background_color(BLOCK_COLOR_GHOST);
  } else {
    internal_set_color(BLOCK_COLOR_GHOST);
  }
  switch (current_render_mode) {
    case RENDER_MODE_UNICODE_BLOCK_ELEMENTS:
      printf("▒ ");
      break;
    case RENDER_MODE_BACKGROUND_COLOR:
      printf("  ");
      break;
    case RENDER_MODE_EMOJI:
      printf("▒ ");
      break;
    case RENDER_MODE_PLATFORM_COLOR_CODES:
      printf("::");
      break;
    case RENDER_MODE_ASCII_ONLY:
      printf("..");
      break;
  }
  if (current_render_mode != RENDER_MODE_ASCII_ONLY) {
    render_reset_color();
  }
}

/* 블록 유형 또는 셀 값을 색상 ID로 매핑 */
static int map_block_or_cell_to_color_id(int value, bool is_cell_value) {
  int block_type_internal;
  if (is_cell_value) {
    if (value == 0) {
      return BLOCK_COLOR_DEFAULT;
    }
    if (value == 1) {
      return BLOCK_COLOR_WALL;
    }
    block_type_internal = value - 2;
  } else {
    block_type_internal = value;
  }

  if (block_type_internal == -1) {
    return BLOCK_COLOR_WALL;
  }

  switch (block_type_internal) {
    case I_BLOCK:
      return BLOCK_COLOR_I;
    case T_BLOCK:
      return BLOCK_COLOR_T;
    case S_BLOCK:
      return BLOCK_COLOR_S;
    case Z_BLOCK:
      return BLOCK_COLOR_Z;
    case L_BLOCK:
      return BLOCK_COLOR_L;
    case J_BLOCK:
      return BLOCK_COLOR_J;
    case O_BLOCK:
      return BLOCK_COLOR_O;
    default:
      return BLOCK_COLOR_DEFAULT;
  }
}

/* 셀 값 또는 블록 유형에 따라 색상 설정 */
void render_set_color_for_cell_value(int cell_value) {
  int color_id = map_block_or_cell_to_color_id(cell_value, true);
  if (current_render_mode == RENDER_MODE_BACKGROUND_COLOR) {
    internal_set_background_color(color_id);
  } else {
    internal_set_color(color_id);
  }
}

/* 블록 유형에 따라 색상 설정 */
void render_set_color_for_block_type(int block_type) {
  int color_id = map_block_or_cell_to_color_id(block_type, false);
  if (current_render_mode == RENDER_MODE_BACKGROUND_COLOR) {
    internal_set_background_color(color_id);
  } else {
    internal_set_color(color_id);
  }
}

/* 콘솔 색상 리셋 */
void render_reset_color(void) {
  internal_set_color(BLOCK_COLOR_DEFAULT);
}

/* 블록 유형에 따라 콘텐츠 렌더링 */
void render_print_segment_content(int block_type_for_mode_specific_render) {
  switch (current_render_mode) {
    case RENDER_MODE_UNICODE_BLOCK_ELEMENTS:
      printf("■ ");
      break;
    case RENDER_MODE_BACKGROUND_COLOR:
      printf("  ");
      break;
    case RENDER_MODE_EMOJI:
      if (block_type_for_mode_specific_render == -1) {
        printf("%s", get_emoji_for_wall());
      } else {
        printf("%s",
               get_emoji_for_block_type(block_type_for_mode_specific_render));
      }
      break;
    case RENDER_MODE_PLATFORM_COLOR_CODES:
      printf("##");
      break;
    case RENDER_MODE_ASCII_ONLY: {
      char c = block_type_to_char(block_type_for_mode_specific_render);
      printf("%c%c", c, c);
    } break;
  }
}

/* 블록 세그먼트 렌더링 */
void render_print_block_segment(int cell_value) {
  render_set_color_for_cell_value(cell_value);

  if (cell_value == 1) {  // Wall
    switch (current_render_mode) {
      case RENDER_MODE_UNICODE_BLOCK_ELEMENTS:
        printf("■ ");
        break;
      case RENDER_MODE_BACKGROUND_COLOR:
        printf("  ");
        break;
      case RENDER_MODE_EMOJI:
        printf("%s", get_emoji_for_wall());
        break;
      case RENDER_MODE_PLATFORM_COLOR_CODES:
        printf("##");
        break;
      case RENDER_MODE_ASCII_ONLY:
        printf("##");
        break;
    }
  } else {  // 실제 블록
    int block_type = cell_value - 2;
    render_print_segment_content(block_type);
  }
  if (current_render_mode == RENDER_MODE_UNICODE_BLOCK_ELEMENTS ||
      current_render_mode == RENDER_MODE_PLATFORM_COLOR_CODES ||
      current_render_mode == RENDER_MODE_BACKGROUND_COLOR) {
    render_reset_color();
  }
}

void render_print_preview_segment(int block_type) {
  render_set_color_for_block_type(block_type);
  render_print_segment_content(block_type);

  if (current_render_mode == RENDER_MODE_UNICODE_BLOCK_ELEMENTS ||
      current_render_mode == RENDER_MODE_PLATFORM_COLOR_CODES ||
      current_render_mode == RENDER_MODE_BACKGROUND_COLOR) {
    render_reset_color();
  }
}

void render_print_empty_segment(void) {
  printf("  ");
}
