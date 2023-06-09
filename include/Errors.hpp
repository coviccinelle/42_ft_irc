#ifndef ERRORS_HPP
# define ERRORS_HPP

# define RPL_WELCOME(nick, user, host) ("001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n")
# define RPL_YOURHOST(nick, servername, version) ("002 " + nick + " :Your host is " + servername + ", running version " + version + "\r\n")
# define RPL_CREATED(nick, date) ("003 " + nick + " :This server was created " + date + "\r\n")
# define RPL_MYINFO(nick, servername, version, umode, cmode, pmode) ("004 " + nick + " " + servername + " " + version + " " + umode + " " + cmode + " " + pmode + "\r\n")
# define RPL_LIST(nick, channel, topic) ("322 " + nick + " " + channel + " #visible" + " :" + topic + "\r\n")
# define RPL_LISTEND(nick) ("323 " + nick + " :End of LIST\r\n")

# define PING(localhost) ("PING " + localhost + "\r\n")
# define PONG(localhost) ("PONG " + localhost + "\r\n")
# define QUIT_REASON(nick, user, host, msg) (nick + "!" + user + "@" + host  + " QUIT :" + msg + "\r\n")
# define QUIT(nick, user, host) (nick + "!" + user + "@" + host  + " QUIT\r\n")
# define NICK(old_nick, nick) (":" + old_nick + " NICK " + nick + "\r\n")
# define NICK_INFORM(old_nick, user, host, nick) (":" + old_nick + "!" + user + "@" + host + " NICK " + nick + "\r\n")
# define JOIN(nick, user, host, chan) (":" + nick + "!" + user + "@" + host  + " JOIN " + chan + "\r\n")

# define PRIVMSG(nick, user, host, dest, str) (":" + nick + "!" + user + "@" + host + " PRIVMSG " + dest + " :" + str + "\r\n") //dest cest le nom du channel
																																//
