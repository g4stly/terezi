#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void die(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
			fputc(' ', stderr);
			perror(NULL);
	}

	exit(-1);
}

char *readline()
{
	char c;
	int i = 0;
	size_t len = 8 * sizeof(char);

	char *buffer = malloc(len);
	if (!buffer) die("readline():");
	memset(buffer, 0, len);

	while ((c = getc(stdin)) != '\n') {
		if (i > len - 2) {
			len += len;
			buffer = realloc(buffer, len);
			if (!buffer) die("readline():");
			memset(buffer + (len / 2), 0, len / 2);
		}
		buffer[i++] = c;
	}

	if (strlen(buffer)) return buffer;
	free(buffer);
	return NULL;
}

