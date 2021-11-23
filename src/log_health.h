#ifndef _inc_HEALTH_CHECK_H
#define _inc_HEALTH_CHECK_H

#include <stdio.h>
#include <stdarg.h>

#define	TIMENOW_RFC8601_SIZE 64

struct connection;

extern char *_timenow_rfc8601(char buf[TIMENOW_RFC8601_SIZE]);

extern void _log_connection(const char *filename, int linenumber,
			    const char *funcname, struct connection *conn,
			    int fd, const char *fmt, ...)
    __attribute__((format(printf, 6, 7)));

#define LOG_CONNECTION(CONN, FD, FMT, ...)		\
  do {							\
    int saved_errno = errno;				\
    _log_connection(__FILE__, __LINE__, __func__,	\
		    CONN, FD,				\
		    FMT, ##__VA_ARGS__);		\
    errno = saved_errno;				\
  } while (0)

#define LOG_HCHK_CONN(CONN, FD, FMT, ...)		\
  do {							\
    if (conn->is_health_check)				\
      {							\
	int saved_errno = errno;			\
	_log_connection(__FILE__, __LINE__, __func__,	\
			CONN, FD,			\
			FMT, ##__VA_ARGS__);		\
	errno = saved_errno;				\
      }							\
  } while (0)

#endif
