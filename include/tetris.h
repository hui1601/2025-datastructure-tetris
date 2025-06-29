#pragma once
#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

/* 왼쪽, 오른쪽, 아래, 회전  */
#define LEFT 0
#define RIGHT 1
#define DOWN 2
#define ROTATE 3

/* 블록 모양 */
#define I_BLOCK 0
#define T_BLOCK 1
#define S_BLOCK 2
#define Z_BLOCK 3
#define L_BLOCK 4
#define J_BLOCK 5
#define O_BLOCK 6

/* 게임 시작, 게임 종료 */
#define GAME_START 0
#define GAME_END 1

/* 테트리스 테이블 크기 */
#define TABLE_X 20
#define TABLE_Y 10

/* 유틸리티 매크로 */
#define CLEAR_INPUT_BUFFER()  \
  while (getchar() != '\n') { \
  }

/* 함수 선언 */
int display_menu(void);
void init_tetris_table(void);
int game_start(void);
void display_tetris_table(void);
const char (*set_block(int block_num))[4][4];
bool check_collision(int x, int y, int rotation);
void drop_block(void);
void move_block(int direction);
void rotate_block(void);
void hold_block(void);
int clear_lines(void);
void clear_screen(void);
void add_block_to_table(void);
void search_result(void);
void print_result(void);

/* 플랫폼에 따라 존재하는 함수들 */
#ifndef getch
int getch(void);
#endif  // getch

#ifndef kbhit
int kbhit(void);
#endif  // kbhit

/* 플랫폼 Wrapper 함수들 */
void init_platform(void);
void init_keyboard(void);
void close_keyboard(void);
void platform_usleep(unsigned int microseconds);

#endif  // TETRIS_H
