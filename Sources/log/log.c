#include "modules/log/log.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

/**************************************************************************
 * Function Name:	string_append
 * Description:		string append for message and text style
 * @ param1 style:	ANSI escape code
 * @ param2 fmt:	Log message format
 * @ param3 arg:	message arguments
 * Returns: 		char array of output message
 **************************************************************************/
char* string_append(const char *style, const char *fmt, va_list arg) {
	char *msg = calloc(1024, sizeof(char));
	vsprintf(msg, fmt, arg);
    char *result = malloc(strlen(style) + strlen(msg) + strlen(NORMAL) + 3); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, style);
    strcat(result, msg);
    strcat(result, NORMAL);
    free(msg);
    return result;
}

/**************************************************************************
 * Function Name:	log_error
 * Description:		Output Error log with blink, red text
 * @ param1 fmt: 	log message format
 **************************************************************************/
void log_error(char* fmt, ...) {
	if(debug_verbose >= 1) {
		va_list arg;
		va_start(arg, fmt);
		va_end(arg);
		char* message = string_append(TEXT_RED, fmt, arg);
		uart_send_blocking(debug_instance, (uint8_t *)message);
		free(message);
	}
}

/**************************************************************************
 * Function Name:	log_warning
 * Description:		Output Warning log with yellow text
 * @ param1 fmt: 	log message format
 **************************************************************************/
void log_warning(char* fmt, ...) {
	if(debug_verbose >= 2) {
		va_list arg;
		va_start(arg, fmt);
		va_end(arg);
		char* message = string_append(TEXT_YELLOW, fmt, arg);
		uart_send_blocking(debug_instance, (uint8_t *)message);
		free(message);
	}
}

/**************************************************************************
 * Function Name:	log_info
 * Description:		Output Information log with green text
 * @ param1 fmt: 	log message format
 **************************************************************************/
void log_info(char* fmt, ...) {
	if(debug_verbose >= 3) {
		va_list arg;
		va_start(arg, fmt);
		va_end(arg);
		char* message = string_append(TEXT_GREEN, fmt, arg);
		uart_send_blocking(debug_instance, (uint8_t *)message);
		free(message);
	}
}

/**************************************************************************
 * Function Name:	log_debug
 * Description:		Output Debug log with normal(white) text
 * @ param1 fmt:	log message format
 **************************************************************************/
void log_debug(char* fmt, ...) {
	if(debug_verbose >= 5) {
		va_list arg;
		va_start(arg, fmt);
		va_end(arg);
		char* message = string_append(TEXT_WHITE, fmt, arg);
		uart_send_blocking(debug_instance, (uint8_t *)message);
		free(message);
	}
}

/**************************************************************************
 * Function Name:	log
 * Description:		Output log with self-define text style
 *
 * @ param1 style: 	text style. Style defined in log.h
 * @ param2 fmt: 	log message format
 *
 * Example:
 * 		log(BOLD TEXT_PURPLE BG_YELLOW, "My style");
 **************************************************************************/
void log_customize(char* style, char* fmt, ...) {
	if(debug_verbose >= 4) {
		va_list arg;
		va_start(arg, fmt);
		va_end(arg);
		char* message = string_append(style, fmt, arg);
		uart_send_blocking(debug_instance, (uint8_t *)message);
		free(message);
	}
}
