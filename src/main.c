#include "lexer.h"
#include "linked_list.h"
#include "parser.h"
#include "pretty_printer.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input file path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *file_pointer = fopen(argv[1], "r");
  if (file_pointer == NULL) {
    perror("File not found");

    return EXIT_FAILURE;
  }

  node_t *token_list = tokenize_input(file_pointer);

  print_list(token_list);

  //ASTNode_t *ast = get_ast(token_list);

  //print_ast(ast);

  return 0;
}
