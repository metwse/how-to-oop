#include "traditional_dispatch.h"

#include <stdio.h>
#include <string.h>


void handle_payload(char *raw)
{
	char first_char = raw[0];
	char *content, *receiver;

	switch (first_char) {
	case '/':
		printf("Command: %s\n", strtok(raw, " "));
		break;
	case '@':
	case '#':
		receiver = strtok_r(&raw[1] /* strip @ or # */, " ",
				    &content);
		printf("Direct message to %s: %s\n",
		       receiver,
		       content);
		break;
	default:
		printf("Global message: %s\n", raw);
	}
}
