# C Project Template
A C project template designed for learning systems programming. This template
provides a complete development environment with building, testing, and
documentation commands pre-configured.

## Quick Start
```sh
make             # Build main executable
./target/main    # Run the program
make tests       # Build tests
./target/*.test  # Run tests
make docs        # Generate documentation
```

Use `make help` for complete list of commands.


## Project Structure
The *source code* is housed in `src/`, which contains all your `.c` and
`.h` files. `main.c` is program's entry point, and `some_utility.c/h` provided
as an example module.

The *testing* environment is located in the `tests/` directory and contains test
suites. Each test within this folder is a standalone program with its own
`main()` function, allowing you to verify parts of your project in isolation
without the need to execute the entire program.

The *build outputs* are in `target/`.
- `target/main` main executable
- `target/*.test` test executables

The *documentation* folder, `docs/`, is intended for documentation
auto-generated from code comments. While you are encouraged to learn and use
documentation tools on your own, documenting your project is entirely optional
and is considered out of scope for this specific course.


## Quick Recipe for Starting a New Project
1. **Copy this template** to a new directory
2. **Update** [Doxyfile](./Doxyfile), change `PROJECT_NAME` to your project
   name
3. **Write your code** in `src/`, rename/replace `some_utility.c/h` with your
   own modules
4. **Write tests** in `tests/` (name them `<module>.c`), build them using
   `make tests`, and run them via `./target/<module>.test`
5. **Build and run program** `make && ./target/main`

You can use `gdb` for debugging and `valgrind` for memory checks.


## Testing
### Running Tests
```sh
make tests              # Build all tests
./target/it_works.test  # Run a specific test
```

### Writing Tests
Create a new file in `tests/` named `<module>.c`:
```c
#include "../src/your_module.h"

#include <assert.h>
#include <stdlib.h>


int main() {
    assert(your_function(10) == 20);
    assert(another_function() != NULL);

    return EXIT_SUCCESS;
}
```

Tests use assert(). If an assertion fails, the program crashes with an error.


## Extra: Build Settings
The Makefile copiles using flags:
- `-std=gnu17` Modern C standard with GNU extensions (We will use GNU
  extensions for networking)
- `-Wall -Wextra` Enable warnings to catch bugs
- `-O0 -g3` No optimization + full debug symbols (for gdb)
- `-lm` Link math library (`math.h`)
