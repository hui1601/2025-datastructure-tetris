#include <tetris.h>

const char i_block[4][4][4] = {
    {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}},
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}},
    {{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}}};

const char t_block[4][4][4] = {
    {{1, 0, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}};

const char s_block[4][4][4] = {
    {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}}};

const char z_block[4][4][4] = {
    {{0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}};

const char l_block[4][4][4] = {
    {{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}};

const char j_block[4][4][4] = {
    {{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}};

const char o_block[4][4][4] = {
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

char tetris_table[21][10];

play_result temp_result;

int block_number = 0;
int next_block_number = 0;
int hold_block_number = -1;
int block_state = 0;
char (*block_pointer)[4][4];
int x = 3, y = 0;
int game = GAME_END;
uint64_t point = 0;
avl_tree* result_tree = NULL;

/* 테트리스 테이블 초기화 */
void init_tetris_table(void) {
  int i, j;

  for (i = 0; i < 20; i++) {
    for (j = 0; j < 10; j++) {
      if (j == 0 || j == 9) {
        tetris_table[i][j] = 1;  // 좌우 벽
      } else {
        tetris_table[i][j] = 0;  // 빈 공간
      }
    }
  }

  for (j = 0; j < 10; j++) {
    tetris_table[20][j] = 1;  // 바닥
  }
}

/* 블록 포인터 설정 */
const char (*set_block(int block_num))[4][4] {
  switch (block_num) {
    case I_BLOCK:
      return i_block;
    case T_BLOCK:
      return t_block;
    case S_BLOCK:
      return s_block;
    case Z_BLOCK:
      return z_block;
    case L_BLOCK:
      return l_block;
    case J_BLOCK:
      return j_block;
    case O_BLOCK:
      return o_block;
  }
  return i_block;
}

/* 충돌 검사 */
bool check_collision(int cx, int cy, int rotation) {
  int i, j;
  const char (*block)[4][4] = set_block(block_number);

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (block[rotation][i][j] != 0) {
        int nx = cx + j;
        int ny = cy + i;

        if (nx < 0 || nx >= 10 || ny < 0 || ny >= 21) {
          return true;
        }

        if (tetris_table[ny][nx] != 0) {
          return true;
        }
      }
    }
  }
  return false;
}

/* 블록을 테이블에 추가 */
void add_block_to_table(void) {
  int i, j;
  const char (*block)[4][4] = set_block(block_number);

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (block[block_state][i][j] != 0) {
        tetris_table[y + i][x + j] = block_number + 2;
      }
    }
  }
}

/* 라인 제거 */
int clear_lines(void) {
  int i, j, k;
  int lines_cleared = 0;

  for (i = 19; i >= 0; i--) {
    int full = 1;
    for (j = 1; j < 9; j++) {
      if (tetris_table[i][j] == 0) {
        full = 0;
        break;
      }
    }

    if (full) {
      // 라인 제거
      for (k = i; k > 0; k--) {
        for (j = 1; j < 9; j++) {
          tetris_table[k][j] = tetris_table[k - 1][j];
        }
      }
      // 맨 위 라인 초기화
      for (j = 1; j < 9; j++) {
        tetris_table[0][j] = 0;
      }

      lines_cleared++;
      i++;  // 같은 라인 다시 검사
    }
  }

  // 점수 계산
  if (lines_cleared > 0) {
    point += lines_cleared * 100;
  }

  return lines_cleared;
}

/* 블록 이동 */
void move_block(int direction) {
  int new_x = x;
  int new_y = y;

  switch (direction) {
    case LEFT:
      new_x--;
      break;
    case RIGHT:
      new_x++;
      break;
    case DOWN:
      new_y++;
      break;
  }

  if (!check_collision(new_x, new_y, block_state)) {
    x = new_x;
    y = new_y;
  } else if (direction == DOWN) {
    // 아래로 이동 중 충돌하면 블록 고정
    add_block_to_table();
    clear_lines();

    block_number = next_block_number;
    next_block_number = rand() % 7;
    block_state = 0;
    x = 3;
    y = 0;

    if (check_collision(x, y, block_state)) {
      game = GAME_END;
    }
  }
}

/* 블록 회전 */
void rotate_block(void) {
  int new_state = (block_state + 1) % 4;

  if (!check_collision(x, y, new_state)) {
    block_state = new_state;
  }
}

