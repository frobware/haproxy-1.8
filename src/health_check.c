#include <proto/connection.h>

#include <assert.h>
#include "health_check.h"

static void current_time(char *s, size_t sz, const char *strftime_fmt, int *milliseconds)
{
	struct tm *tm_info;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	*milliseconds = tv.tv_usec / 1000.0;

	if (*milliseconds >= 1000) {
		*milliseconds -= 1000;
		tv.tv_sec++;
	}

	tm_info = localtime(&tv.tv_sec);
	strftime(s, sz, strftime_fmt, tm_info);
}

void __log_connection_attempt(const char *filename,
			      int linenumber,
			      const char *funcname,
			      struct connection *conn,
			      int fd,
			      const char *fmt, ...)
{
        char addr[INET6_ADDRSTRLEN] = { 'U', 'N', 'K', 'N', 'O', 'W', 'N', '\0' };
        int port = -1;
        char time_buffer[1024] = { '\0' };
        int milliseconds;

        va_list ap;

	assert(conn != NULL);
	assert(conn->addr.to.ss_family == AF_INET);

	current_time(time_buffer, sizeof(time_buffer) - 1, "%Y-%m-%d %H:%M:%S", &milliseconds);
	fprintf(stderr, "%s.%03d [pid %d] [%s:%d %s()]: ", time_buffer, milliseconds, getpid(), filename, linenumber, funcname);
	inet_ntop(AF_INET, &(((struct sockaddr_in *)&conn->addr.to)->sin_addr), addr, INET6_ADDRSTRLEN);
	port = ntohs(((struct sockaddr_in *)&conn->addr.to)->sin_port);
	fprintf(stderr, "HEALTHCHECK sockfd %d server %s:%d ", fd, addr, port);

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fputc('\n', stderr);
	fflush(stderr);
}
