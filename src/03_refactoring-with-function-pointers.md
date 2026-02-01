term Static and Dynamic Dispatch
  : We called the enum-based approach "traditional dispatch" because it was the
    familiar starting point, not because it is a standard pattern. It is
    actually called *static dispatch*, the compiler determines which code to
    execute at compile time based on the enum value. With function pointers,
    you are using *dynamic dispatch*, the decision of which function to call is
    made at runtime based on the function pointer stored in the object. Both
    achieve the same goal (executing type-specific behavior), but dynamic
    dispatch provides more flexibility at the cost of a small runtime overhead.

# Refactoring with Function Pointers
## New Requirements
Multiple receivers: Messages can now target multiple users/channels
- Example: `@alice @bob #general Hello everyone!`
- A single message goes to alice, bob, AND the general channel

## Stop! Before You Continue...
**Question:** How would you implement these in your Ch02 code?

Think about it for a moment... **The painful reality:**
- You would need to rewrite `push_payload()` parsing logic, completely.
- The `payload_data` union would need a new receiver list structure.
- Multiple switch cases would change to handle arrays instead of single
  values.
- Processing logic would need loops for each receiver.
- Command parsing would need string splitting on semicolons.

In other words: you would need to touch almost every function, risking bugs
and spending hours debugging edge cases! *This is the moment* to ask: "Is there
a better way to organize my code?"

The answer is yes: *function pointers*.

## The Current Problem
Look at your code from Ch02. Notice how every time you want to add a new
payload type, you have to modify code in multiple places:

1. Add a new enum value to `payload_kind`
2. Add a new struct to the `payload_data` union
3. Add a parsing case in `push_payload()`
4. Add a processing case in `process_next()`
5. Add a cleanup case (if you are properly freeing memory)

This is called tight coupling. Everything is intertwined, one change ripples
through the entire codebase.

Imagine more future requirements, what if we need to add:
- **Timestamps** for each payload?
- **Priority levels** (urgent, normal, low)?
- **Validation** before processing?
- **Logging** when payloads are processed?
- **JSON export** of payloads?
- **Filtering** payloads by type?

With the current design, each feature requires changes in almost all functions.
*This does not scale!* Real applications have dozens or hundreds of types,
which requires continuous maintenance and extensions.

## The Function Pointer Solution
Instead of using switch statements everywhere, we can *store behavior with
data*, recall *function pointers*.

**Core Idea**: What if each payload could "know how to process itself"?
```c
// Traditional approach: separate data and behavior
switch (payload->kind) {
    case COMMAND_LOGIN:
        printf("Command: login\n");
        printf("  Arguments: [username: %s, password: %s]\n", ...);
        break;
    // ... more cases
}

// Function pointer approach: behavior stored with data
payload->process(payload);  // Each payload knows how to process itself!
```

## Your Mission
Refactor your Ch02 code to use *function pointers* instead of switch
statements, and then you will requested to implement features in "New
Requirements" section.

### New Design Principle "Dynamic Dispatch"
1. Remove the gigantic `payload_kind` enum:
   - No more type enumeration!
   - Each payload type is self-contained.
2. Add a function pointer to each payload:
   ```c
   struct payload {
       void (*process)(const struct payload *self);
       // ... data
   }
   ```
   We use the `self` reference because the process function is a standalone
   function that exists outside of the `struct payload` instance. We must
   explicitly pass the memory address of the struct so the function knows which
   specific object's data it should operate on.
3. Each payload type sets its own processing function:
   - Login command -> uses `process_login()` function
   - Direct message -> uses `process_direct_message()` function
   - ... etc.
4. Simplify `process_next()`:
   ```c
   void process_next(struct payload_buffer *buf) {
       struct payload *p = &buf->payloads[buf->process_base];
       p->process(p);  // That's it! No switch needed.
       buf->process_base++;
   }
   ```

### Benefits You will See
- No more repetitive switch statements in processing code.
- Behavior and data are together: Each payload is self-contained.
- Extensibility: Adding new operations (like `validate` or `to_json`) is
  straightforward.
- Separation of Concerns: Adding new types is easier, as it does not require
  lots of changes in process orchestrator. Just implement business logic in a
  function and assign it.

## What You will Learn
This exercise demonstrates the core principle of object-oriented programming:

> "Objects bundle data and behavior together, allowing them to be treated
> uniformly."

In C++/Java/Python, this is called *polymorphism* with virtual methods. In C,
we achieve the same thing with *function pointers*.

### Important Notes
- We are keeping this simple, just a few function pointers per payload.
- We are not building a full "virtual table" system yet. You will learn details
  of that in upcoming milestones.
- Focus on understanding how function pointers enable polymorphic behavior.
- Later exercises will show more sophisticated patterns.

## Hints
- Keep your existing `payload_data` union structure, simplify it by eliminating
  duplicate logic
- Add a `void (*process)(struct payload *self)` function pointer to
  `struct payload`
- Create separate `process_login()`, `process_direct_message()`, etc. functions
- In `push_payload()`, after parsing, assign the appropriate function pointer
- Your `process_next()` becomes much simpler!

## Expected Outcome
Your code should:
- Handle all the same payloads as Ch01.
- Have no switch statements in `process_next()`.
- Be easier to extend with new payload types.
- Demonstrate how function pointers enable polymorphism.

After completing this, you will understand why object-oriented languages use
virtual methods. They solve the exact same problem you just solved with
function pointers!

After completing the exercise on your own, inspect the notes on
[refactored](./03_solution_01.html) and
[extended with new requirements](./03_solution_02.html) solutions for
this chapter.

---

We complained about switch/case being repetitive, but now that we have defined
so many functions, is not this even more repetitive?

- Is it reasonable to store a pointer for each function inside the struct?
- *No type-safety.* What prevents conflicting behaviors (e.g., a command's
  `process` function but a message's `destructor`)?
- We still had problems in the old approach...

**Up Next:**
[A detailed discussion on dynamic dispatch](./04_questions-arise.html).
