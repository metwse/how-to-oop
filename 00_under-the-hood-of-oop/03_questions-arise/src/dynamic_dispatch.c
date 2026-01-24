#include "dynamic_dispatch.h"
#include "payload.h"

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>


struct payload_buffer *new_buffer()
{
	struct payload_buffer *buf = malloc(sizeof(struct payload_buffer));
	assert(buf);

	buf->process_base = buf->len = 0;
	buf->cap = 1;
	buf->payloads = malloc(sizeof(struct payload));
	assert(buf->payloads);

	return buf;
}

void push_payload(struct payload_buffer *buf, const char *raw)
{
	struct payload parsed;

	bool is_parsing_successful = parse_payload(&parsed, raw);

	if (is_parsing_successful) {
		if (buf->cap == buf->len) {
			buf->cap *= 2;
			buf->payloads = realloc(buf->payloads,
			   buf->cap * sizeof(struct payload));

			assert(buf->payloads);
		}

		buf->payloads[buf->len++] = parsed;
	}
}

void process_next(struct payload_buffer *buf)
{
	assert(buf->process_base < buf->len);

	struct payload *p = &buf->payloads[buf->process_base];
	p->vtable->process(p);

	buf->process_base += 1;
}

void destroy(struct payload_buffer *buf)
{
	for (int i = 0; i < buf->len; i++) {
		struct payload *p = &buf->payloads[i];
		p->vtable->destroy(p);
	}

	free(buf->payloads);
	free(buf);
}
