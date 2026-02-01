/**
 * @file dynamic_dispatch.h
 * @brief Solution for milestone 02, refactor part.
 */


#ifndef DYNAMIC_DISPATCH_H
#define DYNAMIC_DISPATCH_H


struct payload_buffer {
	struct payload *payloads;
	int len;
	int cap;
	int process_base;
};


struct payload_buffer *new_buffer();

void push_payload(struct payload_buffer *buf, const char *raw);

void process_next(struct payload_buffer *buf);

void destroy(struct payload_buffer *buf);


#endif
