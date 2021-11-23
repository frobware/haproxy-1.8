#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <assert.h>

#include "log_health.h"
#include <proto/connection.h>

/*
 * Returns current time formatted using %Y-%m-%dT%H:%M%S.000Z.
 *
 * For example "2021-12-24T14:20:22.257Z".
 */
char *_timenow_rfc8601(char buf[TIMENOW_RFC8601_SIZE])
{
	struct tm *tm_info;
	struct timeval tv;
	int milliseconds;
	char tmpbuf[TIMENOW_RFC8601_SIZE / 2];

	gettimeofday(&tv, NULL);
	milliseconds = tv.tv_usec / 1000.0;
	if (milliseconds >= 1000) {
		milliseconds -= 1000;
		tv.tv_sec++;
	}

	tm_info = localtime(&tv.tv_sec);
	strftime(tmpbuf, TIMENOW_RFC8601_SIZE/2, "%Y-%m-%dT%H:%M", tm_info);
	snprintf(buf, TIMENOW_RFC8601_SIZE, "%s.%03dZ", tmpbuf, milliseconds & 0x3e7);
	return buf;
}

void _log_connection(const char *filename,
		     int linenumber,
		     const char *funcname,
		     struct connection *conn,
		     int fd,
		     const char *fmt, ...)
{
	char addr[INET6_ADDRSTRLEN] = { 'U', 'N', 'K', 'N', 'O', 'W', 'N', '\0' };
	char ts[TIMENOW_RFC8601_SIZE] = { '\0' };
	int port = -1;
	va_list ap;

	assert(conn != NULL);
	assert(conn->addr.to.ss_family == AF_INET);

	fprintf(stderr, "%s [pid %d] [%s:%d %s] ", _timenow_rfc8601(ts), getpid(), filename, linenumber, funcname);
	inet_ntop(AF_INET, &(((struct sockaddr_in *)&conn->addr.to)->sin_addr), addr, INET6_ADDRSTRLEN);
	port = ntohs(((struct sockaddr_in *)&conn->addr.to)->sin_port);
	fprintf(stderr, "fd %d server %s:%d ", fd, addr, port);

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fputc('\n', stderr);
	fflush(stderr);
}
