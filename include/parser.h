#ifndef PARSER_H
#define PARSER_H

#include "linked_list.h"
#include "tokens.h"
#include <stdlib.h>

typedef enum {
  AST_TRANSLATION_UNIT,
  AST_FUNCTION_DEF,
  AST_FUNCTION_DECL,
  AST_TYPE_SPEC,
  AST_IDENTIFIER,
  AST_PARAM_LIST,
  AST_PARAM_DECL,
  AST_COMPOUND_STMT,
  AST_EXPRESSION,
  AST_ADDITION_EXPR,
  AST_MULTIPLICATION_EXPR,
  AST_TERM,
  AST_FACTOR,
  AST_DECL,
  AST_INT_LITERAL,
  AST_FLOAT_LITERAL,
  AST_UNKNOWN,
  AST_NUM_TYPES,
} ASTNodeType;

typedef struct ASTNode {
  ASTNodeType type;
  Token *token;
  struct ASTNode **children;
  int child_count;
  int child_capacity;
} ASTNode_t;

ASTNode_t *get_ast(node_t *head);
#endif // !PARSER
