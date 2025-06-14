#pragma once
#ifndef TYPES_H
#define TYPES_H

#include <time.h>
#include <stdint.h>
/* 결과 구조체 */
typedef struct result {
  char name[30];
  uint64_t point;
  time_t time;
  int rank;
} play_result;

#endif  // TYPES_H
