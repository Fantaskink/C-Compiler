#ifndef TOKENS_H
#define TOKENS_H
#define VALUE_MAX

typedef enum {
  TOKEN_HEAD,
  TOKEN_INT,
  TOKEN_INT_LITERAL,
  TOKEN_IDENTIFIER,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_SEMICOLON,
  TOKEN_FLOAT,
  TOKEN_FLOAT_LITERAL,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_FOR,
  TOKEN_RETURN,
  TOKEN_ASSIGN,
  TOKEN_EQUAL,
  TOKEN_COMMA,
  TOKEN_EOF
} TokenType;

typedef struct {
  TokenType type;
  char value[255];
} Token;

#endif /* TOKENS_H */
