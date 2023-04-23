import irc.bot
import irc.strings
import time
import requests

class JokeBot(irc.bot.SingleServerIRCBot):
    def __init__(self, channel, nickname, server, port, password, joke_interval):
        self.channel = channel
        self.nickname = nickname
        self.password = password
        irc.bot.SingleServerIRCBot.__init__(self, [(server, port, password)], nickname, nickname)
        self.channel = irc.strings.lower(channel)
        self.joke_interval = joke_interval

    def on_welcome(self, connection, event):
        connection.join(self.channel)

    def on_pubmsg(self, connection, event):
        message = event.arguments[0]
        if message.startswith(self.nickname + " tell me a joke"):
            response = requests.get('https://official-joke-api.appspot.com/random_joke')
            if response.status_code == 200:
                joke = response.json()
                connection.privmsg(self.channel, joke['setup'])
                time.sleep(self.joke_interval)
                connection.privmsg(self.channel, joke['punchline'])
                time.sleep(self.joke_interval)

channel = "#chan"
nickname = "JokeBot"
server = "localhost"
port = 5544
password = "toto"
joke_interval = 5 # seconds
bot = JokeBot(channel, nickname, server, port, password, joke_interval)
bot.start()
