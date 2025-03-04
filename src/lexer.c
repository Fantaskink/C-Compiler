#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"
#include "tokens.h"

#define MAX_LEXEME_SIZE 255

typedef struct
{
    FILE *input_file;
    int current_line;
    int current_column;
    char current_char;
} Lexer;

void init_lexer(Lexer *lexer, FILE *file_pointer)
{
    lexer->input_file = file_pointer;
    lexer->current_line = 1;
    lexer->current_column = 0;
    lexer->current_char = '\0';
}

char next_char(Lexer *lexer)
{
    char c = fgetc(lexer->input_file);
    if (c == '\n')
    {
        lexer->current_line++;
        lexer->current_column = 0;
    }
    else
    {
        lexer->current_column++;
    }
    lexer->current_char = c;
    return c;
}

char peek_char(Lexer *lexer)
{
    char c = fgetc(lexer->input_file);
    if (c != EOF)
    {
        fseek(lexer->input_file, -1, SEEK_CUR);
    }
    return c;
}

void assign_lexeme(Token *token, char *lexeme)
{
    token->lexeme = (char *)malloc(strlen(lexeme) + 1);
    if (token->lexeme == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    strcpy(token->lexeme, lexeme);
}

Token check_reserved(char *lexeme, Token token) {
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
        assign_lexeme(&token, (char *)temp);
        return token;
      }
    }
  
    token.type = TOKEN_IDENTIFIER;
    assign_lexeme(&token, lexeme);
    return token;
  }

Token recognize_alpha(Lexer *lexer, Token token)
{
    
    int i = 0;
    char lexeme[MAX_LEXEME_SIZE];

    lexeme[i++] = lexer->current_char;

    char c = next_char(lexer);

    while (isalnum(c) || c == '_')
    {
        lexeme[i++] = c;
        c = next_char(lexer);
    }

    lexeme[i] = '\0';

    token.type = TOKEN_IDENTIFIER;

    assign_lexeme(&token, lexeme);

    token = check_reserved(token.lexeme, token); // Check if token is reserved

    return token;
}

Token next_token(Lexer *lexer)
{
    char c;
    Token token;

    do
    {
        c = next_char(lexer);
    } while (isspace(c));

    token.line = lexer->current_line;
    token.column = lexer->current_column;

    if (isalpha(c) || c == '_')
    {
        return recognize_alpha(lexer, token);
    }

    if (c == EOF)
    {
        printf("Recognized EOF\n");
        token.type = TOKEN_EOF;
        assign_lexeme(&token, "EOF");
        return token;
    }
    return token;
}

node_t *tokenize_input(FILE *file_pointer)
{
    Lexer lexer;
    init_lexer(&lexer, file_pointer);

    Token token = next_token(&lexer);
    node_t *token_list_head = create_node(token);

    while (token.type != TOKEN_EOF) {
        token = next_token(&lexer);
        push(&token_list_head, token);
      }

    fclose(lexer.input_file);

    return token_list_head;
}