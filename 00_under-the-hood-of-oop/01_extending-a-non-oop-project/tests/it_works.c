#include "../src/traditional_dispatch.h"

#include <stdlib.h>


int main()
{
	struct payload_buffer *buf = new_buffer();

	push_payload(buf, "/login metw password123");
	push_payload(buf, "/join channel123");
	push_payload(buf, "/logout");
	push_payload(buf, "#general test 123");
	push_payload(buf, "@metw hello");
	push_payload(buf, "global message");

	for (int i = 0; i < 6; i++) {
		process_next(buf);
	}

	destroy(buf);

	return EXIT_SUCCESS;
}
