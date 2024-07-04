#include "parser.h"
#include "linked_list.h"
#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  node_t *current_node;
  Token current_token;
} Parser;

ASTNode_t *translation_unit(Parser *parser);
ASTNode_t *external_declaration(Parser *parser);
ASTNode_t *function_definition(Parser *parser);
ASTNode_t *type_specifier(Parser *parser);
ASTNode_t *identifier(Parser *parser);
ASTNode_t *parameter_list(Parser *parser);
ASTNode_t *parameter_declaration(Parser *parser);
ASTNode_t *compound_statement(Parser *parser);
ASTNode_t *statement(Parser *parser);
ASTNode_t *expression_statement(Parser *parser);
ASTNode_t *expression(Parser *parser);
ASTNode_t *term(Parser *parser);
ASTNode_t *factor(Parser *parser);
ASTNode_t *assignment_expression(Parser *parser);
ASTNode_t *declaration(Parser *parser);
ASTNode_t *decimal_constant(Parser *parser);
ASTNode_t *get_ast(node_t *head);

/**
 * @brief Advances the parser to the next token in the linked list.
 *
 * This function updates the current node and current token of the parser
 * to the next node and token in the linked list.
 *
 * @param parser A pointer to the parser structure.
 */
void advance(Parser *parser) {
  if (parser->current_node->next != NULL) {
    parser->current_node = parser->current_node->next;
    parser->current_token = parser->current_node->token;
  }
}

/**
 * @brief Restores the parser's current node a specified backup node.
 *
 * This function updates the current node and current token of the parser to the
 * specified backup node from the linked list.
 *
 * @param parser A pointer to the parser structure.
 * @param backup A pointer to the specified backup linked list node containing
 * the current token.
 */

