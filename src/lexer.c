#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"
#include "tokens.h"

FILE *input_file;
int current_line = 1;
int current_column = 0;

char next_char() {
  char c = fgetc(input_file);
  if (c == '\n') {
    current_line++;
    current_column = 0;
  } else {
    current_column++;
  }
  return c;
}

char peek_char() {
  char c = fgetc(input_file);
  fseek(input_file, -1, SEEK_CUR);
  return c;
}

void assign_lexeme(Token *token, const char *lexeme) {
  token->lexeme = (char *)malloc(strlen(lexeme) + 1);
  if (token->lexeme == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(EXIT_FAILURE);
  }
  strcpy(token->lexeme, lexeme);
}

/* Check if token is a reserved keyword, return identifier if not, return
 * correct token type if reserved */
Token check_reserved(const char *lexeme, Token token) {
  int i;
  for (i = 0; i < NUM_TOKENS; i++) {
    char temp[255];
    /* Make a lowercase copy of the token's name to see if it matches a reserved
     * keyword */
    strcpy(temp, token_names[i]);
    for (int j = 0; temp[j]; j++) {
      temp[j] = tolower(temp[j]);
    }

    if (strcmp(lexeme, temp) == 0) {
      token.type = (TokenType)i;
      assign_lexeme(&token, (const char *)temp);
      return token;
    }
  }

  token.type = TOKEN_IDENTIFIER;
  assign_lexeme(&token, lexeme);
  return token;
}

// Recognize an identifier
Token recognize_alpha(char c, Token token) {
  int i = 0;
  char lexeme[255];
  do {
    lexeme[i++] = c;
    c = next_char();
  } while (isalnum(c) || c == '_');

  lexeme[i] = '\0';
  fseek(input_file, -1, SEEK_CUR);
  current_column--;
  if (c == '\n') {
    current_line--;
  }

  assign_lexeme(&token, lexeme);

  token.type = TOKEN_IDENTIFIER;

  token = check_reserved(token.lexeme, token); // Check if token is reserved

  return token;
}

Token recognize_number(char c, Token token) {
  int i = 0;
  char lexeme[255];

  // Consume the first digit if `c` is already a digit
  if (isdigit(c)) {
    lexeme[i++] = c;
  }

  // Read integer part
  while (isdigit(peek_char())) {
    c = next_char();
    lexeme[i++] = c;
  }

  // Check for decimal part
  if (peek_char() == '.') {
    c = next_char(); // Consume the decimal point
    lexeme[i++] = c;
    if (isdigit(peek_char())) {
      // Read fractional part
      do {
        c = next_char();
        lexeme[i++] = c;
      } while (isdigit(peek_char()));
      token.type = TOKEN_FLOAT_LITERAL;
    } else {
      fprintf(stderr, "Error: Expected digit after decimal point\n");
      exit(EXIT_FAILURE);
    }
  } else if (isalpha(peek_char())) {
    c = next_char();
    fprintf(stderr, "Error: Incorrect character '%c' in decimal constant\n", c);
    exit(EXIT_FAILURE);
  } else {
    token.type = TOKEN_INT_LITERAL;
  }

  lexeme[i] = '\0';
  assign_lexeme(&token, lexeme);
  return token;
}

Token recognise_eof(char c, Token token) {
  char *lexeme = "EOF";
  token.lexeme = lexeme;
  token.type = TOKEN_EOF;
  return token;
}

Token recognise_special(char c, Token token) {
  char lexeme[3];
  lexeme[0] = c;
  lexeme[1] = '\0'; // Initialize to handle single character tokens by default

  char next_c = peek_char();
  if (next_c == '=') {
    next_char();
    lexeme[1] = next_c;
    lexeme[2] = '\0';

    switch (c) {
    case '=':
      token.type = TOKEN_EQ;
      break;
    case '!':
      token.type = TOKEN_NEQ;
      break;
    case '<':
      token.type = TOKEN_LTE;
      break;
    case '>':
      token.type = TOKEN_GTE;
      break;
    default:
      token.type = TOKEN_UNRECOGNIZED;
      break;
    }
    assign_lexeme(&token, lexeme);
    return token;
  }

  switch (c) {
  case '+':
    token.type = TOKEN_PLUS;
    break;
  case '-':
    token.type = TOKEN_MINUS;
    break;
  case '(':
    token.type = TOKEN_LPAREN;
    break;
  case ')':
    token.type = TOKEN_RPAREN;
    break;
  case ';':
    token.type = TOKEN_SEMICOLON;
    break;
  case '{':
    token.type = TOKEN_LBRACE;
    break;
  case '}':
    token.type = TOKEN_RBRACE;
    break;
  case ',':
    token.type = TOKEN_COMMA;
    break;
  case '*':
    token.type = TOKEN_STAR;
    break;
  case '/':
    token.type = TOKEN_SLASH;
    break;
  case '%':
    token.type = TOKEN_MOD;
    break;
  case '<':
    token.type = TOKEN_LT;
    break;
  case '>':
    token.type = TOKEN_GT;
    break;
  case '=':
    token.type = TOKEN_ASSIGN;
    break;
  case '&':
    token.type = TOKEN_AMPERSAND;
    break;
  default:
    token.type = TOKEN_UNRECOGNIZED; // Assuming a separate token type for
                                     // unrecognized characters
    break;
  }

  assign_lexeme(&token, lexeme);
  return token;
}

Token recognise_string(char c, Token token) {
  char lexeme[255];
  token.type = TOKEN_STRING_LITERAL;
  int i = 0;

  while ((c = next_char()) != EOF) {
    if (c == '\"') {
      lexeme[i] = '\0'; // Null-terminate the string
      break;
    }

    if (c == '\n') {
      fprintf(stderr, "Error: Missing closing '\"'\n");
      exit(EXIT_FAILURE); // Exiting upon error
    }

    lexeme[i++] = c;
  }

  if (c == EOF) {
    fprintf(stderr, "Error: Unexpected EOF while reading string literal\n");
    exit(EXIT_FAILURE);
  }

  assign_lexeme(&token, lexeme);

  return token;
}

Token next_token() {
  char c;
  Token token;

  do {
    c = next_char();
  } while (isspace(c));

  token.line = current_line;
  token.column = current_column;

  if (isalpha(c) || c == '_') {
    return recognize_alpha(c, token);
  }

  if (isdigit(c)) {
    return recognize_number(c, token);
  }

  if (c == EOF) {
    token.type = TOKEN_EOF;
    assign_lexeme(&token, "EOF");
    return token;
  }

  if (c == '"') {
    return recognise_string(c, token);
  }

  // Ignore comments
  if (c == '/') {
    if (peek_char() == '/') {
      while (c != '\n' && c != '\r') {
        c = next_char();
      }
      return next_token();
    }
  }

  token = recognise_special(c, token);
  if (token.type == TOKEN_UNRECOGNIZED) {
    fprintf(stderr,
            "Error: unrecognized character at line %d, column %d: '%c'\n",
            token.line, token.column, c);
    exit(EXIT_FAILURE);
  }
  return token;
}

node_t *tokenize_input(FILE *file_pointer) {
  input_file = file_pointer;

  Token token;

  token = next_token();
  node_t *token_list_head = create_node(token);

  while (token.type != TOKEN_EOF) {
    token = next_token();
    push(&token_list_head, token);
  }

  fclose(input_file);

  return token_list_head;
}
