// Main method in ths file, parse_payload, gets unstructured input (text),
// parses it into "struct payload". It sets appropriate function pointers.
//
// It is hard to write a clean parser with standard C. Parsing of unstructured
// text input requires lots of edge case/error checking. We shall use external
// libraries to handle that complexity - actually this approach just transfers
// complexity into the library's code, e.g. maintained by specialized
// open-source communities who have already navigated the minefield of
// edge-case handling, and memory safety.
//
// In later chapters, we will use external dependencies to write more clean
// parsers (http://github.com/metwse/rdesc)

#include "payload.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


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

static void message_constructor(struct payload *p, const char *raw)
{
	p->vtable = &message_vtable;

	// Nested polymorphism: each receiver is polymorphic!
	// They can be direct (@user), group (#channel), or global (no prefix)
	// Each receiver knows how to transmit and destroy itself shorthand
	// variable for receivers field
	struct message_receiving_entity *receivers;
	assert((receivers = malloc(sizeof(struct message_receiving_entity))));

	int receiver_count = 0;

	int content_offset = 0;
	while (raw[content_offset] == '@' || raw[content_offset] == '#') {
		int name_end;
		// assuming all payloads are valid (all of the payloads have
		// message content)
		for (name_end = content_offset;
		     raw[name_end] != ' '; name_end++);

		char *receiver_name = malloc(sizeof(char) *
					     (name_end - content_offset));

		for (int i = content_offset + 1; raw[i] != ' '; i++)
			receiver_name[i - content_offset - 1] = raw[i];

		receiver_name[name_end - content_offset - 1] = '\0';

		if (receiver_count >= 1) {
			assert((receivers = realloc(receivers,
				sizeof(struct message_receiving_entity) *
			        (receiver_count + 1))));
		}

		receivers[receiver_count] = (struct message_receiving_entity) {
			.additional_info = receiver_name,
			.vtable = \
				raw[content_offset] == '@' ?
				&direct_message_vtable : &group_message_vtable,
		};

		receiver_count++;

		content_offset = name_end + 1;
	}

	// fallback to global message if no receiver found
	if (content_offset == 0) {
		receivers->vtable = &global_message_vtable;
		receiver_count = 1;
	};


	assert((p->data.message.content =
		malloc(sizeof(char) * (strlen(raw) - content_offset + 1))));

	strcpy(p->data.message.content, raw + content_offset);
	p->data.message.receivers = receivers;
	p->data.message.receiver_count = receiver_count;
}

bool parse_payload(struct payload *p, const char *raw)
{
	// PAIN, but separated
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

			*p = (struct payload) {
				.vtable = &command_login_vtable,
				.data.command_login = {
					.username = username,
					.password = password
				}
			};
		} else if (strcmp("join", command_name) == 0) {
			char *channel;
			assert((channel = extract_token(raw + 6)));

			*p = (struct payload) {
				.vtable = &command_join_vtable,
				.data.command_join = {
					.channel = channel,
				}
			};
		} else if (strcmp("logout", command_name) == 0) {
			*p = (struct payload) {
				.vtable = &command_logout_vtable
			};
		} else {
			printf("Ignoring invalid command %s\n", command_name);
			return false;
		}
	} else {
		message_constructor(p, raw);
	}

	return true;
}
