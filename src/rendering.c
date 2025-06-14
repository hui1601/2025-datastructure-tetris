#include "rendering.h"
#include <stdio.h>
#include <stdlib.h>

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
                       const char* block_char_example) {
  printf("\t\t    %s Example: ", mode_name);
  render_set_color_for_block_type(block_type_example);
  printf("%s", block_char_example);
  render_reset_color();
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

  current_render_mode = RENDER_MODE_PLATFORM_COLOR_CODES;
  printf("\t\t2. Standard Terminal Colors with '##'\n");
  print_sample_line("T-Block", T_BLOCK, "##");
  print_sample_line("Wall", -1, "##");
  printf("\t\t   (Uses ## characters. Good if ■ has issues.)\n\n");

  current_render_mode = RENDER_MODE_ASCII_ONLY;
  printf("\t\t3. ASCII Only (Most Compatible)\n");
  print_sample_line("L-Block", L_BLOCK, "LL");
  print_sample_line("Wall", -1, "##");
  printf(
      "\t\t   (Uses letter pairs like II, TT. No special characters or "
      "colors.)\n\n");

  current_render_mode = old_mode;

  printf("\t\t=========================================\n");
  printf("\t\tEnter your choice (1-3): ");

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
      current_render_mode = RENDER_MODE_PLATFORM_COLOR_CODES;
      break;
    case 3:
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

void internal_set_color(int color_id) {
  if (current_render_mode == RENDER_MODE_ASCII_ONLY &&
      color_id != BLOCK_COLOR_DEFAULT) {
    return;
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
    case BLOCK_COLOR_DEFAULT:  // fallthrough
    default:
      color_code = "\033[0m";  // Reset
      break;
  }
  printf("%s", color_code);
#endif
}

void render_set_color_for_cell_value(int cell_value) {
  if (cell_value == 0) {  // Empty
    internal_set_color(BLOCK_COLOR_DEFAULT);
  } else if (cell_value == 1) {  // Wall
    internal_set_color(BLOCK_COLOR_WALL);
  } else {  // Actual block (cell_value >= 2)
    int block_type = cell_value - 2;
    render_set_color_for_block_type(block_type);
  }
}

void render_set_color_for_block_type(int block_type) {
  if (block_type == -1 &&
      current_render_mode == RENDER_MODE_UNICODE_BLOCK_ELEMENTS) {
    internal_set_color(BLOCK_COLOR_WALL);
    return;
  }
  if (block_type == -1 && current_render_mode == RENDER_MODE_ASCII_ONLY) {
    return;
  }

  switch (block_type) {
    case I_BLOCK:
      internal_set_color(BLOCK_COLOR_I);
      break;
    case T_BLOCK:
      internal_set_color(BLOCK_COLOR_T);
      break;
    case S_BLOCK:
      internal_set_color(BLOCK_COLOR_S);
      break;
    case Z_BLOCK:
      internal_set_color(BLOCK_COLOR_Z);
      break;
    case L_BLOCK:
      internal_set_color(BLOCK_COLOR_L);
      break;
    case J_BLOCK:
      internal_set_color(BLOCK_COLOR_J);
      break;
    case O_BLOCK:
      internal_set_color(BLOCK_COLOR_O);
      break;
    default:
      internal_set_color(BLOCK_COLOR_DEFAULT);
      break;
  }
}

void render_reset_color(void) {
  internal_set_color(BLOCK_COLOR_DEFAULT);
}

void render_print_segment_content(int block_type_for_ascii) {
  switch (current_render_mode) {
    case RENDER_MODE_UNICODE_BLOCK_ELEMENTS:
      printf("■ ");
      break;
    case RENDER_MODE_PLATFORM_COLOR_CODES:
      printf("##");
      break;
    case RENDER_MODE_ASCII_ONLY: {
      char c = block_type_to_char(block_type_for_ascii);
      printf("%c%c", c, c);
    } break;
  }
}

void render_print_block_segment(int cell_value) {
  render_set_color_for_cell_value(cell_value);

  if (cell_value == 1) {  // Wall
    if (current_render_mode == RENDER_MODE_ASCII_ONLY) {
      printf("##");
    } else if (current_render_mode == RENDER_MODE_UNICODE_BLOCK_ELEMENTS) {
      printf("■ ");
    } else {  // RENDER_MODE_PLATFORM_COLOR_CODES
      printf("##");
    }
  } else {  // Actual block (cell_value >= 2)
    int block_type = cell_value - 2;
    render_print_segment_content(block_type);
  }
  if (current_render_mode != RENDER_MODE_ASCII_ONLY) {
    render_reset_color();
  }
}

void render_print_preview_segment(int block_type) {
  render_set_color_for_block_type(block_type);
  render_print_segment_content(block_type);
  if (current_render_mode != RENDER_MODE_ASCII_ONLY) {
    render_reset_color();
  }
}

void render_print_empty_segment(void) {
  printf("  ");
}
