#ifndef SETTING_H
#define SETTING_H

//================================================================================
// PARSER
//================================================================================

/* PARSE_DEBUG option controls debugging message inside of the parser */
#define PARSE_DEBUG_ENABLED 1
/* CODE_DISPLAY option controls source matching feature of the parser */
#define CODE_DISPLAY_ENABLED 1
/* TAB_SIZE_WARNING option controls warning message about tab usage */
#define TAB_SIZE_WARNING_ENABLED 1
/* SUCCESS_DISPLAY option controls successful parsing (without error) message */
#define SUCCESS_DISPLAY_ENABLED 1

//================================================================================
// LEXICAL ANALYZER
//================================================================================

/* LEX_DEBUG option controls debugging message inside of the lexical analyzer */
#define LEX_DEBUG_ENABLED 1
/* SYN_DEBUG option controls debugging message inside of the syntax analyzer */
#define TAB_SIZE 8
/* TOKEN_DEFINITION_FILE indicates the definition file for tokens and the POSIX
 * regex pattern used to match these tokens */
#define TOKEN_DEFINITION_FILE "token_definition.txt"
/* MAX_LINE_LENGTH option controls how many characters to be read at most for a
 * line in the lexical analyzer */
#define MAX_LINE_LENGTH 1000
/* MAX_MESSAGE_LENGTH option controls how many characters to be used at most for
* a lexeme */
#define MAX_LEXEME_LENGTH 100

//================================================================================
// SYNTAX ANALYZER
//================================================================================

#define SYN_DEBUG_ENABLED 1
/* TAB_SIZE option specifies how many space(s) a tab size is equated to */
/* MAX_PARSE_DISPLAY_DEPTH option controls the max depth of indentation for the
* syntax analyzer debugging message(s) */
#define MAX_PARSE_DISPLAY_DEPTH 20
/* MAX_PARSE_DISPLAY_DEPTH option controls the how many space(s) used for an
* indentation for the syntax analyzer debugging message(s) */
#define PARSE_DISPLAY_TAB_LENGTH 2
/* MAX_MESSAGE_LENGTH option controls how many characters to be used at most for
* a message everywhere */
#define MAX_MESSAGE_LENGTH 100

//================================================================================
// DISPLAY
//================================================================================

/* MONOCHROME option controls color display of the output in the terminal */
#ifdef MONOCHROME
#define COL_RESET ""
#define ERROR_COL ""
#define DEBUG_COL ""
#define INFO_COL ""
#define SUCCESS_COL ""
#define NORMAL_COL ""
#define WARNING_COL ""
#define CODE_DISPLAY_ERROR_COL ""
#else
#define COL_RESET "\x1b[0m"
#define ERROR_COL "\x1b[31;1m"
#define DEBUG_COL "\x1b[90m"
#define INFO_COL "\x1b[34;1m"
#define SUCCESS_COL "\x1b[32;1m"
#define NORMAL_COL "\x1b[37m"
#define WARNING_COL "\x1b[33;1m"
#define CODE_DISPLAY_ERROR_COL "\x1b[41m"
#endif

/* ANSI TEXT FOREGROUND COLOR
 *
 * RESET "\x1b[39m"
 * LIGHT_GRAY "\x1b[90m"
 * BLACK "\x1b[30m"
 * LIGHT_RED "\x1b[31;1m"
 * RED "\x1b[31m"
 * LIGHT_GREEN "\x1b[32;1m"
 * GREEN "\x1b[32m"
 * LIGHT_YELLOW "\x1b[33;1m"
 * YELLOW "\x1b[33m"
 * LIGHT_BLUE "\x1b[34;1m"
 * BLUE "\x1b[34m"
 * LIGHT_MAGENTA "\x1b[35;1m"
 * MAGENTA "\x1b[35m"
 * LIGHT_CYAN "\x1b[36;1m"
 * CYAN "\x1b[36;1m"
 * LIGHT_WHITE "\x1b[37;1m"
 * WHITE "\x1b[37m"
 */

/* ANSI TEXT BACKGROUND COLOR
 *
 * RESET "\x1b[49m"
 * BLACK "\x1b[40m"
 * LIGHT_GRAY "\x1b[100m"
 * RED "\x1b[41m"
 * LIGHT_RED "\x1b[101m"
 * YELLOW "\x1b[42m"
 * LIGHT_YELLOW "\x1b[102m"
 * GREEN "\x1b[43m"
 * LIGHT_GREEN "\x1b[103m"
 * BLUE "\x1b[44m"
 * LIGHT_MAGENTA "\x1b[104m"
 * MAGENTA "\x1b[45m"
 * LIGHT_CYAN "\x1b[105m"
 * CYAN "\x1b[46m"
 * LIGHT_WHITE "\x1b[106m"
 * WHITE "\x1b[47m"
 */

/* ANSI TEXT ATTRIBUTE
 *
 * RESET "\x1b[0m"
 * ITALICS_ON "\x1b[3m"
 * ITALICS_OFF "\x1b[23m"
 * BOLD_ON "\x1b[1m"
 * BOLD_OFF "\x1b[21m"
 * UNDERLINE_ON "\x1b[4m"
 * UNDERLINE_OFF "\x1b[24m"
 * BLINK_ON "\x1b[5m"
 * BLINK_OFF "\x1b[25m"
 * INVERSE_ON "\x1b[7m"
 * INVERSE_OFF "\x1b[27m"
 */

#endif /* SETTING_H */