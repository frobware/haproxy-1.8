#include <assert.h>
#include <proto/connection.h>
#include "health_check.h"

/*
 * Returns current time formatted using %Y-%m-%dT%H:%M%S.000Z.
 *
 * For example "2021-12-24T14:20:22.257Z".
 */
char *_timenow_rfc8601(char buf[32])
{
	struct tm *tm_info;
	struct timeval tv;
	int milliseconds;

	gettimeofday(&tv, NULL);
	milliseconds = tv.tv_usec / 1000.0;
	if (milliseconds >= 1000) {
		milliseconds -= 1000;
		tv.tv_sec++;
	}

	tm_info = localtime(&tv.tv_sec);
	strftime(buf, 32, "%Y-%m-%dT%H:%M:%S.000Z", tm_info);
	snprintf(&buf[20], 5, "%03dZ", milliseconds);

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
	int port = -1;
	char ts[32] = { '\0' };

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
