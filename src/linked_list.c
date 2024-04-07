#include "linked_list.h"
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
    switch (token_type) {
    case TOKEN_HEAD:
      printf("HEAD\n");
      break;
    case TOKEN_INT:
      printf("INT\n");
      break;
    case TOKEN_INT_LITERAL:
      printf("Int literal: %s\n", value);
      break;
    case TOKEN_IDENTIFIER:
      printf("Identifier: %s\n", value);
      break;
    case TOKEN_LPAREN:
      printf("LPAREN\n");
      break;
    case TOKEN_RPAREN:
      printf("RPAREN\n");
      break;
    case TOKEN_LBRACE:
      printf("LBRACE\n");
      break;
    case TOKEN_RBRACE:
      printf("RBRACE\n");
      break;
    case TOKEN_SEMICOLON:
      printf("SEMICOLON\n");
      break;
    case TOKEN_FLOAT:
      printf("FLOAT\n");
      break;
    case TOKEN_PLUS:
      printf("PLUS\n");
      break;
    case TOKEN_MINUS:
      printf("MINUS\n");
      break;
    case TOKEN_EOF:
      printf("EOF\n");
      break;
    case TOKEN_COMMA:
      printf("COMMA\n");
      break;
    default:
      printf("Unknown token type\n");
      break;
    }
    current = current->next;
  }
}
