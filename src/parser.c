#include "parser.h"
#include "linked_list.h"
#include "tokens.h"
#include <stdlib.h>

node_t *current_node;
Token current_token;

ASTNode_t *translationUnit();
ASTNode_t *externalDeclaration();
ASTNode_t *functionDefinition();
ASTNode_t *typeSpecifier();
ASTNode_t *identifier();
ASTNode_t *parameterList();
ASTNode_t *paramDecl();

void advance() {
  current_node = current_node->next;
  current_token = current_node->token;
  return;
}

void backtrack(node_t *backup) {
  current_node = backup;
  current_token = current_node->token;
}

ASTNode_t *create_ast_node(ASTNodeType type, Token *token) {
  ASTNode_t *node = malloc(sizeof(ASTNode_t));
  if (node == NULL) {
    return NULL;
  }
  node->type = type;
  node->token = token;
  node->children = NULL;
  node->child_count = 0;
  node->child_capacity = 0;
  return node;
}

void add_child(ASTNode_t *parent, ASTNode_t *child) {
  if (parent->child_capacity == 0) {
    // Initial capacity
    parent->child_capacity = 2;
    parent->children =
        (ASTNode_t **)malloc(parent->child_capacity * sizeof(ASTNode_t *));
  } else if (parent->child_count >= parent->child_capacity) {
    // Double the capacity
    parent->child_capacity *= 2;
    parent->children = (ASTNode_t **)realloc(
        parent->children, parent->child_capacity * sizeof(ASTNode_t *));
  }
  if (parent->children == NULL) {
    // Handle memory allocation failure
    exit(-1);
  }
  parent->children[parent->child_count++] = child;
}

ASTNode_t *translationUnit() {
  ASTNode_t *ast = create_ast_node(AST_TRANSLATION_UNIT, NULL);

  while (current_token.type != TOKEN_EOF) {
    externalDeclaration();
  }

  return ast;
}

ASTNode_t *externalDeclaration() {
  ASTNode_t *node = functionDefinition();

  if (node == NULL) {
    // declaration
  }
  return node;
}

ASTNode_t *functionDefinition() {
  node_t *backup = current_node;

  ASTNode_t *type = typeSpecifier();
  if (type == NULL)
    return NULL;

  ASTNode_t *ident = identifier();
  if (ident == NULL) {
    backtrack(backup);
    return NULL;
  }

  ASTNode_t *params = parameterList();
  if (params == NULL) {
    backtrack(backup);
    return NULL;
  }

  ASTNode_t *node = create_ast_node(AST_FUNCTION_DEF, NULL);
  add_child(node, type);
  add_child(node, ident);
  add_child(node, params);
  return node;
}

ASTNode_t *typeSpecifier() {
  switch (current_token.type) {
  case TOKEN_INT:
    break;
  case TOKEN_FLOAT:
    break;
  case TOKEN_VOID:
    break;
  default:
    return NULL;
  }

  ASTNode_t *node = create_ast_node(AST_TYPE_SPEC, &current_token);
  advance();
  return node;
}

ASTNode_t *identifier() {
  if (current_token.type == TOKEN_IDENTIFIER) {
    ASTNode_t *node = create_ast_node(AST_IDENTIFIER, &current_token);
    advance();
    return node;
  }
  return NULL;
}

ASTNode_t *parameterList() {
  if (current_token.type != TOKEN_LPAREN) {
    return NULL;
  }
  ASTNode_t *paramList = create_ast_node(AST_PARAM_LIST, NULL);
  advance(); // Skip left parenthesis

  if (current_token.type == TOKEN_RPAREN) {
    ASTNode_t *empty = create_ast_node(AST_PARAM_LIST, NULL);
    advance();
    return empty;
  }

  ASTNode_t *param_decl = paramDecl();

  while (param_decl != NULL) {
    add_child(paramList, param_decl);
    if (current_token.type == TOKEN_COMMA) {
      advance();
    }
    param_decl = paramDecl();
  }

  if (current_token.type == TOKEN_RPAREN) {
    advance();
    return paramList;
  }
  return NULL;
}

ASTNode_t *paramDecl() {
  ASTNode_t *param_decl = create_ast_node(AST_PARAM_DECL, NULL);
  ASTNode_t *type = typeSpecifier();
  if (type == NULL) {
    return NULL;
  }
  add_child(param_decl, type);
  advance();
  ASTNode_t *ident = identifier();
  if (ident != NULL) {
    advance();
    add_child(param_decl, ident);
  }

  return param_decl;
}

ASTNode_t *get_ast(node_t *head) {
  current_node = head;
  current_token = current_node->token;

  if (current_token.type != TOKEN_EOF) {
    return translationUnit();
  }
  return NULL;
}