# define NOTICE(nick, user, host, chan, str) (":" + nick + "!" + user + "@" + host + " NOTICE " + chan + " :" + str + "\r\n")
# define PART(nick, user, host, chan) (":" + nick + "!" + user + "@" + host + " PART " + chan + "\r\n")
# define PART_REASON(nick, user, host, chan, reason) (":" + nick + "!" + user + "@" + host + " PART " + chan + " :" + reason + "\r\n")
# define KILL(nick, target) (":" + nick + " KILL " + target + "\r\n")
# define KILL_REASON(nick, target, reason) (":" + nick + " KILL " + target + " :" + reason + "\r\n")
# define BANNED(nick, user, host, channel, target) (":" + nick + "!" + user + "@" + host + " MODE " + channel + " +b " + target + "\r\n")
# define KICK(nick, user, host, chan, kicked, reason) (":" + nick + "!" + user + "@" + host + " KICK " + chan + " " + kicked + " :" + reason + "\r\n")
# define INVITE(nick, user, host, nickinvite, channel)  (":" + nick + "!" + user + "@" + host + " INVITE " + nickinvite + " " + channel + "\r\n")
# define RPL_ENDOFWHO(nick, mask) ("315 " + nick + " " + mask + " :End of /WHO list.\r\n")
# define RPL_WHOREPLY(nick, chan, user, host, server, realname) ("352 " + nick + " " + chan + " " + user + " " + host + " " + server + " " + nick + " " + " H@ :0 " + realname + "\r\n")
# define RPL_CREATIONTIME(nick, chan, ctime)("329 " + nick + " " + chan + " " + ctime + "\r\n")
# define RPL_TOPICWHOTIME(nick, chan, whoset) ("333 " + nick + " " + chan + " " + whoset +  "\r\n")
# define RPL_NOTOPIC(prefix, chan) ("331 " + prefix + " " + chan + " :No topic is set\r\n")
# define TOPIC(nick, chan, topic) (":" + nick + " TOPIC " + chan + " :" + topic + "\n\r")
# define RPL_TOPIC(nick, chan, topic) ("332 " + nick + " " + chan + " :" + topic + "\r\n")
# define RPL_BANLIST(nick, chan, mask, from, time) ("367 " + nick + " " + chan +  " " + mask + " " + from + " " + time +  "\r\n")
# define RPL_ENDOFBANLIST(nick, chan)("368 " + nick + " " + chan + " :End of Channel Ban List\r\n") 
# define ERR_ALREADYREGISTERED "462 :You may not reregister\r\n"
# define ERR_NEEDMOREPARAMS(command) (std::string("461 ") + command + " :Not enough parameters\r\n")
# define ERR_PASSWDMISMATCH(from) "464 " + from + " :Password incorrect\r\n"
# define ERR_NONICKNAMEGIVEN "431 :No nickname given\r\n"
# define ERR_ERRONEUSNICKNAME(nick) ("432 " + nick + " :Erroneous nickname\r\n")
# define ERR_NICKNAMEINUSE(to, nick) ("433 " + to + " " + nick + " :Nickname is already in use\r\n")
# define ERR_NOSUCHNICK(invitenick) ("401 " + invitenick + " :No such nick/channel\r\n")
# define ERR_NOSUCHCHANNEL(channel) ("403 " + channel + " :No such channel\r\n")
# define RPL_YOUREOPER(nick) ("381 " + nick + " :You are now an IRC operator\r\n")
# define ERR_UNKNOWNMODE(nick, ch) ("472 " + nick + " " + ch + " :is unknown mode char to me\r\n")
# define ERR_BADCHANMASK(channel) ("476 " + channel + " :Bad Channel Mask\r\n")
# define ERR_NOPRIVILEGES "481 :Permission Denied- You're not an IRC operator\r\n"
# define ERR_BANNEDFROMCHAN(nick, channel) ( "474 " + nick + " " + channel + " :Cannot join channel (+b)\r\n")
# define ERR_CHANNELISFULL(nick, channel) ("471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n")
# define ERR_TOOMANYCHANNELS(nick, channel) ("405 " + nick + " " + channel + " :You have joined too many channels\r\n")
# define RPL_NAMREPLY(nick, channel) ("353 " + nick + " = " + channel + " :")
# define RPL_ENDOFNAMES(nick, channel) ("366 " + nick + " " + channel + " :End of /NAMES list.\r\n")
# define RPL_INVITING(prefix, invitenick, channel) ("341 " + prefix + " " + invitenick + " " + channel + "\r\n")
# define ERR_USERNOTINCHANNEL(nick, chan) ("441 " + nick + " " + chan + " :They aren't on that channel\r\n")
# define ERR_NOTONCHANNEL(chan) ("442 " + chan + " :You're not on that channel\r\n")
# define ERR_CHANOPRIVSNEEDED(chan) ("482 " + chan + " :You're not channel operator\r\n")
# define ERR_INVITEONLYCHAN(nick, chan) ("473 " + nick + " " + chan + " :Cannot join channel (+i)\r\n")
# define ERR_CANNOTSENDTOCHAN(nick, chan) ("404 " + nick + " " + chan + " :Cannot send to channel\r\n")
# define ERR_USERSDONTMATCH(nick) ("502 " + nick + " :Cannot change mode for other users\r\n")
# define ERR_USERONCHANNEL(invitenick, chan) ("443 " + chan + " " + invitenick + " :is already on channel\r\n")
# define ERR_NORECIPIENT(command) ("411 :No recipient given " + command + "\r\n") 
# define ERR_NOTEXTTOSEND "412 :No text to send\r\n"
# define ERR_NOSUCHNICK(invitenick) ("401 " + invitenick + " :No such nick/channel\r\n")
# define ERR_TOOMANYTARGETS(target, msg) (target + ":407 recipients. " + msg + " ⚠️ \r\n") 
# define ERR_UMODEUNKNOWNFLAG(flag)("501 " + flag + " :Unknown MODE flag\r\n")
# define RPL_UMODEIS(user, mode) ("221 " + user + " +" + mode + "\r\n")
# define RPL_CHANNELMODEIS(nick, chan, mode) ("324 " + nick + " " + chan + " " + mode + "\r\n")
# define ERR_NOOPERHOST(nick) ("491 " + nick + " :No O-lines for your host\r\n")
# define ERR_CANTKILLSERVER(from) ("483 " + from + " :You can't kill a server!\r\n")
# define ERR_NOORIGIN ("409 :No origin specified\r\n")
# define ERR_NOSUCHSERVER(servername) ("402 " + servername + " :No such server\r\n")


#endif
