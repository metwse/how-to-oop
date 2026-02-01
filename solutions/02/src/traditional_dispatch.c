#include "traditional_dispatch.h"

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

/* gigantic function parses every type of payload */
void push_payload(struct payload_buffer *buf, const char *raw)
{
	bool payload_valid = true;
	struct payload p;

	// PAIN
	if (raw[0] == '/') {
		char command_name[7];

		int i;

		for (i = 0; i < 6 && raw[i + 1] != ' '; i++)
			command_name[i] = raw[i + 1];

		command_name[i] = '\0';

		if (strcmp("login", command_name) == 0) {
			char *username, *password;
			// instead of error handling, this implementation
			// raises errors in case of extract failure
			assert((username = extract_token(raw + 7)));
			assert((password = extract_token(
				raw + strlen(username) + 8)));

			p = (struct payload) {
				.kind = COMMAND_LOGIN,
				.data.command_login = {
					.username = username,
					.password = password
				}
			};
		} else if (strcmp("join", command_name) == 0) {
			char *channel;
			assert((channel = extract_token(raw + 6)));

			p = (struct payload) {
				.kind = COMMAND_JOIN,
				.data.command_join = {
					.channel = channel,
				}
			};
		} else if (strcmp("logout", command_name) == 0) {
			p = (struct payload) {
				.kind = COMMAND_LOGOUT
			};
		} else {
			printf("Ignoring invalid command %s\n", command_name);
			payload_valid = false;
		}
	} else {
		// deduce message type by checking first character
		switch (raw[0]) {
		case '@':
			p.kind = MESSAGE_DIRECT;
			break;
		case '#':
			p.kind = MESSAGE_GROUP;
			break;
		default:
			p.kind = MESSAGE_GLOBAL;
		}

		char *receiver_name = "";

		// both group and direct messages have a receiver name
		if (p.kind != MESSAGE_GLOBAL)
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

			assert(buf->payloads);  // Raise an error if memory
						// allocation has been failed.
		}

		buf->payloads[buf->len++] = p;
	}
}

void process_next(struct payload_buffer *buf)
{
	assert(buf->process_base < buf->len);

	struct payload p = buf->payloads[buf->process_base];

	switch (p.kind) {
	case COMMAND_LOGIN:
		printf("Command: login\n"
		       "  Arguments: [username: %s, password %s]\n",
		       p.data.command_login.username,
		       p.data.command_login.password);
		break;
	case COMMAND_JOIN:
		printf("Command: join\n"
		       "  Arguments: [channel: %s]\n",
		       p.data.command_join.channel);
		break;
	case COMMAND_LOGOUT:
		printf("Command: logout\n"
		       "  Arguments: []\n");
		break;
	case MESSAGE_DIRECT:
		printf("Direct message to %s: %s\n",
		       p.data.message_direct.username,
		       p.data.message_direct.content);
		break;
	case MESSAGE_GROUP:
		printf("Group message to %s: %s\n",
		       p.data.message_group.channel,
		       p.data.message_group.content);
		break;
	case MESSAGE_GLOBAL:
		printf("Global message: %s\n",
		       p.data.message_global.content);
		break;
	}

	buf->process_base += 1;
}

void destroy(struct payload_buffer *buf)
{
	for (int i = 0; i < buf->len; i++) {
		struct payload p = buf->payloads[i];

		switch (p.kind) {
		case COMMAND_LOGIN:
			free(p.data.command_login.username);
			free(p.data.command_login.password);
			break;
		case COMMAND_JOIN:
			free(p.data.command_join.channel);
			break;
		case COMMAND_LOGOUT:
			break;
		case MESSAGE_DIRECT:
			free(p.data.message_direct.username);
			free(p.data.message_direct.content);
			break;
		case MESSAGE_GROUP:
			free(p.data.message_group.channel);
			free(p.data.message_group.content);
			break;
		case MESSAGE_GLOBAL:
			free(p.data.message_global.content);
			break;
		}
	}

	free(buf->payloads);
	free(buf);
}
