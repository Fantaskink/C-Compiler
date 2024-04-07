#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "tokens.h"

typedef struct node {
  Token token;
  struct node *next;
} node_t;

node_t *create_node(Token token);
void push(node_t **head, Token token);
void delete_list(node_t *head);
void print_list(node_t *head);

#endif
/* LINKED_LIST_H */
