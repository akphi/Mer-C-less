#ifndef PARSE_ERROR_H
#define PARSE_ERROR_H

/**
 * struct parse_error (Parse_Error) - store information of a parser error
 * (currently being implemented as a linked list node).
 * @line_number:	the line where error occurs
 * @start_col:		the column where the error starts
 * @end_col:		the column where the error ends
 * @message:		the error message
 * @next:		pointer to the next error in the error list
 */
typedef struct parse_error {
	int line_number;
	int start_col;
	int end_col;
	char *message;
	struct parse_error *next;
} Parse_Error;

/* the error list */
extern Parse_Error *error_list;
/* boolean indicates if junk after program end was detected */
extern int error_junk_after_program_end;
/* boolean indicates if unexpected EOF was detected */
extern int error_unexpected_eof;

/**
 * add_error() - create error, print it and add to the end of the error list.
 * @message:		the error message
 * @line_number:	the line where error occurs
 * @start_col:		the column where the error starts
 * @end_col:		the column where the error ends
 */
void add_error(char *message, int line_number, int start_col, int end_col);

/**
 * remove_error() - remove the error at the end of the error list.
 */
void remove_error(void);

/**
 * print_error() - print the error.
 * @error:	the &Parse_Error error
 */
void print_error(Parse_Error *error);

/**
 * print_error_list() - print the list of error.
 */
void print_error_list(void);

/**
 * clean_error_list() - cleanup the error list.
 */
void clean_error_list(void);

#endif /* PARSE_ERROR_H */