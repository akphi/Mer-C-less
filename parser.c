#include "parser.h"
#include "lexical.h"
#include "setting.h"
#include "syntax.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE *input_file;
extern Parse_Error *error_list;
extern int error_junk_after_program_end;
extern int error_unexpected_eof;
extern int has_tab_space;

/* main driver */
int main(int argc, char **argv)
{
#ifdef DEBUG
	printf("%sDEBUG MODE ENABLED%s\n", DEBUG_COL, COL_RESET);
#endif
	int return_value = 0;
	return_value = get_token_definitions();

	/* check token definition file is loaded properly */
	if (return_value)
		return -2;

	/* check if the argument indicating the input file is specfied */
	if (argc < 2) {
		printf("You must supply the input file name "
		       "on the command line\n");
		exit(EXIT_FAILURE);
	}

	/* check if the input is loaded properly */
	++argv;
	return_value = load_input(*argv);
	if (return_value != 0)
		exit(EXIT_FAILURE);

	/* run the parser */
	parse();

#ifndef DISABLE_TAB_SIZE_WARNING
#if TAB_SIZE_WARNING_ENABLED == 1
	/* print warning about tab usage as if the TAB_SIZE option and the tab
	 * size used by the source code is different, then the error location
	 * information will be off */
	if (error_list && has_tab_space)
		printf("%sWARNING - detect usage of tab(s), column location "
		       "might be off since a tab is currently counted as %d "
		       "space(s) (check TAB_SIZE option in setting.h)%s\n",
		       WARNING_COL, TAB_SIZE, COL_RESET);
#endif
#endif

#if SUCCESS_DISPLAY_ENABLED == 1
	/* print success message if no error was found */
	if (!error_list)
		printf("%sSUCCESS - completed parsing with no errors%s\n",
		       SUCCESS_COL, COL_RESET);
#endif

#ifndef DISABLE_SOURCE_DISPLAY
#if CODE_DISPLAY_ENABLED == 1
	/* error matching for source code */
	if (error_list) {
		load_input(*argv);
		printf("%s%s%s\n", DEBUG_COL, *argv, COL_RESET);
		code_display();
	}
#endif
#endif
	/* cleanup the mess the parser left behind */
	cleanup();
	exit(EXIT_SUCCESS);
}

void parse()
{
	program();
}

void lex_only()
{
	Lex_Token *lex_token;
	do {
		lex_token = lex();
	} while (lex_token != NULL);
}

void code_display()
{
	int current_char;
	int has_error;
	int current_line = 1;
	int current_col = -1;
	Parse_Error *error = error_list;
	int *has_reached_error_list_end = (int *)malloc(sizeof(int));
	*has_reached_error_list_end = 0;
	if (input_file) {
		while ((current_char = getc(input_file)) != EOF) {
			/* check the current character and modify current_line
			 * and current_col */
			if (current_char == '\n') {
				current_line++;
				current_col = -1;
			} else if (current_char == '\t') {
				current_col += TAB_SIZE;
			} else {
				current_col++;
			}

			/* check for error in the current cell */
			has_error = check_code_error_from_list(
			    &error, has_reached_error_list_end, current_line,
			    current_col);

			/* decorate character to be printed in error matched
			 * source code */
			if (!has_error) {
				printf("%c", current_char);
			} else if (current_char == '\n') {
				printf("%s %c%s", CODE_DISPLAY_ERROR_COL,
				       current_char, COL_RESET);
			} else {
				printf("%s%c%s", CODE_DISPLAY_ERROR_COL,
				       current_char, COL_RESET);
			}
		}
	}
	/* handle unexpected EOF error */
	Parse_Error *last_error = error_list;
	while (last_error->next) {
		last_error = last_error->next;
	}
	if (error_unexpected_eof) {
		printf("%s%c%s", CODE_DISPLAY_ERROR_COL, ' ', COL_RESET);
	}
	printf("\n");

	/* cleanup */
	free(has_reached_error_list_end);
	has_reached_error_list_end = NULL;
	fclose(input_file);
}

void cleanup()
{
	clean_lex();
	clean_error_list();
}

int check_code_error_from_list(Parse_Error **error,
			       int *has_reached_error_list_end,
			       int current_line, int current_col)
{
	if (*has_reached_error_list_end && error_junk_after_program_end) {
		return 1;
	}
	if (*has_reached_error_list_end)
		return 0;
	/* make sure no error is marked if the error line is not reached */
	if ((*error)->line_number > current_line)
		return 0;
	/* get to the error line */
	while ((*error)->line_number < current_line) {
		if (!(*error)->next) {
			*has_reached_error_list_end = 1;
			return 0;
		} else {
			error = &((*error)->next);
		}
	}
	/* try to reach error column */
	while (((*error)->end_col - 1) < current_col) {
		if (!(*error)->next) {
			/* no more error */
			*has_reached_error_list_end = 1;
			return 0;
		} else if (current_line < (*error)->next->line_number) {
			/* no more error on the current line */
			return 0;
		} else {
			error = &((*error)->next);
		}
	}
	/* make sure the current cell falls within the bound of the error */
	if (current_line == (*error)->line_number) {
		if (((*error)->end_col - 1) < current_col)
			return 0;
		if ((*error)->start_col <= current_col)
			return 1;
	}
	return 0;
}