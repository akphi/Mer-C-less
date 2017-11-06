#include "syntax.h"
#include "parse_error.h"
#include "setting.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Parse_Error *error_list;
extern int error_junk_after_program_end;
extern int error_unexpected_eof;
extern int line_number;
extern int col_number;

Lex_Token *lex_token;

char *options_variable[] = {"PROGNAME_VARIABLE", "VARIABLE"};
char *options_simpl_stmt[] = {"PROGNAME_VARIABLE", "VARIABLE", "READ", "WRITE"};
char *options_struct_stmt[] = {"BEGIN", "IF", "WHILE"};

void indent_depth(int change)
{
	static int depth;
	char format[MAX_PARSE_DISPLAY_DEPTH * PARSE_DISPLAY_TAB_LENGTH];

	if (change < 0) {
		depth += change;
		if (depth < 0)
			depth = 0;
	}

	if (depth > 0) {
		int temp = depth * PARSE_DISPLAY_TAB_LENGTH;
		sprintf(format, "%%%d.%ds", temp, temp);
		printf(format, "");
	}

	if (change > 0)
		depth += change;
}

void enter_non_terminal(char *non_terminal_name)
{
#if defined(DEBUG) && defined(SYN_DEBUG_ENABLED)
	indent_depth(1);
	printf("%s%s (enter)%s\n", NORMAL_COL, non_terminal_name, COL_RESET);
#endif
}

void exit_non_terminal(char *non_terminal_name)
{
#if defined(DEBUG) && defined(SYN_DEBUG_ENABLED)
	indent_depth(-1);
	printf("%s%s (exit)%s\n", NORMAL_COL, non_terminal_name, COL_RESET);
#endif
}

int are_equal(Lex_Token *lex_token, char *token_name)
{
	if (!lex_token)
		return 0;
	if (!strcmp(lex_token->token->name, token_name))
		return 1;
	return 0;
}

int are_equal_any(Lex_Token *lex_token, char *token_names[], int size)
{
	if (!lex_token)
		return 0;
	for (int i = 0; i < size; ++i) {
		if (!strcmp(lex_token->token->name, token_names[i]))
			return 1;
	}
	return 0;
}

void check_token(char *token_name, char *expected_token)
{
	/* only get next token if the current token matched */
	if (are_equal(lex_token, token_name)) {
		lex_token = lex();
	} else {
		if (!lex_token) {
			return;
		} else {
			add_syntax_error(expected_token);
		}
	}
}

void check_token_any(char *token_names[], int size, char *expected_token)
{
	if (are_equal_any(lex_token, token_names, size)) {
		lex_token = lex();
	} else {
		if (!lex_token) {
			return;
		} else {
			add_syntax_error(expected_token);
		}
	}
}

void add_syntax_error(char *expected_token)
{
	char error_msg[MAX_MESSAGE_LENGTH];
	sprintf(error_msg, "ERROR - expect %s but saw '%s'", expected_token,
		lex_token->lexeme);
	add_error(error_msg, line_number,
		  col_number - strlen(lex_token->lexeme), col_number);
}

void program()
{
	enter_non_terminal("<program>");

	/* <program> ::= program <progname> <compound stmt> */
	lex_token = lex();
	EXIT_IF_NULL();
	check_token("PROGRAM", "'program'");
	EXIT_IF_NULL();
	check_token("PROGNAME_VARIABLE", "<progname>");
	EXIT_IF_NULL();
	compound_statement();

	/* check if there is junk after the end of program */
	if (!error_unexpected_eof) {
		if (lex_token) {
			error_junk_after_program_end = 1;
			add_error("ERROR - detect non-empty content after end "
				  "of program",
				  line_number,
				  col_number - strlen(lex_token->lexeme),
				  MAX_LINE_LENGTH + 1);
			return;
		}
	} else {
		return;
	}

	exit_non_terminal("<program>");
}

void compound_statement()
{
	enter_non_terminal("<compound_statement>");

	/* <compound stmt> ::= begin <stmt> {; <stmt>} end */
	check_token("BEGIN", "'begin'");
	EXIT_IF_NULL();
	statement();
	EXIT_IF_NULL();
	while (are_equal(lex_token, "SEMICOLON")) {
		lex_token = lex();
		EXIT_IF_NULL();
		statement();
	}
	EXIT_IF_NULL();
	check_token("END", "end");

	exit_non_terminal("<compound_statement>");
}

void statement()
{
	enter_non_terminal("<statement>");

	/* <stmt> ::= <simple stmt> | <structured stmt> */
	if (are_equal_any(lex_token, options_simpl_stmt,
			  ARRAY_SIZE(options_simpl_stmt))) {
		simple_statement();
	} else if (are_equal_any(lex_token, options_struct_stmt,
				 ARRAY_SIZE(options_struct_stmt))) {
		structured_statement();
	} else {
		add_syntax_error("<variable>, \'read\', \'write\', "
				 "\'begin\', \'if\', or \'while\'");
	}
	EXIT_IF_NULL();

	exit_non_terminal("<statement>");
}

void simple_statement()
{
	enter_non_terminal("<simple_statement>");

	/* <simple stmt> ::= <assignment stmt> | <read stmt> | <write stmt> |
			  <comment> */
	if (are_equal_any(lex_token, options_variable,
			  ARRAY_SIZE(options_variable))) {
		assignment_statement();
	} else if (are_equal(lex_token, "READ")) {
		read_statement();
	} else if (are_equal(lex_token, "WRITE")) {
		write_statement();
	} else {
		add_syntax_error("<variable>, \'read\', or \'write\'");
	}
	EXIT_IF_NULL();

	exit_non_terminal("<simple_statement>");
}

