#pragma once
#ifndef PLATFORM_UNIX_H
#define PLATFORM_UNIX_H
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

struct termios initial_settings, new_settings;

void clear_screen(void) {
  printf("\033[H\033[J");
}

void init_keyboard(void) {
  tcgetattr(STDIN_FILENO, &initial_settings);
  new_settings = initial_settings;
  new_settings.c_lflag &= ~(ICANON | ECHO);
  new_settings.c_cc[VMIN] = 1;
  new_settings.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}

void close_keyboard(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &initial_settings);
}

int kbhit(void) {
  int ch;
  int oldf;

  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }
  clearerr(stdin);
  return 0;
}

int getch(void) {
  int ch;
  while ((ch = getchar()) == EOF)
    ;
  return ch;
}
// check if the os already has usleep defined
#ifndef usleep
#include <errno.h>
#include <sys/time.h>
inline void usleep(unsigned int microseconds) {
  struct timeval tv;
  tv.tv_sec = microseconds / 1000000;
  tv.tv_usec = microseconds % 1000000;

  int ret;
  do {
    ret = select(0, NULL, NULL, NULL, &tv);
  } while (ret == -1 && errno == EINTR);
}
#endif  // usleep

void init_platform(void) {
  // Enable UTF-8 support in terminal(Xterm Control Sequences)
  printf("\x1b%%G");
  if (setlocale(LC_ALL, "en_US.UTF-8") == NULL) {
    fprintf(stderr, "Cannot set locale to en_US.UTF-8. Is it installed?\n");
  }
}

#endif  // PLATFORM_UNIX_H
