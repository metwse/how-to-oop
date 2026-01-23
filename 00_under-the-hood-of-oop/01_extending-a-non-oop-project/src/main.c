#include "traditional_dispatch.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main([[maybe_unused]] int argc, const char **args)
{
	/* I omit error handling for reading from file, assuming the program
	 * run with correct number of arguments and a valid payloads.txt */

	struct payload_buffer *buf = new_buffer();

	FILE *file = fopen(args[1], "r");

	char line[1024];  // Assuming no line exceed 1023 characters

	printf("--- Reading payloads ---\n");
	while (fgets(line, 1024, file) != NULL) {
		int line_len = strlen(line);
		if (line_len < 2)
			continue;

		line[line_len - 1] = '\0';  // Remove newline character at
					    // the end of the line.

		push_payload(buf, line);
	}
	printf("Read %d payloads\n\n", buf->len);

	fclose(file);

	printf("--- Processing payloads ---\n");
	for (int i = 0; i < buf->len; i++) {
		printf("Processing payload %d of %d\n", i, buf->len);

		process_next(buf);

		printf("\n");
	}

	destroy(buf);

	return EXIT_SUCCESS;
}
