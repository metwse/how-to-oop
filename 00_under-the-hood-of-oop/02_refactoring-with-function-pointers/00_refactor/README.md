# Refactored Solution: From Static to Dynamic Dispatch
This directory contains the refactored version of Exercise 01, demonstrating
how function pointers eliminate switch statements and improve code organization.

## What Changed from Exercise 01?
### 1. Removed the Type Enum
Exercise 01 (Static Dispatch):
```c
enum payload_kind {
    COMMAND_LOGIN, COMMAND_JOIN, COMMAND_LOGOUT,
    MESSAGE_DIRECT, MESSAGE_GROUP, MESSAGE_GLOBAL
};

struct payload {
    enum payload_kind kind;  // Type tag
    union payload_data data;
};
```

Exercise 02 (Dynamic Dispatch):
```c
struct payload {
    void (*process)(const struct payload *self);  // Behavior
    void (*destroy)(const struct payload *self);  // Cleanup
    union payload_data data;
};
```

*Impact:* No more type enumeration needed. Each payload carries its own
behavior through function pointers.

### 2. Eliminated Switch Statements for Destroy and Process
Exercise 01:
```c
void process_next(struct payload_buffer *buf) {
    struct payload *p = &buf->payloads[buf->process_base];

    switch (p->kind) {
        case COMMAND_LOGIN:
            printf("Command: login\n");
            printf("  Arguments: [username: %s, password: %s]\n",
                   p->data.command_login.username,
                   p->data.command_login.password);
            break;
        case COMMAND_JOIN:
        // ... more cases
        // ... 6 total cases
    }

    buf->process_base++;
}
```

Exercise 02:
```c
void process_next(struct payload_buffer *buf) {
    struct payload *p = &buf->payloads[buf->process_base];

    p->process(p);  // Dynamic dispatch!

    buf->process_base++;
}
```

*Impact:* From 30 lines with 6 cases to 3 lines. Each payload knows how to
process itself.

### 3. File Organization for Isolation
Exercise 01 Structure:
- `traditional_dispatch.h`: Everything (types, buffer, functions)
- `traditional_dispatch.c`: All logic mixed together

Exercise 02 Structure:
- `payload.h` / `payload.c`: Payload types and behaviors (process/destroy
   functions)
- `dynamic_dispatch.h` / `dynamic_dispatch.c`: Buffer management and
   orchestration

*Impact:* Better isolation between concerns. Payload behavior is separated from
buffer orchestration. We will elaborate isolation on later chapters, but here
is a quick explaination:

## What is Isolation?
Isolation means separating different concerns so changes in one area do not
ripple through unrelated areas.

In Exercise 01:
- Adding a new payload type required changes in multiple functions
- Buffer logic and payload logic were intertwined
- Everything depended on the shared enum

In Exercise 02:
- Payload behaviors are self-contained (each has its own process/destroy
  functions)
- Buffer management is isolated in `dynamic_dispatch.c`
- Adding a new payload type only requires:
  1. Create new process/destroy functions in `payload.c`
  2. Modify `push_payload()` to assign those functions
  3. Done! No changes to `process_next()` or `destroy()`

Benefits of isolation:
- Easier to understand: Each file has a single, clear purpose
- Safer to modify: Changes don't accidentally break unrelated code
- Better for teams: Different developers can work on payloads vs. buffer logic
  without conflicts

### Side Quest: Further Isolation
Currently, `push_payload()` does two things:
1. Parses the raw string and constructs a payload
2. Adds it to the buffer

**Challenge:** Extract parsing logic into a separate function:
```c
struct payload parse_payload(const char *raw);
```

This function should:
- Parse the input string
- Assign appropriate function pointers
- Return a constructed payload

Then `push_payload()` becomes simpler:
```c
void push_payload(struct payload_buffer *buf, const char *raw) {
    struct payload parsed;

    bool is_parsing_successful = parse_payload(&parsed, raw);  // Construct

    // ... add to buffer (if is_parsing_successful)
}
```

**Why?** Parsing logic is now isolated from buffer management. You can test
payload construction independently, and changing the buffer structure does not
affect parsing.

## Remaining Limitations
Even with function pointers, we still have problems:
1. Construction is still static: `push_payload()` still uses if/else to assign
   function pointers
2. Repetitive function pointers: Each payload struct stores two pointers
   (process + destroy)
3. **No type safety:** Nothing prevents mixing incompatible function pointers

**Next:** The [extended version](../01_extending-oop-project/) implements the
new requirements (multiple receivers, bulk commands) to further demonstrate the
benefits of dynamic dispatch.
