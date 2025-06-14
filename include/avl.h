#pragma once
#ifndef AVL_H
#define AVL_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <types.h>
#define AVL_TREE_NAME "play_result.dat"
#define AVL_TREE_TEMP_NAME "play_result_temp.dat"
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif  // MAX
typedef struct avl_node {
  play_result data;
  struct avl_node* left;
  struct avl_node* right;
  int64_t height;
} avl_node;

typedef struct avl_tree {
  avl_node* root;
} avl_tree;

avl_tree* avl_create_tree(void);
void avl_destroy_tree(avl_tree* tree);
avl_node* avl_create_node(play_result data);
avl_node* avl_insert_data(avl_tree* tree, avl_node* node, play_result data);
// Might not be needed in this context(We don't have a delete operation)
// avl_node* delete_avl_node(avl_tree* tree, avl_node* node, play_result data);
avl_node* avl_find_min(avl_node* node);
avl_node* avl_find_max(avl_node* node);
void avl_save(avl_tree* tree);
avl_tree* avl_load(void);
uint64_t avl_print_data(avl_node* node);
void avl_print_node(avl_node* node);
bool avl_print_score_range(avl_node* node, uint64_t min, uint64_t max);
bool avl_print_by_score(avl_node* node, uint64_t score);
bool avl_print_by_name(avl_node* node, const char* name);
avl_node* avl_rotate_left(avl_node* z);
avl_node* avl_rotate_right(avl_node* z);
#endif  // AVL_H
