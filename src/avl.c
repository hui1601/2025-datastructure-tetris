#include "avl.h"
static int avl_get_node_height(avl_node* node) {
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

void avl_destroy_tree(avl_tree* tree) {
  if (tree == NULL)
    return;

  avl_free_nodes(tree->root);
  free(tree);
}

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

avl_node* avl_find_min(avl_node* node) {
  if (node == NULL || node->left == NULL) {
    return node;
  }
  return avl_find_min(node->left);
}

avl_node* avl_find_max(avl_node* node) {
  if (node == NULL || node->right == NULL) {
    return node;
  }
  return avl_find_max(node->right);
}

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
    // Read the name one by one byte while appearing to read or null-terminate
    // it
    while (fread(temp_name + index, sizeof(char), 1, file) == 1 &&
           temp_name[index] != '\0') {
      index++;
    }
    if (feof(file)) {
      break;  // End of file reached
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

void avl_print_node(avl_node* node) {
  printf("\t\t");
  if (node == NULL) {
    printf("NULL\n");
    return;
  }
  time_t temp_time = node->data.time;
  if (temp_time == 0) {
    printf("Name: %s, Point: %" PRIu64 ", Time: N/A, Height: %" PRIu64 "\n",
           node->data.name, node->data.point, node->height);
    return;
  }
  char* time_str = ctime(&temp_time);
  if (time_str == NULL) {
    fprintf(stderr, "Failed to convert time to string\n");
    return;
  }
  time_str[strcspn(time_str, "\n")] = '\0';
  printf("%s\t%" PRIu64 "\t%s\n", node->data.name,
         node->data.point, time_str);
}

/* 스코어 큰 순으로 AVL 트리의 데이터를 출력 */
uint64_t avl_print_data(avl_node* node) {
  if (node == NULL) {
    return 0;
  }
  uint64_t count = avl_print_data(node->right);
  avl_print_node(node);
  count += avl_print_data(node->left);
  return count + 1;
}

/* AVL 트리에서 범위 안에 있는 점수의 노드를 찾아 출력 */
bool avl_print_score_range(avl_node* node, uint64_t min, uint64_t max) {
  if (node == NULL) {
    return false;
  }
  bool found = false;
  found |= avl_print_score_range(node->right, min, max);
  if (node->data.point >= min && node->data.point <= max) {
    avl_print_node(node);
    found = true;
  }
  found |= avl_print_score_range(node->left, min, max);
  return found;
}

/* AVL 트리에서 특정 점수의 노드를 찾아 출력 */
bool avl_print_by_score(avl_node* node, uint64_t score) {
  return avl_print_score_range(node, score, score);
}

/* AVL 트리에서 이름으로 노드를 찾아 출력 */
bool avl_print_by_name(avl_node* node, const char* name) {
  if (node == NULL) {
    return false;
  }
  bool found = false;
  found |= avl_print_by_name(node->right, name);
  if (strcmp(node->data.name, name) == 0) {
    avl_print_node(node);
    found = true;
  }
  found |= avl_print_by_name(node->left, name);
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
