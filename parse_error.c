#include "parse_error.h"
#include "setting.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Parse_Error *error_list = NULL;
int error_junk_after_program_end = 0;
int error_unexpected_eof = 0;

void add_error(char *message, int line_number, int start_col, int end_col)
{
	/* create Parse_Error and add to the end of the error list */
	Parse_Error *new_error = (Parse_Error *)malloc(sizeof(Parse_Error));
	char *message_copy = (char *)malloc(strlen(message) + 1);
	strcpy(message_copy, message);
	new_error->message = message_copy;
	new_error->line_number = line_number;
	new_error->start_col = start_col;
	new_error->end_col = end_col;
	new_error->next = NULL;
	print_error(new_error);
	if (!error_list) {
		error_list = new_error;
	} else {
		Parse_Error *current = error_list;
		while (current->next) {
			current = current->next;
		}
		current->next = new_error;
	}
}

void remove_error()
{
	if (error_list) {
		/* iterate to the end of the error list */
		Parse_Error *current = error_list;
		if (current->next) {
			Parse_Error *prev;
			while (current->next) {
				prev = current;
				current = current->next;
			}
			prev->next = NULL;
		} else {
			error_list = NULL;
		}
		/* deallocation the error */
		free(current->message);
		free(current);
		current = NULL;
	}
}
void print_error(Parse_Error *error)
{
	/* if the error end column is specified as MAX_LINE_LENGTH + 1, which is
	 * impossible to reach, the end position is not printed*/
	if ((MAX_LINE_LENGTH + 1) == error->end_col) {
		printf("%s%s [%d:%d]%s\n", ERROR_COL, error->message,
		       error->line_number, error->start_col + 1, COL_RESET);
	} else {
		/* add 1 to column position to make column start from 1 instead
		 * of 0 */
		printf("%s%s [%d:%d-%d]%s\n", ERROR_COL, error->message,
		       error->line_number, error->start_col + 1,
		       error->end_col + 1, COL_RESET);
	}
}

void print_error_list()
{
	if (error_list) {
		/* iteratively print error in the list */
		for (Parse_Error *current = error_list; current;
		     current = current->next)
			print_error(current);
	}
}

void clean_error_list()
{
#if defined(DEBUG) && defined(PARSE_DEBUG_ENABLED)
	printf("%sCleaning up parse error list%s\n", INFO_COL, COL_RESET);
#endif
	/* iteratively remove error from in the list */
	while (error_list) {
		remove_error();
	}
}