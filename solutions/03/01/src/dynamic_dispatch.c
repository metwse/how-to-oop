#include "dynamic_dispatch.h"
#include "payload.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* extract string until the next space */
static char *extract_token(const char *raw) {
	int end;
	char *token;

	for (end = 0; raw[end] != ' ' && raw[end] != '\0'; end++);

	if (end == 0) {
		return NULL;
	} else {
		token = malloc(sizeof(char) * (end + 1));
		assert(token);
		token[end] = '\0';

		for (int i = 0; i < end; i++)
			token[i] = raw[i];

		return token;
	}
}

/* gigantic function parses every type of payload
 *
 * NOTE: We made process and cleanup methods dynamic (via function pointers),
 * but we are still constructing payloads using static dispatch
 * (switch/if-else). Setting up function tables dynamically is the next level
 * of abstraction, which we will introduce in upcoming exercises.
 */
void push_payload(struct payload_buffer *buf, const char *raw)
{
	bool payload_valid = true;
	struct payload p;

	// still PAIN
	if (raw[0] == '/') {
		char command_name[7];

		int i;

		for (i = 0; i < 6 && raw[i + 1] != ' '; i++)
			command_name[i] = raw[i + 1];

		command_name[i] = '\0';

		if (strcmp("login", command_name) == 0) {
			char *username, *password;
			assert((username = extract_token(raw + 7)));
			assert((password = extract_token(
				raw + strlen(username) + 8)));

			p = (struct payload) {
				.process = process_command_login,
				.destroy = destroy_command_login,
				.data.command_login = {
					.username = username,
					.password = password
				}
			};
		} else if (strcmp("join", command_name) == 0) {
			char *channel;
			assert((channel = extract_token(raw + 6)));

			p = (struct payload) {
				.process = process_command_join,
				.destroy = destroy_command_join,
				.data.command_join = {
					.channel = channel,
				}
			};
		} else if (strcmp("logout", command_name) == 0) {
			p = (struct payload) {
				.process = process_command_logout,
				.destroy = destroy_command_logout,
			};
		} else {
			printf("Ignoring invalid command %s\n", command_name);
			payload_valid = false;
		}
	} else {
		// deduce message type by checking first character
		switch (raw[0]) {
		case '@':
			p.process = process_message_direct;
			p.destroy = destroy_message_direct;
			break;
		case '#':
			p.process = process_message_group;
			p.destroy = destroy_message_group;
			break;
		default:
			p.process = process_message_global;
			p.destroy = destroy_message_global;
		}

		char *receiver_name = "";

		// both group and direct messages have a receiver name
		if (p.process != process_message_global)
			assert((receiver_name = extract_token(raw + 1)));

		int content_len = strlen(raw) - strlen(receiver_name);

		char *content = malloc(sizeof(char) * (content_len + 1));
		assert(content);

		if (receiver_name[0] != '\0')
			strcpy(content, raw + strlen(receiver_name) + 2);
		else
			strcpy(content, raw);

		switch (raw[0]) {
		case '@':
			p.data.message_direct.content = content;
			p.data.message_direct.username = receiver_name;
			break;
		case '#':
			p.data.message_group.content = content;
			p.data.message_group.channel = receiver_name;
			break;
		default:
			p.data.message_global.content = content;
		}
	}

	if (payload_valid) {
		if (buf->cap == buf->len) {
			buf->cap *= 2;
			buf->payloads = realloc(buf->payloads,
			   buf->cap * sizeof(struct payload));

			assert(buf->payloads);
		}

		buf->payloads[buf->len++] = p;
	}
}

void process_next(struct payload_buffer *buf)
{
	assert(buf->process_base < buf->len);

	struct payload *p = &buf->payloads[buf->process_base];
	p->process(p);

	buf->process_base += 1;
}

void destroy(struct payload_buffer *buf)
{
	for (int i = 0; i < buf->len; i++) {
		struct payload *p = &buf->payloads[i];
		p->destroy(p);
	}

	free(buf->payloads);
	free(buf);
}

