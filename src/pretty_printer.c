#include "pretty_printer.h"
#include "parser.h"
#include "tokens.h"
#include <stdio.h>

int depth = 0;

void print_ast(ASTNode_t *ast) {
  if (ast == NULL) {
    return;
  }

  for (int i = 0; i < depth; i++) {
    printf("  ");
  }

  printf("%s", ASTNodeTypeStrings[ast->type]);

  if (ast->token != NULL) {
    printf(" (%s)", ast->token->lexeme);
  }

  for (int i = 0; i < ast->child_count; i++) {
    depth++;
    printf("\n");
    print_ast(ast->children[i]);
    depth--;
  }
}
