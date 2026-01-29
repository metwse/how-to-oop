# Virtual Methods and Inheritance
In [Exercise 03](../03_questions-arise/README.md), we discussed vtables and how
they solve the problem of storing duplicate function pointers in every object.
We also saw that manual vtable assignment is error-prone.

In [Exercise 04](../04_raii/README.md), we learned how C++ automates resource
management with constructors and destructors.

Now let's see how C++ automates *dynamic dispatch* using `virtual` methods and
code reuse using inheritance.

Recall: Our Manual vtable in C
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

// Step 3: Payload stores a vtable pointer
struct payload {
    const struct payload_vtable *vtable;
    union payload_data data;
};

// Step 4: Manual assignment in constructor
void parse_payload(struct payload *p, const char *raw) {
    if (raw[0] == '/') {
        // ... parsing logic
        p->vtable = &vtable_command_login;  // Manual!
    } else {
        p->vtable = &vtable_message;  // Manual!
    }
}

// Step 5: Dynamic dispatch through vtable
void process_next(struct payload_buffer *buf) {
    struct payload *p = &buf->payloads[buf->process_base];
    p->vtable->process(p);  // Dynamic dispatch
}
```

Recall: Problems of manual vtables
1. Manual vtable assignment. Easy to get wrong!
2. No compile-time checks, type-safety. Can assign wrong vtable!
3. Repetitive vtable creation for every type.
4. No shared code between similar types.

## C++ Syntactic Sugar: Virtual Methods!
### Basic Virtual Method Syntax
```cpp
class Payload {
public:
    // Virtual method = automatically uses vtable
    virtual void process() {
        cout << "Processing generic payload" << endl;
    }

    virtual ~Payload() = default;
};

class Message : public Payload {
public:
    // Override base class method
    void process() override {
        cout << "Processing message" << endl;
    }
};
```

### What C++ Does Behind the Scenes
```c
// Compiler generates vtable automatically
struct Payload_vtable {
    void (*process)(Payload *self);
    void (*destructor)(Payload *self);
};

const struct Payload_vtable vtable_Message = {
    .process = Message_process,  // Automatically assigned!
    .destructor = Message_destructor
};

struct Payload {
    const struct Payload_vtable *vptr;  // Hidden! Added by compiler
    // ... your data members
};

// Constructor automatically sets vptr
void Message_constructor(struct Message *m) {
    m->vptr = &vtable_Message;  // Automatic assignment!
}
```

Unlike manual vtables:
- Compiler generates vtable structs
- Compiler populates vtable entries
- Compiler inserts vptr into every object
- Constructor **automatically** assigns correct vtable
- `override` keyword provides compile-time checking

## Inheritance: Sharing Code
### The Problem with Repetition
In our C implementation, we had lots of repeated code:

```c
void process_command_login(const struct payload *p) {
    // Command-specific processing
    printf("Command: login\n");
    // ... print arguments
}

void process_command_join(const struct payload *p) {
    // Command-specific processing
    printf("Command: join\n");
    // ... print arguments
}

void process_command_logout(const struct payload *p) {
    // Command-specific processing
    printf("Command: logout\n");
    // No arguments
}
```

Every command has similar structure but different details.

### Inheritance Solution
Base class contains shared behavior:
```cpp
class Command : public Payload {
public:
    Command(const char *command_name_)
        : command_name { command_name_ } {}

    // Shared implementation
    void process() override {
        cout << "Command: " << command_name << endl;
        process_arguments();  // Call derived class method
    }

    virtual ~Command() {}

protected:
    // Derived classes must implement this
    virtual void process_arguments() = 0;  // Pure virtual, check end of this
                                           // README for details
    // Syntax is not so important. When you are not sure about syntax, Google
    // it.

    string command_name;
};
```

Derived classes implement specific behavior:
```cpp
class LoginCommand : public Command {
public:
    LoginCommand(const char *username_, const char *password_)
        : Command { "login" }, username { username_ }, password { password_ } {}

private:
    // Implement required method
    void process_arguments() override {
        cout << "  Arguments: [username: " << username
             << ", password: " << password << "]" << endl;
    }

    string username;
    string password;
};

class JoinCommand : public Command {
public:
    JoinCommand(const char *channel_)
        : Command { "join" }, channel { channel_ } {}

private:
    void process_arguments() override {
        cout << "  Arguments: [channel: " << channel << "]" << endl;
    }

