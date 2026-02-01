#include "payload.hpp"

// As a convention, we do not use .h for C standard library headers. To include
// standard C headers, omit .h from and add c prefix to header name.
#include <cstdlib>
#include <cstdio>
#include <cstring>


int main([[maybe_unused]] int argc, const char **args) {
    FILE *file = fopen(args[1], "r");

    if (file == NULL) {
        fprintf(stderr, "Could not open %s.\n", args[1]);

        return EXIT_FAILURE;
    }

    char line[1024];

    while (fgets(line, 1024, file) != NULL) {
        int line_len = strlen(line);
        if (line_len < 2)
            continue;

        line[line_len - 1] = '\0';

        handle_command_payload(line);
    }

    fclose(file);

    return EXIT_SUCCESS;
}
