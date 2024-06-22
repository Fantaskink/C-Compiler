#include "lexer.h"
#include "linked_list.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s input_file\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *file_pointer = fopen(argv[1], "r");
  if (file_pointer == NULL) {
    perror("File not found");

    return EXIT_FAILURE;
  }

  node_t *token_list = tokenize_input(file_pointer);

  ASTNode_t *ast = get_ast(token_list);

  print_list(token_list);

  return 0;
}
