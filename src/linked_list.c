#include "linked_list.h"
#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>

node_t *create_node(Token token) {
  node_t *new = (node_t *)malloc(sizeof(node_t));
  if (new == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(EXIT_FAILURE);
  }
  new->token = token;
  new->next = NULL;
  return new;
}

void push(node_t **head, Token token) {
  node_t *new_node = create_node(token);
  if (*head == NULL) {
    *head = new_node;
    return;
  }
  node_t *current = *head;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = new_node;
}

void delete_list(node_t *head) {

  while (head != NULL) {
    node_t *temp = head;
    head = head->next;
    free(temp);
  }
}

void print_list(node_t *head) {
  node_t *current = head;

  while (current != NULL) {
    TokenType token_type = current->token.type;
    char *value = current->token.value;

    printf("Token type: %s\n", token_names[token_type]);
    if (token_type == TOKEN_IDENTIFIER || token_type == TOKEN_INT_LITERAL ||
        token_type == TOKEN_FLOAT_LITERAL) {
      printf("Value: %s\n", value);
    }
    current = current->next;
  }
}
