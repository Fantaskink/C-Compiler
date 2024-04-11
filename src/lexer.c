#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"
#include "tokens.h"

FILE *input_file;

char next_char() { return fgetc(input_file); }

Token check_reserved(char *value) {
  Token token;
  int i;
  for (i = 0; i < NUM_TOKENS; i++) {
    char temp[255];
    strcpy(temp, token_names[i]);

    for (int j = 0; temp[j]; j++) {
      temp[j] = tolower(temp[j]);
    }

    if (strcmp(value, temp) == 0) {
      token.type = (TokenType)i;
      return token;
    }
  }

  token.type = TOKEN_IDENTIFIER;
  strcpy(token.value, value);
  return token;
}

Token recognize_alpha(char c) {
  Token token;
  int i = 0;
  do {
    token.value[i++] = c;
    c = next_char();
  } while (isalnum(c) || c == '_');

  token.value[i] = '\0';
  fseek(input_file, -1, SEEK_CUR);
  token.type = TOKEN_IDENTIFIER;

  token = check_reserved(token.value); // Check if token is reserved

  return token;
}

Token recognize_number(char c) {
  Token token;
  int i = 0;
  do {
    token.value[i++] = c;
    c = next_char();
  } while (isdigit(c));

  if (c == '.' && isdigit(next_char())) {
    fseek(input_file, -1, SEEK_CUR);
    do {
      token.value[i++] = c;
      c = next_char();
    } while (isdigit(c));
    token.type = TOKEN_FLOAT_LITERAL;
  } else {
    token.type = TOKEN_INT_LITERAL;
  }
  fseek(input_file, -1, SEEK_CUR);
  token.value[i] = '\0';
  return token;
}

Token recognise_eof(char c) {
  Token token;
  strcpy(token.value, "End of file");
  token.type = TOKEN_EOF;
  return token;
}

Token recognise_special(char c) {
  Token token;
  char next_c = next_char();
  if (next_c == '=') {
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
      token.type = TOKEN_ASSIGN;
      break;
    }
    return token;
  }

  fseek(input_file, -1, SEEK_CUR);
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
    token.type = TOKEN_DIV;
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
  default:
    token.type = TOKEN_EOF; // Assuming EOF for unrecognized characters
    break;
  }
  return token;
}

Token recognise_string(char c) {
  Token token;
  token.type = TOKEN_STRING_LITERAL;
  int i = 0;

  do {
    c = next_char();

    token.value[i++] = c;

    if (c == '\"') {
      token.value[i - 1] = '\0'; // Null-terminate the string
      break; // Exit the loop upon encountering closing quote
    }

    if (c == '\n') {
      fprintf(stderr, "Error: Missing closing '\"'\n");
      exit(EXIT_FAILURE); // Exiting upon error
    }
  } while (c != EOF);

  return token;
}

Token next_token() {
  char c;
  Token token;

  do {
    c = next_char();
  } while (isspace(c));

  if (isalpha(c) || c == '_') {
    return recognize_alpha(c);
  }

  if (isdigit(c)) {
    return recognize_number(c);
  }

  if (c == EOF) {
    token.type = TOKEN_EOF;
    strcpy(token.value, "EOF");
  }

  if (c == '"') {
    return recognise_string(c);
  }

  // Ignore comments
  if (c == '/') {
    if (next_char() == '/') {
      while (c != '\n' && c != '\r') {
        c = next_char();
      }
      return next_token();
    } else {
      // Move file pointer back after checking one ahead for a
      fseek(input_file, -1, SEEK_CUR);
    }
  }

  return recognise_special(c);
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