    string channel;
};

class LogoutCommand : public Command {
public:
    LogoutCommand() : Command { "logout" } {}

private:
    void process_arguments() override {
        cout << "  Arguments: []" << endl;
    }
};
```

```cpp
// Polymorphism: store different types in same pointer type
Payload *payloads[3];
payloads[0] = new LoginCommand { "alice", "pass123" };
payloads[1] = new JoinCommand { "general" };
payloads[2] = new LogoutCommand {};

// Dynamic dispatch - each calls correct method
for (int i = 0; i < 3; i++) {
    payloads[i]->process();  // Calls through vtable!
}

// Cleanup
for (int i = 0; i < 3; i++) {
    delete payloads[i];  // Virtual destructor ensures correct cleanup!
}
```

```
Command: login
  Arguments: [username: alice, password: pass123]
Command: join
  Arguments: [channel: general]
Command: logout
  Arguments: []
```

## Syntax Reference
1. [Base and Derived Classes](https://www.w3schools.com/CPP/cpp_inheritance.asp)
```cpp
class Base {
    // Shared functionality
};

class Derived : public Base {
    // Specific functionality + inherits from Base
};
```

2. [Virtual Functions](https://www.w3schools.com/CPP/cpp_virtual_functions.asp)
```cpp
virtual return_type method_name();  // Can be overridden
```

3. Pure Virtual Functions (Abstract Classes)
```cpp
virtual void method_name() = 0;  // MUST be overridden
```

4. Override Keyword
```cpp
void process() override;  // Compile error if not overriding
```

5. Virtual Destructor
```cpp
virtual ~ClassName() {}  // Critical for polymorphic classes!
```

**Why critical?** Without virtual destructor:
```cpp
Payload *p = new Message { ... };
delete p;  // Only calls ~Payload(), leaks Message members!
```

With virtual destructor:
```cpp
Payload *p = new Message { ... };
delete p;  // Calls ~Message() then ~Payload() - correct order!
```

6. [Access Specifiers](https://www.w3schools.com/CPP/cpp_access_specifiers.asp)
```cpp
class Example {
public:
    // Accessible from anywhere

protected:
    // Accessible from this class and derived classes

private:
    // Accessible only from this class
};
```

---

You are now familiar with how C++ automates virtual table construction. Next:

1. Implement command hierarchy. Create base `Command` class and derived
   classes: `LoginCommand`, `JoinCommand`, and `LogoutCommand`.
2. Implement message hierarchy. Create base `Message` class with:
   `DirectMessage`, `GroupMessage`, and `GlobalMessage`. Use inheritance to
   share message content storage and common processing logic.
3. Create polymorphic buffer that stores `Payload *` pointers (base class
   pointers) and can hold any derived type (commands or messages).
4. Test virtual destruction via adding print statements in destructors to
   verify derived destructor runs first and base destructor runs second. (This
   task not implemented in provided solution to make code easier to read.)

`virtual` is syntactic sugar for the vtable pattern you built manually.
Inheritance is syntactic sugar for sharing vtable entries and data. C++ did not
invent polymorphism, it automated the plumbing.


## Deep Dive into Pure Virtual Methods
In our Paload, Command, and Message examples, we used a specific syntax:
`virtual void <method_name>() = 0;`. This is called a pure virtual method.

A pure virtual method is a declaration that a function must exist, but the base
class provides no implementation for it. It acts as a mandatory contract: any
lass that inherits from Payload or Command is required to implement this method
to be considered complete.

When a class contains at least one pure virtual method, it becomes an
*Abstract Class*.
- No Instantiation: You cannot create an object of an abstract class (e.g.,
  `new Command("login")` will result in a compiler error).
- Incomplete Blueprint: The class exists solely to provide a common interface
  and shared data for its children.

### Under the Hood: The C Parallel
In your manual C vtable implementation, a pure virtual method is like defining
a function pointer in your `vtable` struct but purposefully leaving it
unassigned in the base "type".

In C: If you accidentally called a `NULL`: function pointer, your program would
crash at runtime.

In C++: The compiler prevents this crash by refusing to compile any code that
tries to create an object that hasn't fulfilled its pure virtual requirements.

As you work through Exercise 05, notice that if you forget to implement
`process_arguments()` in classes that inherit Command, or `process_recipient()`
in classes that inherit Message, your code will not compile. This is the
"syntactic sugar" of compile-time safety replacing manual runtime checks.
