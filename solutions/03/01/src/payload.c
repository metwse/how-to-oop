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
void process_message_direct(const struct payload *self)
{
	printf("Direct message to %s: %s\n",
	       self->data.message_direct.username,
	       self->data.message_direct.content);
}
void process_message_group(const struct payload *self)
{
	printf("Group message to %s: %s\n",
	       self->data.message_group.channel,
	       self->data.message_group.content);
}
void process_message_global(const struct payload *self)
{
	printf("Global message: %s\n",
	       self->data.message_global.content);
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

void destroy_message_direct(const struct payload *self)
{
	free(self->data.message_direct.username);
	free(self->data.message_direct.content);
}

void destroy_message_group(const struct payload *self)
{
	free(self->data.message_group.channel);
	free(self->data.message_group.content);
}

void destroy_message_global(const struct payload *self)
{
	free(self->data.message_global.content);
}
