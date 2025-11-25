# Milestone 1: The "It Works" Prototype
In this first phase, our primary goal is to develop a functioning text-based
messaging application by any means necessary. Efficiency and scalability are
secondary to making the system work.

Our messenger will not support advanced features like media attachments or
encryption initially. It will function as a "relay" server that forwards
messages between users. Despite its simplicity, it will handle user
authentication (login/sign up), broadcast channels, and direct messaging.

| Feature | Description |
|--|--|
| authentication | users must be able to login or sign up using a username and password |
| channels | users can create a new channel or join an existing one by providing a channel name and password, messages sent to a channel are broadcast to all subscribers |
| direct messages | a user can send a private message to another specific user by specifying the recipient's username |

## Implementation Strategy & Hints
### The Communication Protocol (JSON)
Instead of inventing a complex custom format, use JSON (JavaScript Object
Notation) for communication between the client and the server. This allows you
to easily structure your data.

*Example*: Authentication Request When a client connects, the first message
they send could look like this:
```json
{
    "payload_type": "auth",
    "command": "login",
    "arguments": {
        "username": "Alice",
        "password": "SecretPassword123"
    }
}
```

*Example*: Sending a Direct Message
```json
{
    "payload_type": "message",
    "arguments": {
        "type": "dm",
        "recipient": "Bob",
        "content": "Hey Bob, are you there?"
    }
}
```

### Architecture: Client vs. Server
You will need to write two separate Python scripts:
- `server.py`: Runs continuously, listens for connections, and manages data.
- `client.py`: The user interface that connects to the server and sends commands.

Logic Flow:
1. The client connects and sends credentials (username/password).
2. The server checks if the user exists. If not, it creates a new account;
   if yes, it verifies the password.
3. Once authenticated, the server stores the connection object in memory.

### Managing State (In-Memory Storage)
Since we are not using a database yet, you can use Python Dictionaries to store
active users and channels while the server is running.

- Storing Users: Map usernames to their socket connections so you know who to
  send data to.
    ```py
    active_clients = {
        "Alice": [<socket connection object at 0x...>, ...],
        "Bob": [<socket connection object at 0x...>]
    }
    ```
- Storing Channels: Map channel names to channel data and subscriber lists.
    ```py
    channels = {
        "Chill": {
            'password': 'supersecret',
            'subscribers': ["Alice", "Bob"]
        },
        "SecretTeam": {
            'password': "123",
            'subscribers': ['Charlie']
        }
    }
    ```

## Recommended Resources
- [Basics of Sockets](https://realpython.com/python-sockets/) - Overview of
  `bind`, `listen` and `accept`.
- [Using JSON with Sockets](https://heycoach.in/blog/sockets-with-python-and-json/) -
  Explains how to send structured data over the network.
- [Handling Multiple Users](https://www.geeksforgeeks.org/python/socket-programming-multi-threading-python/) -
  For allowing more than one person to connect at the same time.
- [Socket Programming Video Tutorial](https://www.youtube.com/watch?v=3QiPPX-KeSc) -
  A quick visual guide of what we are doing.
- *Try parsing the user input yourself. After you receive input from the user on
  the client: if the input starts with `/`, create a payload that refers to the
  appropriate command; if it starts with `@`, create a payload that refers to a
  direct message; if it starts with `#,` create a payload that refers to a
  channel.*


# Milestone 1.5: Basic Tkinter Interface
In this step, you will wrap your existing client logic in a graphical user
interface (GUI) using Python's built-in `tkinter` library.

| Feature | Description |
|--|--|
| chat history | a scrollable text area that displays incoming messages from the server |
| message input | an input field where the user types their message or commands |
| channel/member list| scrollable text areas for list of channels the user subscribed to and users in this channel |

## Implementation Strategy & Hints
### The "Frozen Window" Problem
This is the main challenge of this milestone.
- Tkinter has its own infinite loop (`root.mainloop()`) to keep the window open.
- Your socket connection has an infinite loop (`while True: client.recv(...)`)
  to wait for messages.

If you put the socket loop inside the Tkinter loop, your window will freeze.
The application stops responding to clicks as it is busy awaiting for network
data.

The solution for this problem is threading. Inspect Python's built-in
`threading` library:
```py
import threading

def receive_messages():
    while True:
        try:
            msg = client_socket.recv(1024).decode()
            # HACK: Insert directly into the text widget
            # You will parse incoming payloads and then handle them with
            # related module, for sake of simplicity we directly pushed
            # data into text widget.

            # Note: In professional apps, we use queues for this, but for now
            # direct insertion is fine.
            chat_text_widget.insert(END, msg + "\n")
        except:
            break

# Start the background thread BEFORE starting the GUI loop
threading.Thread(target=receive_messages, daemon=True).start()

root.mainloop()
```

### Command Handling
Since your server expects JSON (from Milestone 1), your Client GUI needs to be
smart. When the user clicks "Send", your code should check what they typed:
- If they typed Hello World -> Wrap it in JSON: \
  `{"payload_type": "message", "content": "Hello World"}`
- If they typed `/subscribe SecretTeam 123` -> Parse this string and send: \
  `{"payload_type": "command", "type": "subscribe", "arguments": { "channel": "SecretTeam", "password": "123" }}`

## Recommended Resources
- [Tkinter Basics](https://realpython.com/python-gui-tkinter/) - Tkinter
  reference for basics
- [Fixing GUI Freeze](https://stackoverflow.com/questions/459083/how-do-you-run-your-own-code-alongside-tkinters-event-loop) -
  Understanding why threads are necessary.
- *Try combining Tkinter and a chat application yourself! This is where the real
  challenge lies, and you will learn a lot while working to overcome it.*
