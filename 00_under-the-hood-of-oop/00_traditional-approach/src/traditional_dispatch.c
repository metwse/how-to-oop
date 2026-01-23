#include "traditional_dispatch.h"

#include <stdio.h>
#include <string.h>


void handle_payload(char *payload)
{
	char first_char = payload[0];
	char *content, *receiver;

	switch (first_char) {
	case '/':
		printf("Command: %s\n", strtok(payload, " "));
		break;
	case '@':
	case '#':
		receiver = strtok_r(&payload[1] /* strip @ or # */, " ",
				    &content);
		printf("Direct message to %s: %s\n",
		       receiver,
		       content);
		break;
	default:
		printf("Global message: %s\n", payload);
	}
}
