#include "avl.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

int avl_get_node_height(avl_node* node) {
  if (node == NULL) {
    return 0;
  }

  return node->height;
}

void avl_write_nodes(avl_node* node, FILE* file) {
  if (node == NULL) {
    return;
  }

  avl_write_nodes(node->left, file);
  const char* temp_name = node->data.name;
  uint64_t temp_time = (uint64_t)node->data.time;
  fwrite(temp_name, sizeof(char), strlen(temp_name) + 1, file);
  fwrite(&node->data.point, sizeof(uint64_t), 1, file);
  fwrite(&temp_time, sizeof(uint64_t), 1, file);
  avl_write_nodes(node->right, file);
}

void avl_free_nodes(avl_node* node) {
  if (node == NULL) {
    return;
  }

  avl_free_nodes(node->left);
  avl_free_nodes(node->right);
  free(node);
}

avl_tree* avl_create_tree(void) {
  avl_tree* tree = (avl_tree*)malloc(sizeof(avl_tree));
  if (tree == NULL) {
    fprintf(stderr, "Failed to allocate memory for AVL tree\n");
    exit(EXIT_FAILURE);
  }

  tree->root = NULL;
  return tree;
}

/* AVL 트리를 파괴(?)하는 함수 */
void avl_destroy_tree(avl_tree* tree) {
  if (tree == NULL) {
    return;
  }

  avl_free_nodes(tree->root);
  free(tree);
}

/* AVL 트리의 노드를 생성하는 함수 */
avl_node* avl_create_node(play_result data) {
  avl_node* node = (avl_node*)malloc(sizeof(avl_node));
  if (node == NULL) {
    fprintf(stderr, "Failed to allocate memory for AVL node\n");
    exit(EXIT_FAILURE);
  }

  node->data = data;
  node->left = NULL;
  node->right = NULL;
  node->height = 1;
  return node;
}

/* AVL 트리에 데이터를 삽입하는 함수 */
avl_node* avl_insert_data(avl_tree* tree, avl_node* node, play_result data) {
  if (node == NULL) {
    return avl_create_node(data);
  }

  if (data.point < node->data.point) {
    node->left = avl_insert_data(tree, node->left, data);
  } else if (data.point > node->data.point) {
    node->right = avl_insert_data(tree, node->right, data);
  } else if (data.time < node->data.time) {
    node->left = avl_insert_data(tree, node->left, data);
  } else if (data.time > node->data.time) {
    node->right = avl_insert_data(tree, node->right, data);
  }
  // If points and time are the same, don't insert duplicates
  else {
    return node;
  }
  node->height = 1 + MAX(avl_get_node_height(node->left),
                         avl_get_node_height(node->right));
  int balance =
      avl_get_node_height(node->left) - avl_get_node_height(node->right);

  // Left Left Case
  if (balance > 1 && (data.point < node->left->data.point ||
                      (data.point == node->left->data.point &&
                       data.time < node->left->data.time))) {
    return avl_rotate_right(node);
  }

  // Right Right Case
  if (balance < -1 && (data.point > node->right->data.point ||
                       (data.point == node->right->data.point &&
                        data.time > node->right->data.time))) {
    return avl_rotate_left(node);
  }

  // Left Right Case
  if (balance > 1 && (data.point > node->left->data.point ||
                      (data.point == node->left->data.point &&
                       data.time > node->left->data.time))) {
    node->left = avl_rotate_left(node->left);
    return avl_rotate_right(node);
  }

  // Right Left Case
  if (balance < -1 && (data.point < node->right->data.point ||
                       (data.point == node->right->data.point &&
                        data.time < node->right->data.time))) {
    node->right = avl_rotate_right(node->right);
    return avl_rotate_left(node);
  }
  return node;
}

/* AVL 트리에서 최소값을 찾는 함수 */
avl_node* avl_find_min(avl_node* node) {
  if (node == NULL || node->left == NULL) {
    return node;
  }
  return avl_find_min(node->left);
}

/* AVL 트리에서 최대값을 찾는 함수 */
avl_node* avl_find_max(avl_node* node) {
  if (node == NULL || node->right == NULL) {
    return node;
  }
  return avl_find_max(node->right);
}

/* AVL 트리의 데이터를 파일에 저장 */
void avl_save(avl_tree* tree) {
  FILE* file = fopen(AVL_TREE_TEMP_NAME, "wb");
  if (file == NULL) {
    fprintf(stderr, "Failed to open file for saving AVL tree\n");
    return;
  }

  avl_write_nodes(tree->root, file);
  if (ferror(file)) {
    fprintf(stderr, "Error writing to file\n");
  }

  if (fclose(file) != 0) {
    fprintf(stderr, "Failed to close file after saving AVL tree\n");
  }

  remove(AVL_TREE_NAME);
  if (rename(AVL_TREE_TEMP_NAME, AVL_TREE_NAME) != 0) {
    fprintf(stderr, "Failed to rename temporary file to AVL tree file\n");
  }
}

