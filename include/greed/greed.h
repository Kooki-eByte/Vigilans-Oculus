#ifndef GREED_H
#define GREED_H
/* --- GREED LOGGER --- */
/*
 * Greed is a basic logging library, utilizing useful logs for informing a
 * developer based on different levels of logging and colorizing them to ensure
 * readability. Being GREEDY with everything involved in the code.
*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <stdio.h>
#include <time.h>

typedef enum { LOG_DBG, LOG_SCS, LOG_WRN, LOG_ERR, LOG_LVL_LEN } LOG_LEVEL;

static const char *type[LOG_LVL_LEN] = {"DEBUG", "SUCCESS", "WARNING", "ERROR"};

static const char *colors[LOG_LVL_LEN] = {
    "\x1b[34m", /* blue */
    "\x1b[32m", /* green */
    "\x1b[33m", /* yellow */
    "\x1b[31m"  /* red */
};

void g_logger(LOG_LEVEL lvl, const char *msg, const char *file, int line);

#define g_log_debug(msg) g_logger(LOG_DBG, msg, __FILE__, __LINE__);
#define g_log_success(msg) g_logger(LOG_SCS, msg, __FILE__, __LINE__);
#define g_log_warning(msg) g_logger(LOG_WRN, msg, __FILE__, __LINE__);
#define g_log_error(msg) g_logger(LOG_ERR, msg, __FILE__, __LINE__);

#ifdef GREED_IMPLEMENTATION
static const char *DEFAULT_COLOR = "\x1b[0m";

static time_t current_time;
static struct tm *m_time;

static void get_time() {
  time(&current_time);
  m_time = localtime(&current_time);
}

inline void g_logger(LOG_LEVEL lvl, const char *msg, const char *file, int line) {
  get_time();
  fprintf(stderr, "%s", colors[lvl]);
  fprintf(stderr, "[%s]", type[lvl]);
  fprintf(stderr, "[%d/%d/%d -> %d:%d:%d]--", m_time->tm_mday, m_time->tm_mon,
         m_time->tm_year + 1900, m_time->tm_hour, m_time->tm_min,
         m_time->tm_sec);
  fprintf(stderr, "[%s on line %u in file %s]\n%s", msg, line, file, DEFAULT_COLOR);
}
#endif // GREED_IMPLEMENTATION

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // GREED_H