void backtrack(Parser *parser, node_t *backup) {
  parser->current_node = backup;
  parser->current_token = backup->token;
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

ASTNode_t *translation_unit(Parser *parser) {
  ASTNode_t *ast = create_ast_node(AST_TRANSLATION_UNIT, NULL);

  while (parser->current_token.type != TOKEN_EOF) {
    ASTNode_t *ext_decl = external_declaration(parser);
    if (ext_decl != NULL) {
      add_child(ast, ext_decl);
    }
  }
  return ast;
}

ASTNode_t *external_declaration(Parser *parser) {
  ASTNode_t *node = function_definition(parser);
  if (node == NULL) {
    node = declaration(parser);
  }
  if (node == NULL) {
    return NULL;
  }
  return node;
}

ASTNode_t *function_definition(Parser *parser) {
  node_t *backup = parser->current_node;

  ASTNode_t *type = type_specifier(parser);
  if (type == NULL)
    return NULL;

  ASTNode_t *ident = identifier(parser);
  if (ident == NULL) {
    free_ast_node(type);
    backtrack(parser, backup);
    return NULL;
  }

  ASTNode_t *params = parameter_list(parser);
  if (params == NULL) {
    free_ast_node(type);
    free_ast_node(ident);
    backtrack(parser, backup);
    return NULL;
  }

  if (parser->current_token.type == TOKEN_SEMICOLON) {
    advance(parser);
    ASTNode_t *node = create_ast_node(AST_FUNCTION_DECL, NULL);
    add_child(node, type);
    add_child(node, ident);
    add_child(node, params);
    return node;
  }

  ASTNode_t *compound_stmt = compound_statement(parser);
  if (compound_stmt == NULL) {
    free_ast_node(type);
    free_ast_node(ident);
    free_ast_node(params);
    return NULL;
  }

  ASTNode_t *node = create_ast_node(AST_FUNCTION_DEF, NULL);
  add_child(node, type);
  add_child(node, ident);
  add_child(node, params);
  add_child(node, compound_stmt);

  return node;
}

ASTNode_t *type_specifier(Parser *parser) {
  switch (parser->current_token.type) {
  case TOKEN_INT:
  case TOKEN_FLOAT:
  case TOKEN_VOID: {
    ASTNode_t *node = create_ast_node(AST_TYPE_SPEC, &parser->current_token);
    advance(parser);
    return node;
  }
  default:
    return NULL;
  }
}

ASTNode_t *identifier(Parser *parser) {
  if (parser->current_token.type == TOKEN_IDENTIFIER) {
    ASTNode_t *node = create_ast_node(AST_IDENTIFIER, &parser->current_token);
    advance(parser);
    return node;
  }
  return NULL;
}

ASTNode_t *parameter_list(Parser *parser) {
  if (parser->current_token.type != TOKEN_LPAREN) {
    return NULL;
  }
  ASTNode_t *paramList = create_ast_node(AST_PARAM_LIST, NULL);
  advance(parser); // Skip left parenthesis

  if (parser->current_token.type == TOKEN_RPAREN) {
    advance(parser);
    return paramList;
  }

  ASTNode_t *param_decl = parameter_declaration(parser);

  if (param_decl == NULL) {
    perror("Invalid parameter\n");
    exit(EXIT_FAILURE);
  }

  int ended_on_comma = 0;

  while (param_decl != NULL) {
    ended_on_comma = 0;
    add_child(paramList, param_decl);
    if (parser->current_token.type == TOKEN_COMMA) {
      advance(parser);
      ended_on_comma = 1;
    }
    param_decl = parameter_declaration(parser);
  }

  if (ended_on_comma) {
    perror("Comma in parameter list must be follow by another parameter\n");
    exit(EXIT_FAILURE);
  }

  if (parser->current_token.type == TOKEN_RPAREN) {
    advance(parser);
    return paramList;
  }
  perror("Expected closing parenthesis in parameter list");
  return NULL;
}

ASTNode_t *parameter_declaration(Parser *parser) {
  ASTNode_t *param_decl = create_ast_node(AST_PARAM_DECL, NULL);
  ASTNode_t *type = type_specifier(parser);
  if (type == NULL) {
    free_ast_node(param_decl);
    return NULL;
  }
  add_child(param_decl, type);
  ASTNode_t *ident = identifier(parser);
  if (ident == NULL) {
    free_ast_node(param_decl);
    return NULL;
  }

  add_child(param_decl, ident);
  return param_decl;
}

ASTNode_t *compound_statement(Parser *parser) {
  if (parser->current_token.type != TOKEN_LBRACE) {
    return NULL;
  }
  advance(parser);

  ASTNode_t *compound_statement = create_ast_node(AST_COMPOUND_STMT, NULL);

  while (parser->current_token.type != TOKEN_RBRACE &&
         parser->current_token.type != TOKEN_EOF) {
    ASTNode_t *node = statement(parser);
    if (node != NULL) {
      add_child(compound_statement, node);
      continue;
    }
    node = declaration(parser);
    if (node != NULL) {
      add_child(compound_statement, node);
      continue;
    }
    perror("Invalid statement or declaration in compound statement\n");
    free_ast_node(compound_statement);
    advance(parser);
  }

  if (parser->current_token.type == TOKEN_RBRACE) {
    advance(parser);
    return compound_statement;
  } else {
    perror("Expected closing brace in compound statement\n");
    free_ast_node(compound_statement);
    return NULL;
  }
}

ASTNode_t *statement(Parser *parser) {
  ASTNode_t *node = expression_statement(parser);
  if (node != NULL) {
    return node;
  }
  node = compound_statement(parser);
  if (node != NULL) {
    return node;
  }
  free_ast_node(node);
  return NULL;
}

ASTNode_t *expression_statement(Parser *parser) {
  ASTNode_t *node = expression(parser);
  if (node == NULL) {
    return NULL;
  }
  if (parser->current_token.type == TOKEN_SEMICOLON) {
    advance(parser);
    return node;
  }
  return NULL;
}

ASTNode_t *expression(Parser *parser) {
  ASTNode_t *node = term(parser); // Parse the first term

  while (parser->current_token.type == TOKEN_PLUS ||
         parser->current_token.type == TOKEN_MINUS) {
    Token op = parser->current_token; // Store the operator
    advance(parser);
    ASTNode_t *right = term(parser); // Parse the next term

    if (right == NULL) {
      free_ast_node(node);
      return NULL;
    }

    ASTNode_t *new_node = create_ast_node(AST_ADDITION_EXPR, &op);
    add_child(new_node, node);
    add_child(new_node, right);
    node = new_node; // Update the current node
  }

  return node;
}

ASTNode_t *term(Parser *parser) {
  ASTNode_t *node = factor(parser); // Parse the first factor

  while (parser->current_token.type == TOKEN_STAR ||
         parser->current_token.type == TOKEN_SLASH ||
         parser->current_token.type == TOKEN_MOD) {
    Token op = parser->current_token; // Store the operator
    advance(parser);
    ASTNode_t *right = factor(parser); // Parse the next factor

    if (right == NULL) {
      free_ast_node(node);
      return NULL;
    }

    ASTNode_t *new_node = create_ast_node(AST_MULTIPLICATION_EXPR, &op);
    add_child(new_node, node);
    add_child(new_node, right);
    node = new_node; // Update the current node
  }

  return node;
}

ASTNode_t *factor(Parser *parser) {
  if (parser->current_token.type == TOKEN_LPAREN) {
    advance(parser);
    ASTNode_t *expr = expression(parser); // Parse the nested expression
    if (parser->current_token.type == TOKEN_RPAREN) {
      advance(parser);
      return expr;
    } else {
      perror("Expected closing parenthesis");
      free_ast_node(expr);
      return NULL;
    }
  } else if (parser->current_token.type == TOKEN_INT_LITERAL ||
             parser->current_token.type == TOKEN_FLOAT_LITERAL) {
    return decimal_constant(parser); // Handle integer and float literals
  } else if (parser->current_token.type == TOKEN_IDENTIFIER) {
    return identifier(parser); // Handle identifiers
  } else {
    perror("Expected a factor");
    return NULL;
  }
}
ASTNode_t *assignment_expression(Parser *parser) { return NULL; }

ASTNode_t *declaration(Parser *parser) {
  ASTNode_t *node = create_ast_node(AST_DECL, NULL);
  ASTNode_t *type = type_specifier(parser);
  if (type == NULL) {
    free_ast_node(node);
    return NULL;
  }
  ASTNode_t *ident = identifier(parser);
  if (ident == NULL) {
    free_ast_node(node);
    return NULL;
  }
  add_child(node, type);
  add_child(node, ident);

  if (parser->current_token.type == TOKEN_ASSIGN) {
    advance(parser);
    ASTNode_t *expr = expression(parser);
    if (expr == NULL) {
      perror("Assignment in variable declaration must be followed by a decimal "
             "constant\n");
      return NULL;
    }
    add_child(node, expr);
  }
  if (parser->current_token.type == TOKEN_SEMICOLON) {
    advance(parser);
    return node;
  }
  return NULL;
}

ASTNode_t *decimal_constant(Parser *parser) {
  if (parser->current_token.type == TOKEN_INT_LITERAL) {
    ASTNode_t *constant =
        create_ast_node(AST_INT_LITERAL, &parser->current_token);
    advance(parser);
    return constant;
  }
  if (parser->current_token.type == TOKEN_FLOAT_LITERAL) {
    ASTNode_t *constant =
        create_ast_node(AST_FLOAT_LITERAL, &parser->current_token);
    advance(parser);
    return constant;
  }
  return NULL;
}

ASTNode_t *get_ast(node_t *head) {
  Parser parser;
  parser.current_node = head;
  parser.current_token = head->token;

  if (parser.current_token.type != TOKEN_EOF) {

    return translation_unit(&parser);
  }
  return NULL;
}
