# References and Copy Management
In Ch05, we built a custom `String` class with RAII. In Ch06, we used it with
polymorphism. But there is a hidden danger: the default copy behavior can cause
double-free crashes.

This chapter covers two essential topics:
1. References to avoid expensive copies
2. Copy control for making RAII classes safe to copy

## The Hidden Bug in Our Code
Recall the custom `String` class from Ch05:
```cpp
class String {
public:
    String(const char *data_) {
        data = new char[strlen(data_) + 1];
        strcpy(data, data_);
    }

    ~String() { delete[] data; }

    const char* c_str() const { return data; }

private:
    char *data;
};
```

This code has a critical bug: *The Double-Free Disaster*
```cpp
void print_string(String s) {  // !bug: passes by value (creates copy)
    cout << s.c_str() << endl;
}  // s destroyed here, deletes[] data called

int main() {
    String name { "alice" };
    print_string(name);  // Creates copy with same data pointer
    // name destroyed here
    // delete[] data called again on the same pointer. Double-free, CRASH!
}
```

Step-by-step:
1. `name` created, allocates memory for "alice"
2. `print_string(name)` copies `name`:
   - Default copy constructor copies `data` pointer
   - Both `name` and `s` point to same memory
3. `s` destroyed at end of function: deletes memory
4. `name` destroyed at end of main: deletes *already-freed* memory

This is the shallow copy problem: copying the pointer, not the data.

## Solution 1: References - Stop Copying
Instead of creating a copy, pass a reference (alias) to the original:
```cpp
void print_string(String &s) {  // Reference, not copy
    cout << s.c_str() << endl;
}  // Nothing destroyed, just alias goes away

int main() {
    String name("alice");
    print_string(name);  // No copy created!
}  // name destroyed once, everything is fine
```

`String &s` means "s is just another name for the original String, and I
promise not to modify it via RAII methods."

**Reference Syntax**
```cpp
int x = 42;
int &ref = x;     // ref is an alias for x
ref = 100;        // x is now 100

cout << x;        // Prints 100
cout << ref;      // Prints 100 (same variable!)
```

**References vs Pointers**
| Feature | Pointer | Reference |
|---------|---------|-----------|
| Can be null? | Yes (`nullptr`) | No (must refer to something) |
| Can change target? | Yes | No (always refers to same object) |
| Syntax | `*ptr`, `ptr->member` | Just use like variable, no field dereference operator (`->`) |
| Use case | Optional parameters, arrays | Function parameters (avoid copies) |

### When to Use References
Pass by const reference: Default for objects:
```cpp
void process(const Message &msg);  // Read-only, no copy
void send(const String &content);
```

Pass by reference, only when you need to modify:
```cpp
void update_content(Message &msg, const String &newContent) {
    msg.set_content(newContent);  // Modifies original
}
```

Pass by value, small types only:
```cpp
void set_port(int port);      // int is cheap to copy
void set_flag(bool enabled);  // bool is cheap to copy
```

Rule of thumb: If it has a destructor (manages resources), use `&`.

### const Correctness
Mark methods that do not modify the object as `const`:
```cpp
class String {
public:
    const char *c_str() const { return data; }  // Does not modify

    void clear() { delete[] data; data = nullptr; }  // Not const, modifies
};
```

**Why?** `const` methods can be called on `const` references:
```cpp
void print_string(const String &s) {
    cout << s.c_str();  // OK, c_str() is const
    s.clear();          // ERROR: clear() is not const
}
```

### Exercise 01: Fix the References
Refactor this code to use references:

```cpp
// Current code (inefficient and buggy):
void display_message(GlobalMessage msg) {  // Copies entire message!
    cout << "Message: " << msg << endl;
}

// Copies both!
void append_content(GlobalMessage msg, /* string from std */ string suffix) {
    msg.setContent(msg + suffix);  // Modifies copy, not original!
    // std::string overloaded + operator for string concatenation.
    // We will discuss operator overloading later on.
}

int main() {
    GlobalMessage m { "Hello" };
    display_message(m);  // Wasteful copy
    append_content(m, "!");  // Does not modify m!
    display_message(m);  // Still prints "Hello", not "Hello!"
}
```

Tasks:
1. Fix `display_message` to avoid copying
2. Fix `append_content` to modify the original message
3. Add `const` to methods that don't modify objects

## Solution 2: Deep Copy - Rule of Three
References solve the "passing to functions" problem. But what about this?

```cpp
String name1 { "alice" };
String name2 = name1;  // What should happen here?
```

Two options:
### Option 1: Prevent Copying
```cpp
class String {
public:
    // Delete copy operations:
    String(const String &) = delete;
    String &operator=(const String &) = delete;

    // Other members...
};

String name1 { "alice" };
String name2 = name1;  // COMPILE ERROR: copy deleted
```

Use when: The resource cannot be meaningfully shared (e.g. file handles,
synchronization primitives).

### Option 2: Deep Copy
```cpp
class String {
public:
    // Copy constructor: creates independent copy
    String(const String &other) {
        data = new char[strlen(other.data) + 1];
        strcpy(data, other.data);  // Copy the DATA, not the pointer
        cout << "String copied: " << data << "\n";
    }

    // Copy assignment: replaces content with copy
    String &operator=(const String &other) {
        if (this != &other) {  // Self-assignment check
            delete[] data;     // Free old data
            data = new char[strlen(other.data) + 1];
            strcpy(data, other.data);  // Copy new data
        }
        return *this;
    }

    // Destructor (already had this)
    ~String() {
        delete[] data;
    }

private:
    char *data;

};
```

