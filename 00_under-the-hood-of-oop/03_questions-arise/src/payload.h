/**
 * @file payload.h
 * @brief Payload methods.
 */


#ifndef PAYLOAD_H
#define PAYLOAD_H


#include <stdbool.h>


struct message_receiving_entity {
	const struct message_receiving_entity_vtable *vtable;
	char *additional_info;
};

struct message_receiving_entity_vtable {
	void (*transmit_message)(const struct message_receiving_entity *self,
				 const char *content);
	void (*destroy)(const struct message_receiving_entity *self);
};

union payload_data {
	struct {
		char *username;
		char *password;
	} command_login;

	struct {
		char *channel;
	} command_join;

	struct {
		struct message_receiving_entity *receivers;
		char *content;
		int receiver_count;
	} message;
};

struct payload {
	const struct payload_vtable *vtable;
	union payload_data data;
};

struct payload_vtable {
	void (*process)(const struct payload *self);
	void (*destroy)(const struct payload *self);
};


bool parse_payload(struct payload *p, const char *raw);


/* payload vtables */
extern const struct payload_vtable command_login_vtable;
extern const struct payload_vtable command_join_vtable;
extern const struct payload_vtable command_logout_vtable;
extern const struct payload_vtable message_vtable;

/* receiver vtables */
extern const struct message_receiving_entity_vtable direct_message_vtable;
extern const struct message_receiving_entity_vtable group_message_vtable;
extern const struct message_receiving_entity_vtable global_message_vtable;


#endif
