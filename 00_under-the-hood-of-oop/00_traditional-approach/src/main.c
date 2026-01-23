#include "traditional_dispatch.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, const char **args)
{
	if (argc != 2) {
		fprintf(stderr, "Invalid number of arguments. Perhaps, you "
			        "forgot providing argument.\n");

		return EXIT_FAILURE;
	}

	FILE *file = fopen(args[1], "r");

	if (file == NULL) {
		fprintf(stderr, "Could not open %s.\n", args[1]);

		return EXIT_FAILURE;
	}

	char line[1024];  // Assuming no line exceed 1023 characters

	while (fgets(line, 1024, file) != NULL) {
		int line_len = strlen(line);
		if (line_len < 2)
			continue;

		line[line_len - 1] = '\0';  // Remove newline character at
					    // the end of the line.
		handle_payload(line);
	}

	return EXIT_SUCCESS;
}
