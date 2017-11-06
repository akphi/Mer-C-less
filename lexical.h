#ifndef LEX_H
#define LEX_H

#include <regex.h>
#include <stdio.h>

/** 
 * struct token (Token) - store information from token definition file.
 * @name:	name of the token
 * @pattern:	POSIX regex pattern to match the token
 * @regex:	the compiled regex using the pattern
 */
typedef struct token {
	char *name;
	char *pattern;
	regex_t regex;
} Token;

/** 
 * struct lex_token (Lex_Token) - store information of a lexeme.
 * @lexeme:	the content of the lexeme
 * @token:	the token the lexeme is associated with
 */
typedef struct lex_token {
	char *lexeme;
	Token *token;
} Lex_Token;

/* FILE pointer of the input file */
extern FILE *input_file;
/* FILE pointer of the token definition file */
extern FILE *token_def_file;
/* list of token collected from the token definition file */
extern Token **token_list;
/* the current line being read */
extern char line[];
/* the current line number */
extern int line_number;
/* the current column number */
extern int col_number;
/* boolean indicates if tab usage has been spotted */
extern int has_tab_space;

/**
 * load_input() - load the input file.
 * @file_name: 	name of the file
 *
 * Return: 	0: success
 * 		-1: file not found
 */
int load_input(char *file_name);

/**
 * setup_regex() - compile the regex.
 * @regex: 		pointer to the regex to be compiled
 * @regex_pattern:	POSIX string regex pattern
 *
 * Return: 		return value of regcmp()
 */
int setup_regex(regex_t *regex, char *regex_pattern);

/**
 * get_token_definitions() - load token definition file and create token list.
 *
 * Return: 	0: 		success
 * 		-1: 		file not found
 * 		otherwise:	value of setup_regex()
 */
int get_token_definitions(void);

/**
 * get_first_match() - check if the regex matches the lexeme from the
 * starting position (a left-most match).
 * @value: 	the string to be matched against
 * @regex:	the compile regex
 *
 * Return: 	the ending position of the match
 */
int get_first_match(char *value, regex_t *regex);

/**
 * ltrim() - left-trim trailing space and notify if tab usage was detected.
 * @value: 	the string to be trimmed
 *
 * Return: 	the trimmed string
 */
char *ltrim(char *value);

/**
 * lookup() - find the first token to match the lexeme.
 * @next_token:	the next token
 *
 * Return: 	the ending position of the match
 */
int lookup(Token **next_token);

/**
 * lex() - get the next token from the input file.
 *
 * Return: 	the next token and next lexeme wrapped inside a &Lex_Token
 */
Lex_Token *lex(void);

/**
 * clean_lex() - cleanup token definition list.
 */
void clean_lex(void);

#endif /* LEX_H */