#include "lexical.h"
#include "parse_error.h"
#include "setting.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

extern Parse_Error *error_list;

FILE *input_file;
FILE *token_def_file;
Token **token_list;
char line[MAX_LINE_LENGTH];
int line_number;
int col_number;
int has_tab_space = 0;

int get_token_definitions()
{
	int return_value = 0;
	/* check if file exist */
	if (!(token_def_file = fopen(TOKEN_DEFINITION_FILE, "r"))) {
		printf("%sERROR - cannot open %s%s\n", ERROR_COL,
		       TOKEN_DEFINITION_FILE, COL_RESET);
		return -1;
	}
	int token_list_length, max_token_name_length,
	    max_token_regex_pattern_length;
	fscanf(token_def_file, "%d", &token_list_length);
	fscanf(token_def_file, "%d", &max_token_name_length);
	fscanf(token_def_file, "%d", &max_token_regex_pattern_length);

	token_list =
	    (Token **)malloc((token_list_length + 1) * sizeof(Token *));
	char *name = (char *)malloc(max_token_name_length * sizeof(char));
	char *pattern =
	    (char *)malloc(max_token_regex_pattern_length * sizeof(char));
	for (int i = 0; i < token_list_length; ++i) {
		/* create new Token for each defnition read */
		Token *new_token = (Token *)malloc(sizeof(Token));
		fscanf(token_def_file, "%s %s", name, pattern);
		char *name_copy = (char *)malloc(strlen(name) + 1);
		strcpy(name_copy, name);
		new_token->name = name_copy;
		char *pattern_copy = (char *)malloc(strlen(pattern) + 1);
		strcpy(pattern_copy, pattern);
		new_token->pattern = pattern_copy;
		regex_t regex;
		/* check if the regex pattern is compilable */
		return_value = setup_regex(&regex, pattern);
		if (return_value)
			return return_value;
		new_token->regex = regex;
		*(token_list + i) = new_token;
	}
	*(token_list + token_list_length) = NULL;
	/* check the number of definition in the file against the number
	 * specified in the definition file header */
	if (getc(token_def_file) != EOF) {
		printf("%sWARNING - detect more than %d token "
		       "definition(s). Please update the header so new "
		       "definition(s) can be loaded.%s\n",
		       WARNING_COL, token_list_length, COL_RESET);
	}
	fclose(token_def_file);
	return 0;
}

int setup_regex(regex_t *regex, char *regex_pattern)
{
	int return_value;
	return_value = regcomp(regex, regex_pattern, REG_EXTENDED);
	if (return_value) {
		printf("%sERROR - cannot compile regex pattern: %s%s\n",
		       ERROR_COL, regex_pattern, COL_RESET);
	}
	return return_value;
}

int get_first_match(char *value, regex_t *regex)
{
	regmatch_t pmatch[1];
	int return_value = regexec(regex, value, 1, pmatch, 0);
	/* make sure the match start at position 0 (a left-most match) */
	if (!return_value && !pmatch->rm_so) {
		return pmatch->rm_eo;
	}
	return 0;
}

int lookup(Token **next_token)
{
	int return_value = 0;
	for (int i = 0; token_list[i]; ++i) {
		return_value = get_first_match(line, &(token_list[i]->regex));
		/* return the first matching token */
		if (return_value) {
			*next_token = token_list[i];
			return return_value;
		}
	}
	*next_token = NULL;
	return return_value;
}

int load_input(char *file_name)
{
	/* check if file exist */
	if (!(input_file = fopen(file_name, "r"))) {
		printf("%sERROR - cannot open file: %s%s\n", ERROR_COL,
		       file_name, COL_RESET);
		return -1;
	}
#if defined(DEBUG) && defined(LEX_DEBUG_ENABLED)
	printf("%sProcessing file: %s%s\n", DEBUG_COL, file_name, COL_RESET);
#endif
	line_number = 0;
	return 0;
}

