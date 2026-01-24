#include "../src/payload.h"

#include <stddef.h>
#include <stdlib.h>


const char *TEST_PAYLOADS[] = {
	"@alice @bob Hello everyone!",
	"#general #random Check this out!",
	"Global message to all",
};

int main()
{
	struct payload p;

	for (size_t i = 0; i < sizeof(TEST_PAYLOADS) / sizeof(char *); i++) {
		parse_payload(&p, "@metw #general test");

		p.destroy(&p);
	}

	return EXIT_SUCCESS;
}
