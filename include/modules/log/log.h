#ifndef MODULES_LOG_LOG_H_
#define MODULES_LOG_LOG_H_

#include <stdint.h>
#include "HAL/uart_hal.h"

extern uint8_t debug_verbose;
extern uint8_t debug_instance;

/* ANSI Text style */
#define	NORMAL					"\033[0m"
#define BOLD					"\033[1m"
#define UNDERLINE				"\033[4m"
#define BLINK					"\033[5m"
#define TEXT_RED				"\033[31m"
#define	TEXT_GREEN				"\033[32m"
#define	TEXT_YELLOW				"\033[33m"
#define	TEXT_BLUE				"\033[34m"
#define	TEXT_PURPLE				"\033[35m"
#define	TEXT_LIBLUE				"\033[36m"
#define	TEXT_WHITE				"\033[37m"
#define BG_RED					"\033[41m"
#define	BG_GREEN				"\033[42m"
#define	BG_YELLOW				"\033[43m"
#define	BG_BLUE					"\033[44m"
#define	BG_PURPLE				"\033[45m"
#define	BG_LIBLUE				"\033[46m"
#define	BG_WHITE				"\033[47m"
#define CLEANLINE				"\r\033[0K"

void log_error(char* fmt, ...);
void log_warning(char* fmt, ...);
void log_info(char* fmt, ...);
void log_debug(char* fmt, ...);
void log_customize(char* style, char* fmt, ...);

#endif /* MODULES_LOG_LOG_H_ */
