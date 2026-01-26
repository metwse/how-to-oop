> `\begin{aside}`\
> *Resource acquisition is initialization (RAII)* describe a particular
> language behavior for managing resources. In RAII, holding a resource is a
> *class invariant*, and is tied to *object lifetime*. Resource allocation
> (or acquisition) is done during object creation (specifically
> initialization), by the constructor, while resource deallocation (release)
> is done during object destruction (specifically finalization), by the
> destructor. In other words, resource acquisition must succeed for
> initialization to succeed. Thus, the resource is guaranteed to be held
> between when initialization finishes and finalization starts (holding the
> resources is a class invariant), and to be held only when the object is
> alive. Thus, if there are no object leaks, there are no resource leaks.
> *[Wikipedia]*\
> `\end{aside}`

We have been implementing RAII principle manually:
1. `parse_payload()` construct payload object - that is *data + behavior*,
   and allocate resources.
2. Resources held during *object lifetime* (until buffer is destroyed via
   `buffer_destroy`)
3. Destructor methods deallocate resources used by payload objects.

In this chapter, we will inspect how C++ *syntactic sugar* automates this
steps.

# From Manual Memory Management to RAII
You have built manual polymorphism in C. You have seen how verbose it is to
initialize and clean up resources. Now let's see how C++ automates this with
*constructors* and *destructors*.

## The Problem We Had in C
Initialization:
```c
struct payload *p = malloc(sizeof(struct payload));
assert(p);
p->vtable = &vtable_message;
p->data.message.content = malloc(strlen(raw) + 1);
strcpy(p->data.message.content, raw);
// ... more initialization
```

Cleanup:
```c
p->vtable->destroy(p);  // Must remember to call!
free(p);
```

What could go wrong?
- Forget to initialize a field - *undefined behavior*
- Forget to call `destroy()` - *memory leak*
- Call `destroy()` twice - *double-free*
- Use after `free()` - *use-after-free*

## Before Switching to C++...
To familiarize yourself with C++ syntax, take a quick glance to links:

