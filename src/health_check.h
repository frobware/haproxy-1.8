#ifndef inc_HEALTH_CHECK
#define inc_HEALTH_CHECK

#include <stdio.h>
#include <stdarg.h>

struct connection;

extern void __log_connection_attempt(const char *filename,
				     int linenumber,
                                     const char *funcname,
                                     struct connection *conn,
				     int sockfd,
				     const char *fmt,
                                     ...)
  __attribute__((format(printf, 6, 7)));

#define LOG_CONNECTION(CONN, FD, FMT, ...)			\
  do {								\
    int saved_errno = errno;					\
    __log_connection_attempt(__FILE__, __LINE__, __func__,	\
			     CONN, FD, FMT, ##__VA_ARGS__);	\
    errno = saved_errno;					\
  } while (0)

#define LOG_HCHK_CONNECTION(CONN, FD, FMT, ...)			\
  do {								\
    if (conn->xaaa_is_health_check)				\
      {								\
	int saved_errno = errno;				\
	__log_connection_attempt(__FILE__, __LINE__, __func__,	\
				 CONN, FD, FMT, ##__VA_ARGS__);	\
	errno = saved_errno;					\
      }								\
  } while (0)

#endif	/* inc_HEALTH_CHECK */

	    
