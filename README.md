# 🌻✨ **ft_irc** 🎉 💌 - _Internet Relay Chat Server_

## Summary

This project involves creating your own IRC (Internet Relay Chat) server. The server interacts with IRC clients, allowing users to engage in real-time messaging, both publicly and privately. Users can exchange direct messages and join group channels. The project emphasizes adherence to solid standards and protocols that govern computer interactions on the Internet.

![image](https://github.com/coviccinelle/42_ft_irc/assets/51762886/047382fd-88ef-44c6-a1d5-ce395c613e91)


### _How to use:_

Your executable will be run as follows:
`./ircserv <port> <password>`

You can communicate with the server by downloading a client (ex: irssi).
Example command for irssi: 

`/connect localhost <port> <password>`

For more information about the protocol, please go take a look at the document right here (if I recall it correctly) ^^ 

`https://www.rfc-editor.org/rfc/rfc1459.html`

Current Version: 6

![image](https://github.com/coviccinelle/42_ft_irc/assets/51762886/3ec17b20-13cd-42c5-8661-9055583666f5)



## Contents

1. [Introduction](#chapter-i-introduction)
2. [General Rules](#chapter-ii-general-rules)
3. [Mandatory Part](#chapter-iii-mandatory-part)
   - [Requirements](#iii1-requirements)
   - [For MacOS only](#iii2-for-macos-only)
   - [Test Example](#iii3-test-example)
4. [Bonus Part](#chapter-iv-bonus-part)

## Chapter I: Introduction

Internet Relay Chat (IRC) is a text-based communication protocol on the Internet, that provides real-time messaging for public and private conversations. IRC clients connect to servers, forming a network.

## Chapter II: General Rules

- The program should not crash under any circumstances.
- Provide a functional Makefile.
- Compile the code with `c++` using flags `-Wall -Wextra -Werror`.
- Code must comply with the C++98 standard.
- Avoid crashing or unexpectedly quitting for a fully functional project.

## Chapter III: Mandatory Part

### III.1 Requirements

- Develop an IRC server in C++98 named `ircserv`.
- Handle multiple clients simultaneously without hanging.
- No forking allowed; all I/O operations must be non-blocking.
- Use only 1 `poll()` (or equivalent) for handling all operations.
- Implement authentication, nickname setting, channel joining, and private messaging.
- Forward messages to all clients in a channel.
- Support operators and regular users.
- Implement commands specific to operators.

### III.2 For MacOS only

- Use `fcntl()` with `fcntl(fd, F_SETFL, O_NONBLOCK)` for non-blocking mode.

### III.3 Test Example

Verify all possible errors and issues. Use `nc` for a simple test to ensure the correct processing of commands.

## Chapter IV: Bonus Part

- Bonus features: Handle file transfer and implement a bot.
- Bonus part assessed only if the mandatory part is perfect.
