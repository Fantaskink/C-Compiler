#ifndef TOKENS_H
#define TOKENS_H
#define VALUE_MAX

typedef enum {
  TOKEN_HEAD,

  // Primitive types
  TOKEN_INT,
  TOKEN_CHAR,
  TOKEN_FLOAT,
  TOKEN_VOID,

  // Statements
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_FOR,
  TOKEN_RETURN,

  // Literals
  TOKEN_INT_LITERAL,
  TOKEN_IDENTIFIER,
  TOKEN_FLOAT_LITERAL,
  TOKEN_STRING_LITERAL,

  // Delimiters
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_SEMICOLON,
  TOKEN_COMMA,

  // Operators
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_DIV,
  TOKEN_MOD,
  TOKEN_ASSIGN,

  // Comparison
  TOKEN_LT,
  TOKEN_GT,
  TOKEN_LTE,
  TOKEN_GTE,
  TOKEN_EQ,
  TOKEN_NEQ,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_EOF,

  NUM_TOKENS
} TokenType;

static const char *token_names[NUM_TOKENS] = {"HEAD",
                                              "INT",
                                              "CHAR",
                                              "FLOAT",
                                              "VOID",
                                              "IF",
                                              "ELSE",
                                              "WHILE",
                                              "FOR",
                                              "RETURN",
                                              "INT_LITERAL",
                                              "IDENTIFIER",
                                              "FLOAT_LITERAL",
                                              "STRING_LITERAL",
                                              "LPAREN",
                                              "RPAREN",
                                              "LBRACE",
                                              "RBRACE",
                                              "SEMICOLON",
                                              "COMMA",
                                              "PLUS",
                                              "MINUS",
                                              "STAR",
                                              "DIV",
                                              "MOD",
                                              "ASSIGN",
                                              "LT",
                                              "GT",
                                              "LTE",
                                              "GTE",
                                              "EQ",
                                              "NEQ",
                                              "AND",
                                              "OR",
                                              "EOF"};

typedef struct {
  TokenType type;
  char value[255];
} Token;

#endif /* TOKENS_H */
