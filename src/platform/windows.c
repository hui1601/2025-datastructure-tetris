#include "tetris.h"

#include <conio.h>
#include <windows.h>

#include <locale.h>
#include <stdio.h>

static DWORD __old_console_mode = 0;

void clear_screen(void) {
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE)
    return;

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD cells, written;
  COORD home = {0, 0};

  if (!GetConsoleScreenBufferInfo(hOut, &csbi))
    return;
  cells = (DWORD)csbi.dwSize.X * (DWORD)csbi.dwSize.Y;

  FillConsoleOutputCharacterA(hOut, ' ', cells, home, &written);
  FillConsoleOutputAttribute(hOut, csbi.wAttributes, cells, home, &written);
  SetConsoleCursorPosition(hOut, home);
}

void init_keyboard(void) {
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
  GetConsoleMode(hIn, &__old_console_mode);

  DWORD new_mode = __old_console_mode;
  new_mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
  SetConsoleMode(hIn, new_mode);
}

void close_keyboard(void) {
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
  SetConsoleMode(hIn, __old_console_mode);
}

inline void platform_usleep(unsigned int microseconds) {
  Sleep(microseconds / 1000);
}

void init_platform(void) {
  if (!SetConsoleOutputCP(CP_UTF8)) {
    fprintf(stderr,
            "Warning: SetConsoleOutputCP(CP_UTF8) failed. Output may not be "
            "UTF-8.\n");
    return;
  }

  if (!SetConsoleCP(CP_UTF8)) {
    fprintf(stderr,
            "Warning: SetConsoleCP(CP_UTF8) failed. Input may not be UTF-8.\n");
    return;
  }

  if (setlocale(LC_ALL, ".UTF8") == NULL) {
    fprintf(stderr,
            "Error: Cannot set C library locale to .UTF8. Is the UCRT "
            "installed and up to date?\n");
  }
}

// Wrappers for kbhit() and getch() using Windows API
int kbhit(void) {
  return _kbhit();
}

int getch(void) {
  return _getch();
}