# Questions Arise
In Ch03, we eliminated giant switch statements using function pointers. But we
have created new problems and reached limits of manual polymorphism.

## The Paradox of Repetition
We complained about switch/case being repetitive. But look at what we wrote:

### Before (Switch Statement):
```c
switch (p->kind) {
    case COMMAND_LOGIN: /* ... */ break;
    case COMMAND_JOIN:  /* ... */ break;
    case MESSAGE_DIRECT: /* ... */ break;
    // ... 6 cases total
}
```

### After (Function Pointers):
```c
// Declaration repetition
void process_command_login(const struct payload *self);
void process_command_join(const struct payload *self);
void process_command_logout(const struct payload *self);
void process_message(const struct payload *self);
void destroy_command_login(const struct payload *self);
void destroy_command_join(const struct payload *self);
void destroy_command_logout(const struct payload *self);
void destroy_message(const struct payload *self);

// Assignment repetition
if (strcmp("login", command_name) == 0) {
    *p = (struct payload) {
        .process = process_command_login,
        .destroy = destroy_command_login,
        // ...
    };
} else if (strcmp("join", command_name) == 0) {
    *p = (struct payload) {
        .process = process_command_join,
        .destroy = destroy_command_join,
        // ...
    };
}
// ... endless if/else
```

Question: Did we just trade one type of repetition for another?

## The Type Safety Problem
Nothing prevents this disaster:
```c
struct payload broken = {
    .process = process_command_login,  // Command behavior
    .destroy = destroy_message,        // Message behavior!
};
```

The compiler won't complain. At runtime:
- `process` expects `data.command_login.username`
- `destroy` expects `data.message.receivers`
- **Result:** Memory corruption, segfault, undefined behavior

Question: How reasonable is it to store multiple function pointers in a struct
when there is no mechanism to ensure they are consistent?

## The Structural Repetition Problem
Every payload carries two function pointers:

```c
struct payload {
    void (*process)(const struct payload *self);  // 8 bytes
    void (*destroy)(const struct payload *self);  // 8 bytes
    union payload_data data;
};
```

For 1000 messages, we store 16,000 bytes of function pointers. But here is the
thing: All messages of the same type have THE SAME function pointers!

```c
payload[0] = { .process = process_message, .destroy = destroy_message, ... };
payload[1] = { .process = process_message, .destroy = destroy_message, ... };
payload[2] = { .process = process_message, .destroy = destroy_message, ... };
// ... all 1000 messages store identical pointers
```

Question: Why are we duplicating the same pointers in every instance?

---

## Enter: The Virtual Function Table (vtable)
The solution is to share function pointers across instances of the same type.

**Concept: Separate Type Information from Instance Data**\
Before (current approach):
```
Each payload = [process*, destroy*, data]
                ^^^^^^^^^^^^^^^  stored per-instance
```

After (vtable approach):
```
Each payload = [vtable*, data]
                ^^^^^^^  single pointer to shared table

vtable_message = [process_message*, destroy_message*]
                  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  shared by ALL messages
```

**C Implementation of vtables:**
```c
// Step 1: Define the vtable structure
struct payload_vtable {
    void (*process)(const struct payload *self);
    void (*destroy)(const struct payload *self);
};

// Step 2: Create static vtables (one per type)
const struct payload_vtable vtable_command_login = {
    .process = process_command_login,
    .destroy = destroy_command_login
};

const struct payload_vtable vtable_message = {
    .process = process_message,
    .destroy = destroy_message
};

// Step 3: Payload now stores a vtable pointer
struct payload {
    const struct payload_vtable *vtable;  // Single pointer!
    union payload_data data;
};

// Step 4: Usage
payload->vtable->process(payload);  // Dynamic dispatch!

// ... same steps for receivers
```

Using vtables, we reduced pointers to one pointer per instance instead of N
pointers. Also, a partial type safety is ensured as all functions for a type
live in one vtable.

### Remaining Problems
1. Still manual: We still write if/else to assign vtables
2. No enforcement: Can still assign wrong vtable to wrong data
3. Constructor repetition: Every type needs init code

## What Have We Learned?
Function pointers solve the processing problem but create new problems:

| Problem | Solution | New Problem Created |
|---------|----------|---------------------|
| Giant switch statements | Function pointers in structs | Repetitive declarations |
| Adding new types modifies many places | Each type self-contained | Verbose construction |
| Type/behavior coupling | Dynamic dispatch | No type safety |
| - | - | Memory overhead (partially solved by vtables) |

The pattern: Each solution introduces new complexity.

## The Fundamental Limitation
We have been manually implementing OOP patterns in C. This works, but it is
*verbose* - requires lots of boilerplate.

C++ doesn't invent new concepts. It automates what we have been doing. Recall
*syntactic sugar!*

| What We Did Manually | C++ Keyword |
|----------------------|-------------|
| `struct payload` with function pointers | `class` |
| Assigning function pointers in constructor | Constructor automatically sets up vtable |
| Our `vtable` struct | Compiler-generated vtable (hidden) |
| `payload->vtable->process(payload)` | `payload->process()` (syntactic sugar) |
| Manual type safety checks | Compiler enforces at compile-time |

## Objective
Implement vtables in C. Refactor Ch03 to use vtables. You should:

1. Define `struct payload_vtable` & `struct message_receiving_entity_vtable`
2. Create vtables for each payload & receiver type
3. Change `struct payload` & `struct message_receiving_entity_` to use a
   single vtable pointer
4. Update construction logic to assign the correct vtable
5. Change `process_next()` to call through vtable

After completing this, you will understand exactly what C++ `virtual` functions
do under the hood.

---

You now have implemented:

1. Polymorphism with enums + switch (Ch01-02)
2. Polymorphism with function pointers (Ch03)
3. Polymorphism with vtables (Ch04)

We will translate Ch03 directly to C++ and see:
- How `class` eliminates manual vtable management
- How constructors automate function pointer assignment
- How `virtual` provides type-safe polymorphism
- How inheritance shares behavior without code duplication

OOP languages exist because manual polymorphism is tedious and error-prone.
Every limitation we have hit is a feature C++ automates. You are ready to
appreciate what C++ actually does.

A brief detour from polymorphism: Before discussing C++ polymorphism features,
we should first cover how resources are managed using [RAII](./05_raii.html).
