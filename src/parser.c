#include "parser.h"
#include "linked_list.h"
#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

node_t *current_node;
Token current_token;

ASTNode_t *translationUnit();
ASTNode_t *externalDeclaration();
ASTNode_t *functionDefinition();
ASTNode_t *typeSpecifier();
ASTNode_t *identifier();
ASTNode_t *parameterList();
ASTNode_t *paramDecl();
ASTNode_t *declaration();
ASTNode_t *decimalConstant();

void advance() {
  if (current_node->next != NULL) {
    current_node = current_node->next;
    current_token = current_node->token;
  }
}

void backtrack(node_t *backup) {
  current_node = backup;
  current_token = current_node->token;
}

ASTNode_t *create_ast_node(ASTNodeType type, Token *token) {
  ASTNode_t *node = malloc(sizeof(ASTNode_t));
  if (node == NULL) {
    perror("Failed to allocate memory for AST node");
    exit(EXIT_FAILURE);
  }

  if (token != NULL) {
    node->token = malloc(sizeof(Token));
    if (node->token == NULL) {
      perror("Failed to allocate memory for AST node token");
      free(node);
      exit(EXIT_FAILURE);
    }
    memcpy(node->token, token, sizeof(Token));
  } else {
    node->token = NULL;
  }

  node->type = type;
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
    perror("Failed to allocate memory for children nodes");
    exit(EXIT_FAILURE);
  }
  parent->children[parent->child_count++] = child;
}

void free_ast_node(ASTNode_t *node) {
  if (node == NULL)
    return;
  for (size_t i = 0; i < node->child_count; i++) {
    free_ast_node(node->children[i]);
  }
  free(node);
  free(node->children);
}

ASTNode_t *translationUnit() {
  ASTNode_t *ast = create_ast_node(AST_TRANSLATION_UNIT, NULL);

  while (current_token.type != TOKEN_EOF) {
    ASTNode_t *decl = externalDeclaration();
    if (decl != NULL) {
      add_child(ast, decl);
    } else {
      perror("Failed to parse external declaration");
      exit(EXIT_FAILURE);
    }
  }

  return ast;
}

ASTNode_t *externalDeclaration() {
  ASTNode_t *node = functionDefinition();

  if (node == NULL) {
    node = declaration();
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
    free_ast_node(type);
    backtrack(backup);
    return NULL;
  }

  ASTNode_t *params = parameterList();
  if (params == NULL) {
    free_ast_node(type);
    free_ast_node(ident);
    backtrack(backup);
    return NULL;
  }

  if (current_token.type == TOKEN_SEMICOLON) {
    advance();
    ASTNode_t *node = create_ast_node(AST_FUNCTION_DECL, NULL);
    add_child(node, type);
    add_child(node, ident);
    add_child(node, params);
    return node;
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
  case TOKEN_FLOAT:
  case TOKEN_VOID: {
    ASTNode_t *node = create_ast_node(AST_TYPE_SPEC, &current_token);
    advance();
    return node;
  }
  default:
    return NULL;
  }
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
    advance();
    return paramList;
  }

  ASTNode_t *param_decl = paramDecl();

  if (param_decl == NULL) {
    perror("Invalid parameter\n");
    exit(EXIT_FAILURE);
  }

  int ended_on_comma = 0;

  while (param_decl != NULL) {
    ended_on_comma = 0;
    add_child(paramList, param_decl);
    if (current_token.type == TOKEN_COMMA) {
      advance();
      ended_on_comma = 1;
    }
    param_decl = paramDecl();
  }

  if (ended_on_comma) {
    perror("Comma in parameter list must be follow by another parameter\n");
    exit(EXIT_FAILURE);
  }

  if (current_token.type == TOKEN_RPAREN) {
    advance();
    return paramList;
  }
  perror("Expected closing parenthesis in parameter list");
  return NULL;
}

ASTNode_t *paramDecl() {
  ASTNode_t *param_decl = create_ast_node(AST_PARAM_DECL, NULL);
  ASTNode_t *type = typeSpecifier();
  if (type == NULL) {
    free_ast_node(param_decl);
    return NULL;
  }
  add_child(param_decl, type);
  ASTNode_t *ident = identifier();
  if (ident == NULL) {
    free_ast_node(param_decl);
    return NULL;
  }

  add_child(param_decl, ident);
  return param_decl;
}

ASTNode_t *declaration() {
  ASTNode_t *node = create_ast_node(AST_DECL, NULL);
  ASTNode_t *type = typeSpecifier();
  if (type == NULL) {
    free_ast_node(node);
    return NULL;
  }
  ASTNode_t *ident = identifier();
  if (type == NULL) {
    free_ast_node(node);
    return NULL;
  }
  add_child(node, type);
  add_child(node, ident);

  if (current_token.type == TOKEN_ASSIGN) {
    advance();
    ASTNode_t *variable = decimalConstant();
    if (variable == NULL) {
      perror("Assignment in variable declaration must be followed by a decimal "
             "constant\n");
      return NULL;
    }
  }
  if (current_token.type == TOKEN_SEMICOLON) {
    advance();
    return node;
  }
  return NULL;
}

ASTNode_t *decimalConstant() {
  if (current_token.type == TOKEN_INT_LITERAL) {
    ASTNode_t *constant = create_ast_node(AST_INT_LITERAL, &current_token);
    advance();
    return constant;
  }
  if (current_token.type == TOKEN_FLOAT_LITERAL) {
    ASTNode_t *constant = create_ast_node(AST_FLOAT_LITERAL, &current_token);
    advance();
    return constant;
  }
  return NULL;
}

ASTNode_t *get_ast(node_t *head) {
  current_node = head;
  current_token = current_node->token;

  if (current_token.type != TOKEN_EOF) {
    return translationUnit();
  }
  return NULL;
}
