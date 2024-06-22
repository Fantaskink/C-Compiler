#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#include "parser.h"

static const char *ASTNodeTypeStrings[AST_NUM_TYPES] = {
    "AST_TRANSLATION_UNIT", "AST_FUNCTION_DEF", "AST_FUNCTION_DECL",
    "AST_TYPE_SPEC",        "AST_IDENTIFIER",   "AST_PARAM_LIST",
    "AST_PARAM_DECL",       "AST_DECL",         "AST_INT_LITERAL",
    "AST_FLOAT_LITERAL",    "AST_UNKNOWN",
};

void print_ast(ASTNode_t *ast);

#endif // !
