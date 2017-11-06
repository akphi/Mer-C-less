#ifndef SYNTAX_H
#define SYNTAX_H

#include "lexical.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* the check for EOF token must be written in macro so that the return command
 * can exit the current non-terminal function */
#define EXIT_IF_NULL()                                                         \
	if (!lex_token) {                                                      \
		if (!error_unexpected_eof) {                                   \
			add_error("ERROR - detect unexpected EOF",             \
				  line_number, col_number,                     \
				  MAX_LINE_LENGTH + 1);                        \
			error_unexpected_eof = 1;                              \
		}                                                              \
		return;                                                        \
	}

/* the current token being checked */
extern Lex_Token *lex_token;
/* group of options to check for non-terminal or terminal */
extern char *options_variable[];
extern char *options_simpl_stmt[];
extern char *options_struct_stmt[];

/**
 * add_syntax_error() - create a &Parse_Error error and print it.
 * @expected_token: 	the token(s) to be expected
 */
void add_syntax_error(char *expected_token);

/**
 * are_equal() - check if the specified token match the expected token.
 * @lex_token: 		the current token
 * @token_name: 	the name of the expected token
 *
 * Return: 	0: unmatched
 * 		1: matched
 */
int are_equal(Lex_Token *lex_token, char *token_name);

/**
 * are_equal_any() - check if the specified token match the expected tokens.
 * @lex_token: 		the current token
 * @token_names: 	the list of names of the expected tokens
 * @size: 		the size of the list of names of the expected tokens
 *
 * Return: 	0: unmatched
 * 		1: matched
 */
int are_equal_any(Lex_Token *lex_token, char *token_names[], int size);

/**
 * check_token() - get the next token if the specified token match the expected token, otherwise, add a syntax error and keep the current token for the next syntax analyzer step.
 * @token_name: 	the name of the expected token
 * @expected_token: 	the token(s) to be expected
 */
void check_token(char *token_name, char *expected_token);

/**
 * check_token_any() - get next token if the specified token match the expected tokens, otherwise, add a syntax error and keep the current token for the next syntax analyzer step.
 * @token_names: 	the list of names of the expected tokens
 * @size: 		the size of the list of names of the expected tokens
 * @expected_token: 	the token(s) to be expected
 */
void check_token_any(char *token_names[], int size, char *expected_token);

/* group of debugging message display functions */
void indent_depth(int depth);
void enter_non_terminal(char *non_terminal_name);
void exit_non_terminal(char *non_terminal_name);

/* group of non-terminal syntax analyzer functions */
void program(void);
void compound_statement(void);
void statement(void);
void simple_statement(void);
void assignment_statement(void);
void read_statement(void);
void write_statement(void);
void structured_statement(void);
void if_statement(void);
void while_statement(void);
void expression(void);
void simple_expression(void);
void term(void);
void factor(void);

#endif /* SYNTAX_H */