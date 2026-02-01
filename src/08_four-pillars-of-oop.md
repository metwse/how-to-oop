# The Four Pillars of OOP
You have spent the previous chapters manually building the mechanics of
object-oriented languages using C. You have seen that modern languages do not
invent new logic; they simply automate the tedious plumbing of pointers and
memory management. This chapter formally defines the four conceptual pillars
that support every OOP design.

Before diving into practical applications, let's consolidate what you have
learned by mapping your manual implementations to the standard OOP terminology.

## Encapsulation
term Encapsulation
  : The bundling of data and the functions that operate on that data into a
    single unit (a class or struct), while restricting direct access to some
    of the object's components.

You implemented encapsulation throughout this module, even before switching to
C++.

**In C (Ch03):** You bundled data and behavior by storing function
pointers alongside data in the same struct:
```c
struct payload {
    void (*process)(const struct payload *self);
    void (*destroy)(const struct payload *self);
    union payload_data data;
};
```

The `payload` struct encapsulated both state (`data`) and behavior (`process`,
`destroy`). This prevented scattering related logic across multiple files.

**In C++ (Ch05):** You formalized encapsulation with access specifiers:
```cpp
class String {
public:
    String(const char *str);  // Controlled interface
    ~String();
    const char *c_str() const;

private:
    char *data;  // Hidden from external access
};
```

By marking `data` as `private`, you enforced a critical rule: only the `String`
class itself can manage its memory. External code cannot accidentally corrupt
the pointer, preventing entire categories of bugs.

**Why?** Without encapsulation, your `char *data` would be public, and any code
could call `free(data)` or reassign the pointer. Encapsulation creates a
protective barrier around your resources.

## Abstraction
term Abstraction
  : The process of hiding implementation details and exposing only a simple,
    high-level interface to the user.

While encapsulation bundles data with behavior, abstraction focuses on *what*
the user sees versus *what* the object does internally. We do not deal with
*implementations*, with abstraction we interact with *ideas*.

**In C (Ch03):** Your `process_next()` function demonstrated abstraction:
```c
void process_next(struct payload_buffer *buf) {
    struct payload *p = &buf->payloads[buf->process_base];
    p->process(p);  // Abstract interface
    buf->process_base++;
}
```

The caller of `process_next()` does not know or care whether the payload is a
login command or a direct message. The function pointer `process` abstracts
away the implementation details. The interface is simple: "process this
payload." **In C++ (Ch06):** Virtual methods provide the same abstraction with
cleaner syntax:
```cpp
for (Payload *p : payloads) {
    p->process();  // Abstract interface
}
```

Each payload knows how to process itself, but the calling code is abstracted
from those details. You can add new payload types without changing the loop.

The key difference:
- **Encapsulation** answers: "How do I protect my data?"
- **Abstraction** answers: "How do I hide my complexity?"

Your `String` class encapsulates its `char *data` while abstracting the memory
management details behind `c_str()` and the destructor.

## Inheritance
term Inheritance
  : A mechanism where a class (derived/child) acquires properties and behaviors
    from another class (base/parent), enabling code reuse and establishing
    "Is-A" relationships.

**In C (Ch03):** Inheritance was manual and error-prone. You had to duplicate
the vtable structure across every payload type or use complex casting tricks to
simulate base types.

**In C++ (Ch06):** Inheritance became declarative, i.e. we have not required to
implement underlying polymorphism manually:
```cpp
class Command : public Payload {
public:
    void process() override {
        cout << "Command: " << command_name << endl;
        process_arguments();  // implemented in derived classes
    }

    virtual void process_arguments() = 0;

protected:
    string command_name;
};

class LoginCommand : public Command {
    // Inherits: command_name, process()
    // Implements: process_arguments()
};
```

`LoginCommand` *is a* `Command`, which *is a* `Payload`. This hierarchy
eliminates repetition: all commands share the `command_name` field and the
common `process()` logic.