/* AVL 트리에서 데이터를 불러오기 */
avl_tree* avl_load(void) {
  avl_tree* tree = avl_create_tree();
  FILE* file = fopen(AVL_TREE_NAME, "rb");
  if (file == NULL) {
    fprintf(stderr, "Failed to open file for loading AVL tree\n");
    return tree;
  }

  play_result data;
  uint64_t temp_time, index = 0;
  char temp_name[30] = {0};
  while (true) {
    index = 0;
    while (fread(temp_name + index, sizeof(char), 1, file) == 1 &&
           temp_name[index] != '\0') {
      index++;
    }

    if (feof(file)) {
      break;
    }

    strncpy(data.name, temp_name, sizeof(data.name) - 1);
    fread(&data.point, sizeof(uint64_t), 1, file);
    fread(&temp_time, sizeof(uint64_t), 1, file);
    data.time = temp_time;
    tree->root = avl_insert_data(tree, tree->root, data);
  }
  fclose(file);
  return tree;
}

void avl_print_node(avl_node* node, uint64_t rank) {
  printf("\t\t");
  if (node == NULL) {
    printf("%-8s %-20s %-10s %-25s\n", "-", "NULL NODE", "-", "N/A");
    return;
  }
  char time_display_str[30];
  time_t temp_time = node->data.time;
  if (temp_time == 0) {
    strcpy(time_display_str, "N/A");
  } else {
    char* ctime_result = ctime(&temp_time);
    if (ctime_result == NULL) {
      strcpy(time_display_str, "Time Error");
    } else {
      strncpy(time_display_str, ctime_result, sizeof(time_display_str) - 1);
      time_display_str[sizeof(time_display_str) - 1] = '\0';
      time_display_str[strcspn(time_display_str, "\n")] = '\0';
    }
  }
  char name_buffer[31];
  strncpy(name_buffer, node->data.name, sizeof(name_buffer) - 1);
  name_buffer[sizeof(name_buffer) - 1] = '\0';
  printf("%-8" PRIu64 " %-20.20s %-10" PRIu64 " %-25.25s\n", rank, name_buffer,
         node->data.point, time_display_str);
}

/* 스코어 큰 순으로 AVL 트리의 데이터를 출력 */
uint64_t avl_print_data(avl_node* node, uint64_t* rank_val_ptr) {
  if (node == NULL) {
    return 0;
  }
  uint64_t count_right = avl_print_data(node->right, rank_val_ptr);
  avl_print_node(node, *rank_val_ptr);
  (*rank_val_ptr)++;
  uint64_t count_left = avl_print_data(node->left, rank_val_ptr);
  return count_right + count_left + 1;
}

/* AVL 트리에서 범위 안에 있는 점수의 노드를 찾아 출력 */
bool avl_print_score_range(avl_node* node,
                           uint64_t min,
                           uint64_t max,
                           uint64_t* rank_val_ptr) {
  if (node == NULL) {
    return false;
  }

  bool found = false;
  if (avl_print_score_range(node->right, min, max, rank_val_ptr)) {
    found = true;
  }

  if (node->data.point >= min && node->data.point <= max) {
    avl_print_node(node, *rank_val_ptr);
    found = true;
  }

  (*rank_val_ptr)++;
  if (avl_print_score_range(node->left, min, max, rank_val_ptr)) {
    found = true;
  }

  return found;
}

/* AVL 트리에서 특정 점수의 노드를 찾아 출력 */
bool avl_print_by_score(avl_node* node,
                        uint64_t score,
                        uint64_t* rank_val_ptr) {
  return avl_print_score_range(node, score, score, rank_val_ptr);
}

/* AVL 트리에서 이름으로 노드를 찾아 출력 */
bool avl_print_by_name(avl_node* node,
                       const char* name,
                       uint64_t* rank_val_ptr) {
  if (node == NULL) {
    return false;
  }

  bool found = false;
  if (avl_print_by_name(node->right, name, rank_val_ptr)) {
    found = true;
  }

  if (strcmp(node->data.name, name) == 0) {
    avl_print_node(node, *rank_val_ptr);
    found = true;
  }

  (*rank_val_ptr)++;
  if (avl_print_by_name(node->left, name, rank_val_ptr)) {
    found = true;
  }

  return found;
}

avl_node* avl_rotate_left(avl_node* z) {
  avl_node* y = z->right;
  z->right = y->left;
  y->left = z;
  z->height =
      1 + MAX(avl_get_node_height(z->left), avl_get_node_height(z->right));
  y->height =
      1 + MAX(avl_get_node_height(y->left), avl_get_node_height(y->right));
  return y;
}

avl_node* avl_rotate_right(avl_node* z) {
  avl_node* y = z->left;
  z->left = y->right;
  y->right = z;
  z->height =
      1 + MAX(avl_get_node_height(z->left), avl_get_node_height(z->right));
  y->height =
      1 + MAX(avl_get_node_height(y->left), avl_get_node_height(y->right));
  return y;
}
