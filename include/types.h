#pragma once
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <time.h>

/* 결과 구조체 */
typedef struct result {
  char name[30];
  uint64_t point;
  time_t time;
} play_result;

#endif  // TYPES_H