/* 블록 한번에 떨어뜨리기 */
void drop_block(void) {
  while (!check_collision(x, y + 1, block_state)) {
    y++;
  }
  move_block(DOWN);
}

/* 블록 Hold */
void hold_block(void) {
  if (hold_block_number < 0) {
    hold_block_number = block_number;
    block_number = next_block_number;
    next_block_number = rand() % 7;
    block_state = 0;
    x = 3;
    y = 0;

    if (check_collision(x, y, block_state)) {
      game = GAME_END;
    }
  } else {
    int temp = block_number;
    block_number = hold_block_number;
    hold_block_number = temp;
  }
}

/* 테트리스 테이블 표시 */
void display_tetris_table(void) {
  int i, j;
  const char (*block)[4][4] = set_block(block_number);
  const char (*next)[4][4] = set_block(next_block_number);
  const char (*hold)[4][4] = NULL;
  if (hold_block_number >= 0) {
    hold = set_block(hold_block_number);
  }

  clear_screen();

  // 점수 표시
  printf("\n\t[ SCORE: %" PRIu64 " ]\t\t[ NEXT BLOCK ]\n", point);
  fflush(stdout);
  // 테이블 그리기
  for (i = 0; i < 21; i++) {
    printf("\t");
    for (j = 0; j < 10; j++) {
      if (tetris_table[i][j] != 0) {
        render_print_block_segment(tetris_table[i][j]);
      } else {
        // 현재 블록 표시
        int in_block = 0;
        int bi, bj;
        for (bi = 0; bi < 4; bi++) {
          for (bj = 0; bj < 4; bj++) {
            if (block[block_state][bi][bj] != 0 && y + bi == i && x + bj == j) {
              in_block = 1;
              break;
            }
          }
          if (in_block) {
            break;
          }
        }

        if (in_block) {
          render_print_preview_segment(block_number);
        } else {
          render_print_empty_segment();
        }
      }
    }
    if (i < 4) {
      // 다음 블록 표시
      printf("\t    ");
      for (j = 0; j < 4; j++) {
        if (next[0][i % 4][j] != 0) {
          render_print_preview_segment(next_block_number);
        } else {
          render_print_empty_segment();
        }
      }
    } else if (i == 4) {
      printf("    [ HOLD BLOCK ]");
    } else if (i < 8) {
      // 보류된 블록 표시
      printf("\t    ");
      if (hold != NULL) {
        for (j = 0; j < 4; j++) {
          if (hold[0][(i - 5) % 4][j] != 0) {
            render_print_preview_segment(hold_block_number);
          } else {
            render_print_empty_segment();
          }
        }
      } else {
        printf("    ");
      }
    }
    printf("\n");
  }

  printf(
      "\n\t[ j: LEFT | l: RIGHT | k: DOWN | i: ROTATE | a: DROP | h: HOLD | p: "
      "QUIT ]\n");
  fflush(stdout);
}

/* 게임 시작 */
int game_start(void) {
  int ch;
  time_t start_time, current_time;

  init_keyboard();
  init_tetris_table();
  srand(time(NULL));
  block_number = rand() % 7;
  next_block_number = rand() % 7;
  block_state = 0;
  x = 3;
  y = 0;
  point = 0;

  time(&start_time);

  while (game == GAME_START) {
    display_tetris_table();

    time(&current_time);
    if (difftime(current_time, start_time) >= 1.0) {
      move_block(DOWN);
      time(&start_time);
    }

    if (kbhit()) {
      ch = tolower(getch());
      switch (ch) {
        case 'j':
          move_block(LEFT);
          break;
        case 'l':
          move_block(RIGHT);
          break;
        case 'k':
          move_block(DOWN);
          break;
        case 'i':
          rotate_block();
          break;
        case 'a':
          drop_block();
          break;
        case 'h':
          hold_block();
          break;
        case 'p':
          game = GAME_END;
          break;
      }
    }

    platform_usleep(50000);
  }

  close_keyboard();

  clear_screen();
  printf("\n\n\t\tGAME OVER!\n");
  printf("\t\tFinal Score: %" PRIu64 "\n", point);
  printf("\n\t\tEnter your name: ");

  scanf("%s", temp_result.name);
  CLEAR_INPUT_BUFFER();

  temp_result.point = point;
  time_t t = time(NULL);
  temp_result.time = t;
  result_tree->root =
      avl_insert_data(result_tree, result_tree->root, temp_result);

  avl_save(result_tree);

  printf("\n\t\tPress any key to continue...");
  getchar();

  return 1;
}

