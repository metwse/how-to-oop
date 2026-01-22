#include "../src/some_utility.h"

#include <assert.h>
#include <stdlib.h>


int main()
{
	assert(add(2, 2) == 4);
	assert(add(1, 2) == 3);

	return EXIT_SUCCESS;
}
