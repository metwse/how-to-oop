# Traditional Approach
You will build a simple message relay in this part that reads messages from a
file and processes them based on their type. This milestone will show you the
traditional approach to handling different message types using if/else logic.

Later, you will see how function pointers can make this code cleaner and more
extensible.

## Objective
Write a C program that reads a file line by line and processes different types
of messages.

Payload Types:
1. **Commands** - Lines starting with `/`
   - These are system commands. Commands may continue with arguments, ignore
     them and just print the command name.
   - Print `"Command: <command_name>"`
   - Example: `/quit` prints `"Command: quit"`
2. **Direct Messages** - Lines starting with `@`
   - Private messages to a specific user
   - Print `"Direct message to <username>: <content>"`
   - Example: `@alice Hey there!` prints
     `"Direct message to alice: Hey there!"`
3. **Group Messages** - Lines starting with `#`
   - Messages to a group channel
   - Print `"Group message to <channel>: <content>"`
   - Example: `#general Hello everyone` prints
     `"Group message to general: Hello everyone"`
4. **Global Messages** - All other lines
   - Broadcast messages to everyone
   - Print `"Global message: <content>"`
   - Example: `Hello world` prints `"Global message: Hello world"`

Your program should:
1. Accept a filename as a command-line argument
2. Read the file line by line
3. Identify the payload type based on the first character
4. Process and print each payload according to its type
5. Handle empty lines gracefully (skip them)
6. Close the file and exit cleanly

You do not need to write production-grade codes. You are allowed to leak
memory, ignore boundary checks etc.

## Example
Input file `payloads.txt`
```
/login metw SuperSecretP4%%w0rd
Hello, world!
@bob How are you doing?
#announcements Server maintenance tonight
This is a global broadcast
/logout
@alice Thanks for your help
#general See you all later
```

Expected Output
```
Command: login
Global message: Hello, world!
Direct message to bob: How are you doing?
Group message to announcements: Server maintenance tonight
Global message: This is a global broadcast
Command: logout
Direct message to alice: Thanks for your help
Group message to general: See you all later
```

## What You will Learn
This exercise demonstrates the traditional approach to handling different
types:

- Lots of if/else or switch statements
- Duplicated logic for similar operations
- Hard to extend when adding new payload types

After completing this, you'll appreciate how function pointers can simplify
this pattern!

---

Implement logic for this task yourself, and then inspect the solution 01.
After that, you can start working on the
[second milestone](../02_extending-a-non-oop-project.html).
