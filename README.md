*This project has been created as part of the 42 curriculum by anguenda, ntahadou.*

# ft_irc

## Description

IRC, or **Internet Relay Chat**, is a text-based communication protocol that allows several users to communicate in real time through a central server. Users connect to the server with an IRC client, choose a nickname, join discussion channels, exchange messages with everyone inside a channel, or send private messages directly to another user.

`ft_irc` is a C++98 implementation of an IRC server. Its goal is to reproduce the essential behavior of a real IRC server while introducing the main concepts of network programming.

The server listens for incoming TCP connections and manages several clients at the same time using non-blocking sockets and `epoll`. Each connected client must authenticate with the server password and complete the IRC registration process using a nickname and username.

After registration, users can:

- Create or join channels
- Leave channels
- Send messages to a channel
- Send private messages to another user
- Change or view a channel topic
- Invite users to invite-only channels
- Remove users from a channel
- Give or remove channel operator privileges
- Configure channel modes such as passwords, user limits, topic restrictions, and invite-only access

The first user who creates a channel becomes its operator and can manage the channel and its permissions.

Because IRC communication uses TCP, received data does not always correspond to one complete command. A command may arrive in several parts, or several commands may arrive together. The server therefore keeps an individual buffer for every client and processes commands only when a complete IRC line has been received.

The project implements the **server side only**. Existing IRC clients such as HexChat, WeeChat, Irssi, or Netcat can be used to connect to the server and test its behavior.

The main goal of the project is to understand:

- TCP client-server communication
- Socket creation, binding, listening, and accepting connections
- Non-blocking file descriptors
- I/O multiplexing with `epoll`
- IRC command parsing
- Client and channel management
- Message routing and broadcasting
- Network errors and client disconnections
- Persistent buffering of incomplete TCP data

## Instructions

### Requirements

- Linux operating system
- C++ compiler
- GNU Make
- Netcat or an IRC client

### Compilation

Compile the project from the repository root:

```bash
make
```

Remove object files:

```bash
make clean
```

Remove object files and the executable:

```bash
make fclean
```

Recompile the complete project:

```bash
make re
```

### Execution

Run the server with:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 123
```

Arguments:

- `<port>`: TCP port used by the server.
- `<password>`: Password required for client registration.

A port between `1` and `65535` must be used. A non-privileged port such as `6667` is recommended.

### Connecting with Netcat

Open another terminal:

```bash
nc -C 127.0.0.1 6667
```

Register the client:

```text
PASS 123
NICK fox
USER fox 0 * :Fox User
```

### Connecting with an IRC client

Example configuration:

```text
Server: 127.0.0.1
Port: 6667
Password: 123
Nickname: fox
Username: fox
```

## Resources

The following resources were used to understand IRC, TCP sockets, non-blocking I/O, and multiplexing:

- [RFC 2812 — Internet Relay Chat: Client Protocol](https://www.rfc-editor.org/info/rfc2812/)
- [RFC 2812, Section 5.2 — Error Replies](https://www.rfc-editor.org/info/rfc2812/#section-5.2)
- [RFC 1459 — Internet Relay Chat Protocol](https://www.rfc-editor.org/info/rfc1459)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Linux socket(2) manual](https://man7.org/linux/man-pages/man2/socket.2.html)
- [Linux bind(2) manual](https://man7.org/linux/man-pages/man2/bind.2.html)
- [Linux listen(2) manual](https://man7.org/linux/man-pages/man2/listen.2.html)
- [Linux accept(2) manual](https://man7.org/linux/man-pages/man2/accept.2.html)
- [Linux recv(2) manual](https://man7.org/linux/man-pages/man2/recv.2.html)
- [Linux send(2) manual](https://man7.org/linux/man-pages/man2/send.2.html)
- [Linux fcntl(2) manual](https://man7.org/linux/man-pages/man2/fcntl.2.html)
- [Linux epoll(7) manual](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [Linux epoll_ctl(2) manual](https://man7.org/linux/man-pages/man2/epoll_ctl.2.html)
- [Linux epoll_wait(2) manual](https://man7.org/linux/man-pages/man2/epoll_wait.2.html)

RFC 2812 was used as the main reference for IRC command syntax, registration, channel operations, numeric replies, and error replies.

The Linux manual pages and Beej's Guide were used to understand socket creation, binding, listening, accepting clients, sending and receiving data, non-blocking sockets, and event-based I/O.

### Use of AI

AI tools were used as a learning and documentation assistant.

AI was used for:

- Explaining socket programming concepts
- Explaining TCP communication and partial data reception
- Clarifying the behavior of `epoll`
- Reviewing IRC command formats and numeric replies
- Suggesting manual tests using Netcat and IRC clients
- Improving documentation and README organization
- Identifying edge cases to test

AI-generated suggestions were reviewed, adapted, tested, and validated by the project authors. The implementation, debugging, architecture, and final technical decisions were completed by the authors.

## Features

- Multiple simultaneous clients
- IPv4 TCP communication
- Non-blocking sockets
- I/O multiplexing using `epoll`
- Client registration
- Private messages
- Channel messages
- Channel creation and management
- Channel operators
- Channel topics
- Invitations
- User removal
- Channel modes
- Partial command buffering
- Client disconnection handling
- IRC numeric replies and errors
- C++98 compatibility

## Supported Commands

- `PASS`
- `NICK`
- `USER`
- `JOIN`
- `PART`
- `PRIVMSG`
- `TOPIC`
- `INVITE`
- `KICK`
- `MODE`
- `QUIT`

## Channel Modes

- `i`: Invite-only channel
- `t`: Only channel operators can change the topic
- `k`: Set or remove a channel key
- `o`: Give or remove operator privileges
- `l`: Set or remove a channel user limit

## Usage Examples

Join or create a channel:

```text
JOIN #general
```

Send a message to a channel:

```text
PRIVMSG #general :Hello everyone
```

Send a private message:

```text
PRIVMSG bob :Hello Bob
```

Set a topic:

```text
TOPIC #general :Welcome to the general channel
```

Invite a user:

```text
INVITE bob #general
```

Remove a user:

```text
KICK #general bob :Reason
```

Enable invite-only mode:

```text
MODE #general +i
```

Set a channel key:

```text
MODE #general +k secret
```

Set a channel limit:

```text
MODE #general +l 10
```

## Technical Choices

The server uses one listening socket to accept incoming connections. Each connected client receives its own socket file descriptor.

All sockets are configured as non-blocking and monitored through `epoll`. This allows the server to handle multiple clients without creating one thread or one process per connection.

Because TCP is a byte-stream protocol, one command may arrive through several `recv()` calls, and several commands may arrive through one `recv()` call.

Each client therefore has a persistent input buffer. The server processes a command only after detecting a complete IRC line ending.

Example:

```text
First recv():   "PASS "
Second recv():  "123\r\n"
```


The server rebuilds:

```text
PASS 123\r\n
```

## Testing

Important test cases include:

- Correct and incorrect passwords
- Duplicate nicknames
- Commands before registration
- Joining and leaving channels
- Private and channel messages
- Operator permissions
- Channel modes
- Invalid commands
- Missing parameters
- Abrupt disconnections
- Partial commands
- Several commands received together
- Large message traffic
- Memory leaks
- Invalid memory access

Example partial-data test:

```text
Type: PA       then press Ctrl+D
Type: SS       then press Ctrl+D
Type:  123     then press Enter
```

The server should rebuild:

```text
PASS 123\r\n
```

`Ctrl+D` is handled locally by the terminal. It sends the currently typed bytes without adding a newline. It is not an IRC command.

## Authors

- `anguenda`
- `ntahadou`
