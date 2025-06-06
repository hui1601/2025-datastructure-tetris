#ifndef PLATFORM_WINDOWS_H
#define PLATFORM_WINDOWS_H

#include <conio.h>
#include <windows.h>

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
  new_mode &=
      ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
  SetConsoleMode(hIn, new_mode);
}

void close_keyboard(void) {
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
  SetConsoleMode(hIn, __old_console_mode);
}

// Wrappers for kbhit() and getch() using Windows API
int kbhit(void) {
  return _kbhit();
}
int getch(void) {
  return _getch();
}
#endif  // PLATFORM_WINDOWS_H