1. [Output (Print Text)](https://www.w3schools.com/cpp/cpp_output.asp)
2. [New and Delete](https://www.w3schools.com/cpp/cpp_memory_management_new.asp)
3. [Classes/Objects](https://www.w3schools.com/cpp/cpp_classes.asp)
4. [Class Methods](https://www.w3schools.com/cpp/cpp_class_methods.asp)
5. [Access Specifiers](https://www.w3schools.com/cpp/cpp_access_specifiers.asp)

## Implementing RAII: String Class
Let's build a RAII class from scratch using `char *`.

### String Class with Manual Memory Management
```cpp
class String {
private:
    char *data;

public:
    // Constructor: Acquire resource (allocate memory)
    String(const char *str) {
        data = new char[strlen(str) + 1];  // Allocate
        strcpy(data, str);
        std::cout << "String created: " << data << "\n";
    }

    // Destructor: Release resource (free memory)
    ~String() {
        std::cout << "String destroyed: " << data << "\n";
        delete[] data;  // Free automatically!
    }

    const char* c_str() const { return data; }
};
```

### Usage:
```cpp
{
    String username("alice");
    String message("Hello!");
    std::cout << "Processing: " << username.c_str() << "\n";
}  // Destructors called automatically here!
```

### Output:
```
String created: alice
String created: Hello!
Processing: alice
String destroyed: Hello!     ← Reverse order!
String destroyed: alice      ← Reverse order!
```

### What C++ Does Behind the Scenes:
```c
// Equivalent C code:
{
    struct String username;
    String_constructor(&username, "alice");  // Constructor

    struct String message;
    String_constructor(&message, "Hello!");  // Constructor

    printf("Processing: %s\n", String_c_str(&username));

    String_destructor(&message);   // Automatic!
    String_destructor(&username);  // Automatic, reverse order!
}
```

*syntactic sugar:* RAII = Constructor allocates, Destructor frees. No manual
`free`!

## Construction and Destruction Order
### Rule 1: Local Variables
Objects are destroyed in reverse order of their construction:

```cpp
{
    String a("first");   // Constructed 1st
    String b("second");  // Constructed 2nd
    String c("third");   // Constructed 3rd
}
// Destroyed: c (3rd), b (2nd), a (1st) - REVERSE!
```

**Why?** Later objects might depend on earlier ones. Reverse order ensures
dependencies are destroyed last.

### Rule 2: Member Variables
Members are destroyed in reverse declaration order:

```cpp
class Payload {
private:
    String username;  // Declared 1st
    String content;   // Declared 2nd

public:
    Payload(const char *user, const char *msg)
        : username(user), content(msg) {}
    // Destructor automatically destroys: content, then username
};
```

*syntactic sugar*:
```cpp
~Payload() {
    content.~String();   // Destroy 2nd-declared member first
    username.~String();  // Destroy 1st-declared member last
}
```

## Dependency Chains: RAII Composition
RAII objects can contain other RAII objects. Cleanup happens automatically in
the correct order.

### Example: Command with Multiple Strings
On upcoming exercises, we will implement command with *inheritance*. But since
we have not covered inheritance yet, this example does not use it:
```cpp
class LoginCommand {
private:
    String username;   // RAII object 1
    String password;   // RAII object 2

public:
    LoginCommand(const char *cmd, const char *user, const char *pass)
        : username(user), password(pass) {
        std::cout << "LoginCommand constructed\n";
    }

    ~LoginCommand() {
        std::cout << "LoginCommand destructed\n";
        // String destructors called automatically after this!
    }

    void process() {
        // ... business logic for login
    }
};
```

### Full Execution:
```cpp
int main() {
    LoginCommand p("alice", "s3cr3t");
    p.process();
}
```

### Output:
```
String created: alice       ← username constructor
String created: s3cr3t      ← password constructor
Payload constructed
Processing: login
Payload destructed
String destroyed: s3cr3t    ← password destructor (reverse!)
String destroyed: alice     ← username destructor
```

*syntactic sugar*: You never called `delete` or wrote cleanup code. The
compiler generated the entire destruction chain!

## The Dependency Chain in Detail
Construction order (top-to-bottom):
1. `username` member constructed
2. `password` member constructed
3. `LoginCommand` constructor body runs

Destruction order (bottom-to-top, reverse!):
1. `LoginCommand` destructor body runs
2. `password` member destroyed
3. `username` member destroyed

**Why reverse?** If `LoginCommand` constructor uses `username` and `password`
in its body, `username` and `password` must outlive the `LoginCommand`
destructor body. Reverse order guarantees this.

## Comparison: C vs C++
Here is a simplified version of what we have been doing in C (Manual dependency
tracking):
```c
struct login_command {
    char *username;
    char *password;
};

void payload_init(struct login_command *cmd,
                  const char *user, const char *pass) {
    cmd->username = malloc(strlen(user) + 1);
    strcpy(cmd->username, user);

    cmd->password = malloc(strlen(pass) + 1);
    strcpy(cmd->password, pass);
}

void payload_destroy(struct payload *p) {
    free(cmd->password);   // Manual order!
    free(cmd->username);   // Manual order!
}
```

C++ (Automatic dependency tracking):
```cpp
class Logincommand {
private:
    String username;
    String password;

public:
    LoginCommand(const char *user, const char *pass)
                : username(user), password(pass) {}
    // Destructor generated automatically with correct order!
};
```

## What About Standard Library?
`std::string` and `std::vector` are RAII classes, just like our custom
`String`.

```cpp
class Message {
private:
    std::string content;  // RAII, manages char * internally
    std::vector<MessageReceivingEntity> receivers;  // RAII manages list and
                                                    // receivers internally

public:
    Message(const std::string& str) : content(str) {}

    // std::vector destructor called and it called MessageReceivingEntity's
    // destructor
    // std::string destructor called automatically
};
```

Under the hood, `std::string` does:
```cpp
class string {
private:
    char *data;
public:
    string(const char *str) { data = new char[strlen(str) + 1]; }
    ~string() { delete[] data; }
};
```

Same pattern we just built, standard library = collection of well-tested RAII
classes.

---

You have now seen RAII with manual `char *`. Next:

1. Implement your payload classes using custom String (do not implement all of
   them, just a few to get used to defining classes)
2. See how construction/destruction order prevents bugs
3. Replace custom String with `std::string` (same behavior, more features)
4. Understand: Standard library is built on RAII principles

RAII is not a C++ feature or a magic, it is a design pattern. C++ makes it the
default through automatic constructor/destructor calls. This is syntactic sugar
that prevents entire categories of memory bugs.
