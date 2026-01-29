# Extending a Non-OOP Project
In Exercise 00, you built a basic message relay. Now you need to extend it with
new features. This exercise will demonstrate how difficult it becomes to
maintain and extend code that relies on traditional if/else dispatching.

## Objective
Your program needs to handle two new capabilities:

### 1. Command Arguments
Commands now need to parse and store their arguments:

1. **Login** - `/login <username> <password>`
   - Store username and password
2. **Join** - `/join <channel>`
   - Store channel name
3. **Logout** - `/logout`
   - No arguments

**Output format:**
```
Command: login
  Arguments: [username: metw, password: SuperSecretP4%%w0rd]
Command: join
  Arguments: [channel: general]
Command: logout
  Arguments: []
```

### 2. Payload Buffering
Instead of printing payloads immediately, you need to:

1. Read and parse all payloads from the file into a buffer (array/list)
2. After reading everything, print (process) payloads *one by one* from the
   buffer
3. Print "Processing payload X of Y" before each message/command

This simulates how real servers queue payloads before processing. As we do not
have any networking capabilities yet, we simply print a payload out to
represent how it may be processed (e.g. print formatting).

## Expected Output
(using `payloads.txt` from previous example)
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
   - Extract command name and all arguments
   - Store arguments in an `union` structure
4. Maintain compatibility with previous version:
   - Your code should still handle direct messages, group messages, and global
     messages from Exercise 00

## Hints
- Consider using a struct to represent a buffered payload and enum for type of
  it
- Recap: [union](https://www.w3schools.com/c/c_unions.php)
  ```c
  union payload_data {
      struct { char *username; char *password; } login;
      struct { char *content; } message;
  };
  ```
- Dynamic allocation (`malloc`) will be necessary for the buffer
- Think about how to represent "variable number of arguments"

---

The problem this reveals is that the traditional approaches make extensions
**exponentially harder**:
- Exercise 00: Handle 4 message types -> ~50 lines
- Exercise 01: Add arguments + buffering + separate processing -> ~300+ lines

<details>
  <summary>
    Spoiler: Do not inspect the requirements for Exercise 02 before completing
    this exercise if you value your mental health!
  </summary>

  The agony of extending your code will be unbearable. You will eventually be
  forced to either perform a complete rewrite to accommodate new features or
  fundamentally change your design principles to escape the impending misery.

  Inspect the [solution](./) for this exercise. Current design principle
  becoming increasingly unmaintainable with
  [new requirements](../02_refactoring-with-function-pointers/README.md).
</details>
