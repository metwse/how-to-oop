// "behavioral" functions

#include "payload.h"

#include <stdio.h>
#include <stdlib.h>


void process_command_login(const struct payload *self)
{
	printf("Command: login\n"
	       "  Arguments: [username: %s, password %s]\n",
	       self->data.command_login.username,
	       self->data.command_login.password);
}

void process_command_join(const struct payload *self)
{
	printf("Command: join\n"
	       "  Arguments: [channel: %s]\n",
	       self->data.command_join.channel);
}

void process_command_logout([[maybe_unused]] const struct payload *self)
{
	printf("Command: logout\n"
	       "  Arguments: []\n");
}

void process_message(const struct payload *self)
{
	struct message_receiving_entity *receivers = \
		self->data.message.receivers;

	for (int i = 0; i < self->data.message.receiver_count; i++)
		receivers[i].vtable->transmit_message(&receivers[i],
						      self->data.message.content);
}

void transmit_direct_message(const struct message_receiving_entity *self,
			     const char *content)
{
	printf("Direct message to %s: %s\n", self->additional_info, content);
}

void transmit_group_message(const struct message_receiving_entity *self,
			    const char *content)
{
	printf("Group message to %s: %s\n", self->additional_info, content);
}

void transmit_global_message([[maybe_unused]] const struct message_receiving_entity *self,
			     const char *content)
{
	printf("Global message: %s\n", content);
}


void destroy_command_login(const struct payload *self)
{
	free(self->data.command_login.username);
	free(self->data.command_login.password);
}

void destroy_command_join(const struct payload *self)
{
	free(self->data.command_join.channel);
}

void destroy_command_logout([[maybe_unused]] const struct payload *self)
{}

void destroy_message(const struct payload *self)
{
	struct message_receiving_entity *receivers = \
		self->data.message.receivers;

	for (int i = 0; i < self->data.message.receiver_count; i++)
		receivers[i].vtable->destroy(&receivers[i]);

	free(self->data.message.content);
	free(receivers);
}

void destroy_global_message([[maybe_unused]] const struct message_receiving_entity *self)
{}

void destroy_group_or_direct_message(const struct message_receiving_entity *self)
{
	free(self->additional_info);
}


/* payload vtables */
const struct payload_vtable command_login_vtable = {
	.process = process_command_login,
	.destroy = destroy_command_login,
};

const struct payload_vtable command_join_vtable = {
	.process = process_command_join,
	.destroy = destroy_command_join,
};

const struct payload_vtable command_logout_vtable = {
	.process = process_command_logout,
	.destroy = destroy_command_logout,
};

const struct payload_vtable message_vtable = {
	.process = process_message,
	.destroy = destroy_message,
};

/* receiver vtables */
const struct message_receiving_entity_vtable direct_message_vtable = {
	.transmit_message = transmit_direct_message,
	.destroy = destroy_group_or_direct_message,
};

const struct message_receiving_entity_vtable group_message_vtable = {
	.transmit_message = transmit_group_message,
	.destroy = destroy_group_or_direct_message,
};

const struct message_receiving_entity_vtable global_message_vtable = {
	.transmit_message = transmit_global_message,
	.destroy = destroy_global_message,
};
