#ifndef PARSER_H
#define PARSER_H

#include "parse_error.h"

/**
 * parse() - run the syntax analyzer.
 */
void parse(void);

/**
 * lex_only() - only run the lexical analyzer.
 */
void lex_only(void);

/**
 * code_display() - conduct and print error matching for the source code.
 */
void code_display(void);

/**
 * check_code_error_from_list() - check if the current position has any error
 * @error: 			pointer to the current error in the error list
 * @has_reached_error_list_end: boolean indicates if the end of the rror list is reached
 * @current_line:		the current line of the source code
 * @current_col:		the current column of the source code
 *
 * Return: 			0: no error
 * 				1: has error
 */
int check_code_error_from_list(Parse_Error **error,
			       int *has_reached_error_list_end,
			       int current_line, int current_col);

/**
 * cleanup() - cleanup the parser leftover.
 */
void cleanup(void);

#endif /* PARSER_H */