Lex_Token *lex()
{
	/* always left-trimmed first*/
	ltrim(line);
	/* check for moving to a new line or EOF */
	while (!strlen(line)) {
		char *file_line = fgets(line, MAX_LINE_LENGTH, input_file);
		if (!file_line) {
#if defined(DEBUG) && defined(LEX_DEBUG_ENABLED)
			printf("%sReached end of file (EOF)%s\n", DEBUG_COL,
			       COL_RESET);
#endif
			fclose(input_file);
			/* EOF token is NULL */
			return NULL;
		}
		col_number = 0;
		++line_number;
		ltrim(line);
#if defined(DEBUG) && defined(LEX_DEBUG_ENABLED)
		printf("%sProcessing line %d%s\n", DEBUG_COL, line_number,
		       COL_RESET);
#endif
	}
	Token *next_token;
	int lexeme_upper_bound = lookup(&next_token);
	/* handle unknown token, exceedingly long token and COMMENT
	 * notice here that comment is trimmed before checking for long lexeme
	 */
	if (!lexeme_upper_bound) {
		/* 0 is returned for the ending position of the match, this
		 * means there is no match */
		add_error("ERROR - cannot identify token", line_number,
			  col_number, col_number + 1);
		/* take away the first character and call lex() again */
		memmove(line, line + 1, strlen(line));
		col_number += 1;
		return lex();
	} else if (!strcmp(next_token->name, "COMMENT")) {
		/* strip comment */
		memmove(line, line + lexeme_upper_bound,
			strlen(line) - (lexeme_upper_bound - 1));
		col_number += lexeme_upper_bound;
		return lex();
	} else if (lexeme_upper_bound > MAX_LEXEME_LENGTH) {
		add_error("ERROR - lexeme is too long", line_number, col_number,
			  col_number + lexeme_upper_bound);
		/* take the long lexeme out and call lex() again */
		memmove(line, line + lexeme_upper_bound,
			strlen(line) - (lexeme_upper_bound - 1));
		col_number += lexeme_upper_bound;
		return lex();
	}
	/* handle legal token */
	char *next_lexeme = (char *)malloc(lexeme_upper_bound * sizeof(char));
	strncpy(next_lexeme, line, lexeme_upper_bound);
	next_lexeme[lexeme_upper_bound] = '\0';
	memmove(line, line + lexeme_upper_bound,
		strlen(line) - (lexeme_upper_bound - 1));
#if defined(DEBUG) && defined(LEX_DEBUG_ENABLED)
	printf("%sIdentified token %s, and lexeme '%s' [%d:%d-%d]%s\n",
	       DEBUG_COL, next_token->name, next_lexeme, line_number,
	       col_number + 1, col_number + lexeme_upper_bound + 1, COL_RESET);
#endif
	col_number += lexeme_upper_bound;

	/* use static variable here instead of declaring it globally the
	 * downside is that we will miss the deallocation of the last
	 * next_lex_token */
	static Lex_Token *next_lex_token = NULL;
	if (!next_lex_token) {
		next_lex_token = (Lex_Token *)malloc(sizeof(Lex_Token));
	} else {
		free(next_lex_token->lexeme);
	}
	next_lex_token->lexeme = next_lexeme;
	next_lex_token->token = next_token;
	return next_lex_token;
}

char *ltrim(char *value)
{
	while (isspace(*value)) {
		/* detect tab usage since tab will mess up error messages'
		 * location information */
		if (*value == '\t') {
			has_tab_space = 1;
			col_number = col_number + TAB_SIZE;
		} else {
			++col_number;
		}
		memmove(value, value + 1, strlen(value));
	}
	return value;
}

void clean_lex()
{
#if defined(DEBUG) && defined(PARSE_DEBUG_ENABLED)
	printf("%sCleaning up token definition list%s\n", INFO_COL, COL_RESET);
#endif
	for (int i = 0; token_list[i]; ++i) {
		free(token_list[i]->name);
		token_list[i]->name = NULL;
		free(token_list[i]->pattern);
		token_list[i]->pattern = NULL;
		regfree(&(token_list[i]->regex));
	}
	free(token_list);
	token_list = NULL;
}
