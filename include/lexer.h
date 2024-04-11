#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"
#include "tokens.h"

Token next_token();

node_t *tokenize_input(FILE *file_pointer);

#endif // !LEXER_H
