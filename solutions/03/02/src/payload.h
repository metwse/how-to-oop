/**
 * @file payload.h
 * @brief Payload methods.
 */


#ifndef PAYLOAD_H
#define PAYLOAD_H


#include <stdbool.h>


struct message_receiving_entity {
	void (*transmit_message)(const struct message_receiving_entity *self,
				 const char *content);
	void (*destroy)(const struct message_receiving_entity *self);
	char *additional_info;
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
	void (*process)(const struct payload *self);
	void (*destroy)(const struct payload *self);
	union payload_data data;
};


/**
 * @brief Constructor method sets up function pointers and data fields of
 *        payloads.
 *
 * @param p Output for parsed payload
 * @param raw Raw payload string to parse
 */
bool parse_payload(struct payload *p, const char *raw);

/* process strategies */
void process_command_login(const struct payload *self);
void process_command_join(const struct payload *self);
void process_command_logout(const struct payload *self);
void process_message(const struct payload *self);

/* transmit strategies */
void transmit_group_message(const struct message_receiving_entity *self,
			    const char *content);
void transmit_direct_message(const struct message_receiving_entity *self,
			    const char *content);
void transmit_global_message(const struct message_receiving_entity *self,
			    const char *content);

/* payload cleanup strategies */
void destroy_command_login(const struct payload *self);
void destroy_command_join(const struct payload *self);
void destroy_command_logout(const struct payload *self);
void destroy_message(const struct payload *self);

/* message cleanup strategies */
void destroy_group_or_direct_message(const struct message_receiving_entity *self);
void destroy_global_message(const struct message_receiving_entity *self);


#endif