Now copying is safe:
```cpp
String name1 { "alice" };
String name2 = name1;  // Deep copy, independent memory
name1.~String();       // Deletes name1's memory
// name2 still valid, has its own copy!
```

### The Rule of Three
If you define any of these, you should define all three:

1. Destructor `~T()`
2. Copy constructor `T(const T&)`
3. Copy assignment `T& operator=(const T&)`

**Why?** If your class needs a custom destructor (manages resources), the default
copy operations are almost certainly wrong.

Shallow copy (default, WRONG for our String):
```
Before copy:
  name1 -> [data ptr] -> "alice"

After: String name2 = name1;
  name1 -> [data ptr] --
                       |-> "alice" SHARED! BAD!
  name2 -> [data ptr] --

After name1 destroyed:
  name2 -> [data ptr] -> <freed memory>  CRASH!
```

Deep copy (correct):
```
Before copy:
  name1 -> [data ptr] -> "alice"

After: String name2 = name1;
  name1 -> [data ptr] -> "alice"
  name2 -> [data ptr] -> "alice"  Independent copy, GOOD!

After name1 destroyed:
  name2 -> [data ptr] -> "alice"  Still valid!
```

### The Rule of Five (C++11)
Modern C++ adds move operations:

4. Move constructor `T(T &&)`
5. Move assignment `T &operator=(T &&)`

For now, we will stick with rule of three. Move semantics is an optimization
(transfer instead of copy), but not essential for correctness.
[Here](https://www.geeksforgeeks.org/cpp/rule-of-five-in-cpp/) is a quick
reference for rule of five, if you are into it.

### The Rule of Zero
Best practice: Avoid manual resource management entirely.

```cpp
// Instead of custom String with rule of three:
class Message {
public:
    Message(const char *content_);
    Message(const Message&);  // Copy constructor
    Message& operator=(const Message&);  // Copy assignment
    ~Message();  // Destructor

private:
    char *content;  // Manual memory. Rule of three needed
};

// Use std::string (already implements rule of three):
class Message {
public:
    Message(const char *content_) : content { content_ } {}
    // No custom destructor/copy needed! Compiler defaults work!

private:
    string content;  // RAII type, handles copying automatically
};
```

Rule of Zero: If all your members are RAII types (`std::string`, `std::vector`,
`std::unique_ptr`), you don't need custom copy/destructor.

### Exercise 02: Implement Rule of Three
```cpp
class Buffer {
public:
    Buffer(size_t s) : size(s) {
        data = new char[size];
        memset(data, 0, size);
    }

    ~Buffer() {
        delete[] data;
    }

    // TODO: Implement copy constructor
    // TODO: Implement copy assignment

    char *get_data() { return data; }
    size_t get_size() const { return size; }

private:
    char *data;
    size_t size;
};

// This should work after you implement copy operations:
void test_buffer() {
    Buffer buf1(100);
    buf1.get_data()[0] = 'X';

    Buffer buf2 = buf1;  // Copy constructor
    buf2.get_data()[0] = 'Y';

    cout << buf1.get_data()[0];  // Should print 'X' (not 'Y'!)
    cout << buf2.get_data()[0];  // Should print 'Y'
}
```

Tasks:
1. Implement copy constructor that performs deep copy
2. Implement copy assignment operator (don't forget self-assignment check!)
3. Verify no memory leaks with valgrind

Bonus: Rewrite using `std::vector<char>` (Rule of Zero).

---

For references, avoid expensive copies by passing `const T &`. Use `T &` only
when you need to modify. Small types (int, bool) can still pass by value.

Rule of Three:
- If you write a destructor, write copy constructor and copy assignment
- Prevents shallow copy bugs (double-free)
- Implement deep copy: allocate new memory, copy data

Rule of Zero:
- Prefer RAII types (`std::string`, `std::vector`)
- Let compiler generate correct copy operations (syntactic sugar!)
- Only write rule of three when you must manage raw resources

### Pattern Reference
```cpp
// 1. Read-only parameter:
void process(const Message &msg);

// 2. Modifiable parameter:
void update(Message &msg);

// 3. RAII class that owns memory:
class MyClass {
public:
    MyClass(const MyClass &);  // Deep copy
    MyClass &operator=(const MyClass &);  // Deep copy
    ~MyClass();  // Free memory

private:
    char *data;
};

// 4. RAII class that CANNOT be copied
class Socket {
public:
    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;
    ~Socket();

private:
    int fd;
};

// 5. Rule of Zero (preferred):
class Message {
    std::string content;  // Handles its own copying
    // No custom copy/destructor needed!
};
```

---

These are fundamental C++ idioms you will use in every project. Combined with
RAII (Ch05) and polymorphism (Ch06), you can now begin writing safe, modern
C++ code.

Future topics (you will learn during your hands-on project):
- Move semantics (optimization for transfers)
- Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Templates and generic programming
- Exception safety

But with references and proper copy control, you have the foundation for safe
C++ programming!