**Type substitution:** Because `LoginCommand` inherits from `Payload`, you can
store it in a `Payload *` pointer:
```cpp
Payload *p = new LoginCommand("alice", "secret");
p->process();  // Calls LoginCommand's version
```

This is the "Is-A" relationship in action.

## Composition
term Composition
  : A design principle where complex objects are built by combining simpler
    ones, establishing "Has-A" relationships.

While inheritance models "Is-A," composition models "Has-A."

**Example from Ch05 (RAII):**
```cpp
class LoginCommand {
private:
    String username;  // LoginCommand *has a* String
    String password;  // LoginCommand *has a* String
};
```

`LoginCommand` is composed of `String` objects. It does not inherit from
`String`; it contains them.

**Composition in practice (Ch03):**
```c
struct payload_buffer {
    struct payload *payloads;  // Buffer *has* payloads
    size_t capacity;
    size_t size;
};
```

The buffer *has* payloads and *has* capacity tracking. These are distinct
concerns managed through composition.

**Guideline:** Prefer composition over inheritance when:
- The relationship is "Has-A" rather than "Is-A"
- You want to avoid tight coupling between classes
- You need flexibility to change parts independently

## Polymorphism
term Polymorphism
  : The ability to treat different types uniformly through a common interface
    while maintaining their unique behaviors.

You implemented polymorphism in three evolutionary stages:

**1. Static Dispatch (Ch01-02):** Decision made at compile-time using `enum`
tags:
```c
switch (payload.kind) {
    case COMMAND_LOGIN:
        process_login();
        break;
    case MESSAGE_DIRECT:
        process_direct_message();
        break;
}
```

The compiler generates separate code paths. No runtime flexibility and adding a
new type requires recompiling and modifying the switch statement.

**2. Manual Dynamic Dispatch (Ch03-04):** Runtime decision using function
pointers:
```c
struct payload {
    void (*process)(const struct payload *self);
};

p->process(p);  // Calls different function depending on runtime type
```

You manually assigned function pointers during construction. This was flexible
but error-prone: nothing stopped you from assigning the wrong function pointer.

**3. Automatic Dynamic Dispatch (Ch06):** C++ `virtual` keyword automates
vtable management:
```cpp
class Payload {
public:
    virtual void process() = 0;
};

class LoginCommand : public Payload {
public:
    void process() override { /* ... */ }
};

Payload *p = new LoginCommand(...);
p->process();  // Compiler generates vtable lookup
```

The compiler guarantees the correct function is called. Vtable assignment
happens automatically in constructors. The `override` keyword catches typos at
compile-time.

**The progression:** From explicit type checks (Ch01) -> manual vtables (Ch03)
-> automated vtables (Ch06). Same underlying mechanism, progressively less
manual work.

## Bringing It All Together
These four pillars are not independent features. They work together to enable
extensible designs.

**Example: Your Chapter 06 command hierarchy**
- **Encapsulation:** `Command` class hides `command_name` as `protected`
- **Abstraction:** `process()` provides a simple interface that hides the
  argument parsing complexity
- **Inheritance:** `LoginCommand` inherits common structure from `Command`
- **Polymorphism:** A `Payload *` can point to any command, and `process()`
  calls the correct implementation

**From manual to automatic:**
| Pillar | Manual (C) | Automatic (C++) |
|--------|-----------|-----------------|
| Encapsulation | Struct grouping | `private`/`protected`/`public` |
| Abstraction | Function pointers | Virtual methods |
| Inheritance | Manual struct layout | `class Derived : public Base` |
| Polymorphism | Manual vtable assignment | Compiler-generated vtables |

## What You Have Accomplished
You did not just learn C++ syntax. You built the fundamental patterns that all
object-oriented languages implement. These patterns exist in Java, Python, C#,
and every other OOP language. The syntax changes, but the underlying mechanics,
the mechanics you implemented by hand, remain the same.

---

With these theoretical foundations solidified, you are ready to transition from
"how objects work" to "how objects solve problems." In the next part, we will
apply these pillars to build a robust, real-world networking application.

You now understand what your compiler is doing behind the scenes. Let's build
something real.