/* 결과 검색 */
void search_result(void) {
  char name[30];
  int found = 0;

  clear_screen();
  printf("\n\n\t\t[ SEARCH RESULT ]\n");
  printf("\t\t============================\n");
  printf("\n\t\tWhich method do you want to search?\n");
  printf("\t\t\t   1) Search by name\n");
  printf("\t\t\t   2) Search by score\n");
  printf("\t\t\t   3) Search by score range\n");
  printf("\t\t============================\n");
  printf("\n\t\tEnter your choice: ");
  int choice;
  uint64_t score;
  scanf("%d", &choice);

  switch (choice) {
    case 1:
      printf("\n\t\tEnter name to search: ");
      scanf("%s", name);
      CLEAR_INPUT_BUFFER();
      found = avl_print_by_name(result_tree->root, name);
      if (!found) {
        printf("\n\t\tNo records found for name: %s\n", name);
      }
      break;
    case 2:
      printf("\n\t\tEnter score to search: ");
      scanf("%" PRIu64, &score);
      CLEAR_INPUT_BUFFER();
      found = avl_print_by_score(result_tree->root, score);
      if (!found) {
        printf("\n\t\tNo records found for score: %" PRIu64 "\n", score);
      }
      break;
    case 3:
      printf("\n\t\tEnter score range to search (min max): ");
      uint64_t min, max;
      scanf("%" PRIu64 " %" PRIu64, &min, &max);
      CLEAR_INPUT_BUFFER();
      found = avl_print_score_range(result_tree->root, min, max);
      if (!found) {
        printf("\n\t\tNo records found for score range: %" PRIu64 " - %" PRIu64
               "\n",
               min, max);
      }
      break;
    default:
      printf("\n\t\tInvalid choice!\n");
      printf("\n\t\tPress any key to continue...");
      getchar();
      return;
  }

  printf("\n\t\tPress any key to continue...");
  getchar();
}

/* 전체 결과 출력 */
void print_result(void) {
  clear_screen();
  uint64_t count = 0;
  printf("\n\t\t%-20s %-10s %-20s\n", "NAME", "SCORE", "DATE");
  printf("\t\t================================================\n");

  avl_node* node = result_tree->root;
  if (node == NULL) {
    printf("\n\t\tNo records found!\n");
    printf("\n\t\tPress any key to continue...");
    getchar();
    return;
  } else {
    count = avl_print_data(node);
  }
  printf("\n\t\tTotal records: %" PRIu64 "\n", count);
  printf("\n\t\tPress any key to continue...");
  getchar();
}

/* 메뉴 표시 */
int display_menu(void) {
  int menu = 0;

  while (1) {
    clear_screen();
    printf("\n\n\t\t\t\tText Tetris");
    printf("\n\t\t\t============================");
    printf("\n\t\t\t\tGAME MENU\t\n");
    printf("\n\t\t\t============================");
    printf("\n\t\t\t   1) Game Start");
    printf("\n\t\t\t   2) Search history");
    printf("\n\t\t\t   3) Record Output");
    printf("\n\t\t\t   4) QUIT");
    printf("\n\t\t\t============================");
    printf("\n\t\t\t\t\t SELECT : ");
    scanf("%d", &menu);
    CLEAR_INPUT_BUFFER();
    if (menu < 1 || menu > 4) {
      continue;
    } else {
      return menu;
    }
  }
  return 0;
}

/* 메인 함수 */
int main(void) {
  init_platform();
  init_rendering_system();
  int menu = 1;
  result_tree = avl_load();
  if (result_tree == NULL) {
    result_tree = avl_create_tree();
  }
  while (menu) {
    menu = display_menu();

    if (menu == 1) {
      game = GAME_START;
      menu = game_start();
    } else if (menu == 2) {
      search_result();
    } else if (menu == 3) {
      print_result();
    } else if (menu == 4) {
      break;
    }
  }
  avl_save(result_tree);
  return 0;
}
