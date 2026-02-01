# Extending a Non-OOP Project
In Ch01, you built a basic message relay. Now you need to extend it with new
features. This exercise will demonstrate how difficult it becomes to maintain
and extend code that relies on traditional if/else dispatching.

## Objective
Your program needs to handle two new capabilities:

**1. Command Arguments**: Commands now need to parse and store their arguments:
1. **Login** - `/login <username> <password>`
   - Store username and password
2. **Join** - `/join <channel>`
   - Store channel name
3. **Logout** - `/logout`
   - No arguments

Output format:
```
Command: login
  Arguments: [username: metw, password: SuperSecretP4%%w0rd]
Command: join
  Arguments: [channel: general]
Command: logout
  Arguments: []
```

**2. Payload Buffering:** Instead of printing payloads immediately, you need to
1. Read and parse all payloads from the file into a buffer (array/list)
2. After reading everything, print (process) payloads *one by one* from the
   buffer
3. Print "Processing payload X of Y" before each message/command

This simulates how real servers queue payloads before processing. As we do not
have any networking capabilities yet, we simply print a payload out to
represent how it may be processed (e.g. print formatting).

## Example
(using `payloads.txt` from previous example)\
Expected Output:
```
--- Reading payloads ---
Read 8 payloads

--- Processing payloads ---
Processing payload 1 of 8
Command: login
  Arguments: [username: metw, password: SuperSecretP4%%w0rd]

Processing payload 2 of 8
Global message: Hello, world!

Processing payload 3 of 8
Group message to announcements: Server maintenance tonight

...
```

## Requirements
Your program should:

1. Create payload and buffer structures:
   - Store the payload type (login / join / logout / direct message /
     group message / global message)
   - For commands, store parsed arguments
2. Do two-pass processing:
   - First pass: Read file and populate buffer. You should allocate new
     `char *` for each string (receiver name, command arguments etc.)
   - Second pass: Process each buffered message, print them out
3. Command argument parsing:
   - Extract command and all arguments
   - Store arguments in an `union` structure
4. Maintain compatibility with previous version:
   - Your code should still handle direct messages, group messages, and global
     messages from Ch01

## Hints
- Recap: [union](https://www.w3schools.com/c/c_unions.php)
  ```c
  union payload_data {
      struct { char *username; char *password; } login;
      struct { char *content; } message;
  };
  ```
- Recap: [enum](https://www.w3schools.com/c/c_enums.php)
  ```c
  union payload_kind {
      COMMAND_LOGIN, COMMAND_LOGOUT /* ... */
  };
  ```
- Consider using a struct to represent a buffered payload and enum for type of
  it:
  ```c
  struct payload {
      enum payload_kind kind;  /* Type discriminator */
      union payload_data data;  /* Type-specific payload data */
  }
  ```
- Dynamic allocation (`malloc`) will be necessary for the buffer

---

The problem this reveals is that the traditional approaches make extensions
**exponentially harder**:
- Ch01: Handle 4 message types -> ~50 lines
- Ch02: Add arguments + buffering + separate processing -> ~300+ lines

<details>
  <summary>
    Spoiler: Do not inspect the requirements for Ch03 before completing this
    chapter if you value your mental health!
  </summary>

  The agony of extending your code will be unbearable. You will eventually be
  forced to either perform a complete rewrite to accommodate new features or
  fundamentally change your design principles to escape the impending misery.

  Inspect the solutions for this chapter. Current design principle becoming
  increasingly unmaintainable with
  [new requirements](../03_refactoring-with-function-pointers.html).
</details>
