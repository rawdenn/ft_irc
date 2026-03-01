*This project has been created as part of the 42 curriculum. by jmeouchy and rdennaou*


# ft_irc - IRC Server Project

This project implements a **basic IRC server** in C++ as part of the **42 curriculum**. It supports standard IRC commands and behaviors

---

## Description

This server allows multiple clients to connect, register, join channels, and communicate via messages. It supports the following features:

- **User registration** (PASS, NICK, USER)  
- **Messaging** (PRIVMSG to users or channels)  
- **Channel management** (JOIN, PART, TOPIC, MODE, KICK, INVITE)  
- **Channel modes**:
  - `i` — Set/remove invite-only channel  
  - `t` — Set/remove topic restriction to channel operators  
  - `k` — Set/remove channel key (password)  
  - `o` — Give/take operator privileges  
  - `l` — Set/remove user limit 

**Note on user limits (`+l`):**  
If a channel has 10 users and the limit is set to 5, only **future users trying to join** will be restricted. Users leaving and rejoining are still subject to the limit.

---

## Commands Overview

### Registration Commands

- **PASS**: Enter the server password.  
- **NICK**: Set the client nickname.  
- **USER**: Set the username and client information.

### Channel Commands

- **QUIT**: Disconnects the user from the server.  
- **JOIN**: Join a channel; creates it if it doesn’t exist. The first user becomes the channel moderator. Displays  the the topic and current user list upon creation.  
- **PART**: Leave a channel.  
- **PRIVMSG**: Send a message directly to a user or to a channel.  
- **TOPIC**: Set or viewtopic of a channel. By default, channels have no topic.  
- **KICK**: A channel moderator removes a member from a channel.  
- **INVITE**: Adds a user to the invited list; only invited users can join invite-only channels. The user is removed from the list once they join.  
- **MODE**: Set or remove channel modes (`i`, `t`, `k`, `o`, `l`) as described above.

---

## Instructions

1. Compile the program using the provided Makefile: make
2. Run the server binary with the port number and server password:
	./ircserv <port_number> <password>
	# Example:
	./ircserv 6667 mysecretpass
3. Open a new terminal window and connect to the server using nc (Netcat):
	nc -C localhost <port_number>
	# Example:
	nc -C localhost 6667
4. Register your client by sending the registration commands in this order:
	PASS <password>
	NICK <nickname>
	USER <username> 0 * :<real_name>
	# Example:
	PASS mysecretpass
	NICK alice
	USER alice 0 * :Alice Smith
5. Once registered, you can start using other commands such as JOIN, PRIVMSG, TOPIC, MODE, PART, KICK, and QUIT.

## Resources

RFC 1459 §9.2.2 – Registration Commands
https://datatracker.ietf.org/doc/html/rfc1459#section-9.2.2

IRC Documentation - USER Command Reference
https://dd.ircdocs.horse/refs/commands/user

## AI Usage

- This README was partially generated with AI. AI assistance was used for:
- Organizing content and instructions.
- Summarizing numeric command behaviors.
- Refactoring descriptions for clarity and readability.