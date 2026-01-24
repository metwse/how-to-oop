# Hold on! Lots of Repetition, Again!
Our code base has once again become impossible to maintain! This solution only
implements the first part of the task, multiple receivers. For function
batching, try it yourself!

## What Changed in This Solution?
This solution extends `00_refactor` with support for multiple receivers:

Example Input:
```
@alice @bob #general Hello everyone!
```

Output:
```
Direct message to alice: Hello everyone!
Direct message to bob: Hello everyone!
Group message to general: Hello everyone!
```

### Key Innovation: Nested Polymorphism
We introduced *two levels* of polymorphic behavior:

**Level 1 - Payloads:**
```c
struct payload {
    void (*process)(const struct payload *self);
    void (*destroy)(const struct payload *self);
    union payload_data data;
};
```

**Level 2 - Message Receivers:**
```c
struct message_receiving_entity {
    void (*transmit_message)(const struct message_receiving_entity *self,
                             const char *content);
    void (*destroy)(const struct message_receiving_entity *self);
    char *additional_info;
};
```

Each receiver is polymorphic! Direct messages, group messages, and global
messages all use the same interface but behave differently.

### Architecture
**File separation:**
- `payload_constructor.c` - Parsing and construction
- `payload_behaviors.c` - Behavioral implementations
- `dynamic_dispatch.c` - Orchestration (clean!)

This demonstrates composition: payloads contain arrays of polymorphic receivers.

## The Problem: Too Much Repetition!
Let's count the repetition we have encountered:

### 1. Manual Function Pointer Assignment
**For every payload type**, we manually assign function pointers:

```c
if (strcmp("login", command_name) == 0) {
    *p = (struct payload) {
        .process = process_command_login,  // manual assignment
        .destroy = destroy_command_login,  // manual assignment
        // ...
    };
} else if (strcmp("join", command_name) == 0) {
    *p = (struct payload) {
        .process = process_command_join,   // manual assignment again!
        .destroy = destroy_command_join,   // manual assignment again!
        // ...
    };
}
// ... repeat for EVERY type
```

### 2. Repetitive Function Declarations
For every message receiver type:
```c
void transmit_direct_message(const struct message_receiving_entity *self, ...);
void transmit_group_message(const struct message_receiving_entity *self, ...);
void transmit_global_message(const struct message_receiving_entity *self, ...);

void destroy_group_or_direct_message(const struct message_receiving_entity *self);
void destroy_global_message(const struct message_receiving_entity *self);
```

Every receiver needs its own function declarations!

### 3. No Type Safety
Nothing prevents this disaster:
```c
receivers[0] = (struct message_receiving_entity) {
    .transmit_message = transmit_direct_message,
    .destroy = destroy_global_message  // WRONG! Mixing behaviors!
};
```

The compiler will not complain, but this creates inconsistent objects.

### 4. Verbose Struct Initialization
Every construction site repeats the pattern:
```c
receivers[receiver_count] = (struct message_receiving_entity) {
    .additional_info = receiver_name,
    .transmit_message = /* pick one */,
    .destroy = /* pick one */
};
```

---

## Enter: Syntactic Sugar
As mentioned in [`introduction part`](../README.md), *syntactic sugar* is a
anguage feature that makes code easier to write without changing what it
actually does.

What we have been doing in C is writing OOP patterns *by hand*:
- Manually assigning function pointers
- Manually managing polymorphic behavior
- Manually ensuring consistency between related functions

What C++ provides is syntactic sugar for these patterns:
- Classes: Group data and functions together automatically
- Constructors: Initialize objects with correct function pointers automatically
- Virtual functions: Polymorphism without manual function pointer management
- Type safety: Compiler ensures you do not mix incompatible behaviors

## The Limitation We've Hit
Function pointers solve the processing problem (no more giant switch
statements!), but they create new problems:

1. Construction is verbose and error-prone
2. No compiler help to ensure consistency
3. Every new type requires manual wiring
4. Repetitive patterns everywhere

These are not fundamental limitations of OOP. they're limitations of
*implementing OOP manually in C*.

## Time to Switch to C++!
You have now experienced why OOP languages exist. They did not invent new
concepts - they automated the patterns we've been writing by hand.

Next steps:
1. Translate this exact solution to C++
2. See how classes eliminate the repetition
3. Understand that C++ is just syntactic sugar over what you've been doing

The core concepts remain the same. C++ just handles the plumbing for you.

---

But before switching to C++, we need to do
[a detailed discussion on dynamic dispatch](../../03_questions-arise/README.md).
