#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#include "parser.h"

static const char *ASTNodeTypeStrings[AST_NUM_TYPES] = {
    "Translation Unit",
    "Function Definition",
    "Function Declaration",
    "Type Specifier",
    "Identifier",
    "Parameter List",
    "Parameter Declaration",
    "Compound Statement",
    "Expression",
    "Addition Expression",
    "Multiplication Expression",
    "Term",
    "Factor",
    "Declaration",
    "Int Literal",
    "Float Literal",
    "UNKNOWN",
};

void print_ast(ASTNode_t *ast);

#endif //! PRETTY_PRINTER