void assignment_statement()
{
	enter_non_terminal("<assignment_statement>");

	/* <assignment stmt> ::= <variable> := <expression> */

	check_token_any(options_variable, ARRAY_SIZE(options_variable),
			"<variable>");
	EXIT_IF_NULL();
	check_token("ASSIGNING_OPERATOR", "':='");
	EXIT_IF_NULL();
	expression();
	EXIT_IF_NULL();

	exit_non_terminal("<assignment_statement>");
}

void read_statement()
{
	enter_non_terminal("<read_statement>");

	/* <read stmt> ::= read ( <variable> { , <variable> } ) */
	check_token("READ", "':='");
	EXIT_IF_NULL();
	check_token("LEFT_PARENTHESIS", "'('");
	EXIT_IF_NULL();
	check_token_any(options_variable, ARRAY_SIZE(options_variable),
			"<variable>");
	EXIT_IF_NULL();
	while (are_equal(lex_token, "COMMA")) {
		lex_token = lex();
		EXIT_IF_NULL();
		check_token_any(options_variable, ARRAY_SIZE(options_variable),
				"<variable>");
	}
	EXIT_IF_NULL();
	check_token("RIGHT_PARENTHESIS", "')'");
	EXIT_IF_NULL();

	exit_non_terminal("<read_statement>");
}

void write_statement()
{
	enter_non_terminal("<write_statement>");

	/* <write stmt> ::= write ( <expression> { , <expression> } ) */
	check_token("WRITE", "'write");
	EXIT_IF_NULL();
	check_token("LEFT_PARENTHESIS", "'('");
	EXIT_IF_NULL();
	expression();
	EXIT_IF_NULL();
	while (are_equal(lex_token, "COMMA")) {
		lex_token = lex();
		EXIT_IF_NULL();
		expression();
	}
	EXIT_IF_NULL();
	check_token("RIGHT_PARENTHESIS", "')'");
	EXIT_IF_NULL();

	exit_non_terminal("<write_statement>");
}

void structured_statement()
{
	enter_non_terminal("<structured_statement>");

	/* <structured stmt> ::= <compound stmt> | <if stmt> | <while stmt> */
	if (are_equal(lex_token, "BEGIN")) {
		compound_statement();
	} else if (are_equal(lex_token, "IF")) {
		if_statement();
	} else if (are_equal(lex_token, "WHILE")) {
		while_statement();
	} else {
		add_syntax_error("\'begin\', \'if\', or \'while\'");
	}
	EXIT_IF_NULL();

	exit_non_terminal("<structured_statement>");
}

void if_statement()
{
	enter_non_terminal("<if_statement>");

	/* <if stmt> ::= if <expression> then <stmt> |
			 if <expression> then <stmt> else <stmt> */
	check_token("IF", "'if'");
	EXIT_IF_NULL();
	expression();
	EXIT_IF_NULL();
	check_token("THEN", "'then'");
	EXIT_IF_NULL();
	statement();
	EXIT_IF_NULL();
	if (are_equal(lex_token, "ELSE")) {
		lex_token = lex();
		EXIT_IF_NULL();
		statement();
	}
	EXIT_IF_NULL();

	exit_non_terminal("<if_statement>");
}

void while_statement()
{
	enter_non_terminal("<while_statement>");

	/* <while stmt> ::= while <expression> do <stmt> */
	check_token("WHILE", "'while'");
	EXIT_IF_NULL();
	expression();
	EXIT_IF_NULL();
	check_token("DO", "'do'");
	EXIT_IF_NULL();
	statement();
	EXIT_IF_NULL();

	exit_non_terminal("<while_statement>");
}

void expression()
{
	enter_non_terminal("<expression>");

	/* <expression> ::= <simple expr> |
			    <simple expr> <relational_operator> <simple expr> */
	simple_expression();
	EXIT_IF_NULL();
	if (are_equal(lex_token, "RELATIONAL_OPERATOR")) {
		lex_token = lex();
		EXIT_IF_NULL();
		simple_expression();
	}
	EXIT_IF_NULL();

	exit_non_terminal("<expression>");
}

void simple_expression()
{
	enter_non_terminal("<simple_expression>");

	/* <simple expr> ::= [ <sign> ] <term> { <adding_operator> <term> } */
	if (are_equal(lex_token, "ADDING_OPERATOR")) {
		lex_token = lex();
		EXIT_IF_NULL();
	}
	term();
	EXIT_IF_NULL();
	while (are_equal(lex_token, "ADDING_OPERATOR")) {
		lex_token = lex();
		EXIT_IF_NULL();
		term();
	}
	EXIT_IF_NULL();

	exit_non_terminal("<simple_expression>");
}

void term()
{
	enter_non_terminal("<term>");

	/* <term> ::= <factor> { <multiplying_operator> <factor> } */
	factor();
	EXIT_IF_NULL();
	while (are_equal(lex_token, "MULTIPLYING_OPERATOR")) {
		lex_token = lex();
		EXIT_IF_NULL();
		factor();
	}
	EXIT_IF_NULL();

	exit_non_terminal("<term>");
}

void factor()
{
	enter_non_terminal("<factor>");

	/* <factor> ::= <variable> | <constant> | ( <expression> ) */
	if (are_equal_any(lex_token, options_variable,
			  ARRAY_SIZE(options_variable))) {
		lex_token = lex();
	} else if (are_equal(lex_token, "CONSTANT")) {
		lex_token = lex();
	} else if (are_equal(lex_token, "LEFT_PARENTHESIS")) {
		lex_token = lex();
		EXIT_IF_NULL();
		expression();
		EXIT_IF_NULL();
		check_token("RIGHT_PARENTHESIS", "')'");
	} else {
		add_syntax_error("<variable>, <constant>, or ( <expression> )");
	}
	EXIT_IF_NULL();

	exit_non_terminal("<factor>");
}