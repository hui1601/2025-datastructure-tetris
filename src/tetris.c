#include <tetris.h>
#if defined(_WIN32)
// For Windows
#include <platform/windows.h>
#elif defined(__unix__) || defined(__APPLE__)
// For POSIX compliant systems
#include <platform/unix.h>
#else
#error "Oops! Unsupported platform!"
#endif

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
char (*set_block(int block_num))[4][4] {
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
  char (*block)[4][4] = set_block(block_number);

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (block[rotation][i][j] == 1) {
        int nx = cx + j;
        int ny = cy + i;

        if (nx < 0 || nx >= 10 || ny < 0 || ny >= 21) {
          return true;
        }

        if (tetris_table[ny][nx] == 1) {
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
  char (*block)[4][4] = set_block(block_number);

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (block[block_state][i][j] == 1) {
        tetris_table[y + i][x + j] = 1;
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

/* 테트리스 테이블 표시 */
void display_tetris_table(void) {
  int i, j;
  char (*block)[4][4] = set_block(block_number);

  clear_screen();

  // 다음 블록 표시
  printf("\n\t\t[ NEXT BLOCK ]\n");
  char (*next)[4][4] = set_block(next_block_number);
  for (i = 0; i < 4; i++) {
    printf("\t\t");
    for (j = 0; j < 4; j++) {
      if (next[0][i][j] == 1) {
        printf("＠");
      } else {
        printf("  ");
      }
    }
    printf("\n");
  }

  // 점수 표시
  printf("\n\t\t[ SCORE: %ld ]\n\n", point);

  // 테이블 그리기
  for (i = 0; i < 21; i++) {
    printf("\t");
    for (j = 0; j < 10; j++) {
      if (tetris_table[i][j] == 1) {
        printf("＃");
      } else {
        // 현재 블록 표시
        int in_block = 0;
        int bi, bj;
        for (bi = 0; bi < 4; bi++) {
          for (bj = 0; bj < 4; bj++) {
            if (block[block_state][bi][bj] == 1 && y + bi == i && x + bj == j) {
              in_block = 1;
              break;
            }
          }
          if (in_block)
            break;
        }

        if (in_block) {
          printf("＠");
        } else {
          printf("  ");
        }
      }
    }
    printf("\n");
  }

  printf(
      "\n\t[ j: LEFT | l: RIGHT | k: DOWN | i: ROTATE | a: DROP | p: QUIT ]\n");
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
      ch = getch();
      switch (ch) {
        case 'j':
        case 'J':
          move_block(LEFT);
          break;
        case 'l':
        case 'L':
          move_block(RIGHT);
          break;
        case 'k':
        case 'K':
          move_block(DOWN);
          break;
        case 'i':
        case 'I':
          rotate_block();
          break;
        case 'a':
        case 'A':
          drop_block();
          break;
        case 'p':
        case 'P':
          game = GAME_END;
          break;
      }
    }

    usleep(50000);
  }

  close_keyboard();

  printf("\n\n\t\tGAME OVER!\n");
  printf("\t\tFinal Score: %ld\n", point);
  printf("\n\t\tEnter your name: ");

  scanf("%s", temp_result.name);

  temp_result.point = point;
  time_t t = time(NULL);
  struct tm* tm_info = localtime(&t);
  temp_result.year = tm_info->tm_year + 1900;
  temp_result.month = tm_info->tm_mon + 1;
  temp_result.day = tm_info->tm_mday;
  temp_result.hour = tm_info->tm_hour;
  temp_result.min = tm_info->tm_min;

  FILE* fp = fopen("result.txt", "a");
  if (fp != NULL) {
    fprintf(fp, "%s %ld %d %d %d %d %d\n", temp_result.name, temp_result.point,
            temp_result.year, temp_result.month, temp_result.day,
            temp_result.hour, temp_result.min);
    fclose(fp);
  }

  printf("\n\t\tPress any key to continue...");
  getchar();
  getchar();

  return 1;
}

/* 결과 검색 */
void search_result(void) {
  char name[30];
  FILE* fp;
  struct result r;
  int found = 0;

  clear_screen();
  printf("\n\n\t\t[ SEARCH RESULT ]\n");
  printf("\n\t\tEnter name to search: ");
  scanf("%s", name);

  fp = fopen("result.txt", "r");
  if (fp == NULL) {
    printf("\n\t\tNo records found!\n");
    printf("\n\t\tPress any key to continue...");
    getchar();
    getchar();
    return;
  }

  printf("\n\t\t%-20s %-10s %-20s\n", "NAME", "SCORE", "DATE");
  printf("\t\t================================================\n");

  while (fscanf(fp, "%s %ld %d %d %d %d %d", r.name, &r.point, &r.year,
                &r.month, &r.day, &r.hour, &r.min) == 7) {
    if (strcmp(r.name, name) == 0) {
      printf("\t\t%-20s %-10ld %04d-%02d-%02d %02d:%02d\n", r.name, r.point,
             r.year, r.month, r.day, r.hour, r.min);
      found = 1;
    }
  }

  fclose(fp);

  if (!found) {
    printf("\n\t\tNo records found for '%s'!\n", name);
  }

  printf("\n\t\tPress any key to continue...");
  getchar();
  getchar();
}

/* 전체 결과 출력 */
void print_result(void) {
  FILE* fp;
  struct result r;
  int count = 0;

  clear_screen();
  printf("\n\n\t\t[ ALL RECORDS ]\n");

  fp = fopen("result.txt", "r");
  if (fp == NULL) {
    printf("\n\t\tNo records found!\n");
    printf("\n\t\tPress any key to continue...");
    getchar();
    getchar();
    return;
  }

  printf("\n\t\t%-20s %-10s %-20s\n", "NAME", "SCORE", "DATE");
  printf("\t\t================================================\n");

  while (fscanf(fp, "%s %ld %d %d %d %d %d", r.name, &r.point, &r.year,
                &r.month, &r.day, &r.hour, &r.min) == 7) {
    printf("\t\t%-20s %-10ld %04d-%02d-%02d %02d:%02d\n", r.name, r.point,
           r.year, r.month, r.day, r.hour, r.min);
    count++;
  }

  fclose(fp);

  printf("\n\t\tTotal records: %d\n", count);
  printf("\n\t\tPress any key to continue...");
  getchar();
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
  int menu = 1;

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
      exit(0);
    }
  }

  return 0;
}
