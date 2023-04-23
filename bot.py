import irc.bot
import irc.strings
import time
import requests

class Bot(irc.bot.SingleServerIRCBot):
    def __init__(self, channel, nickname, server, port, password, joke_interval):
        self.channel = channel
        self.nickname = nickname
        self.password = password
        irc.bot.SingleServerIRCBot.__init__(self, [(server, port, password)], nickname, nickname)
        self.channel = irc.strings.lower(channel)
        self.joke_interval = joke_interval

    def on_welcome(self, connection, event):
        connection.join(self.channel)
        connection.privmsg(self.channel, f"Hello everyone, I'm {self.nickname}, your friendly neighborhood bot!")
        connection.privmsg(self.channel, f"I'm here to make your day a little brighter and your conversations a little more interesting.")
        connection.privmsg(self.channel, f"Whether you need a joke, a fun fact, or just someone to chat with, I'm your guy.")
        connection.privmsg(self.channel, f"So don't be shy, say hi and let's have some fun!")
        connection.privmsg(self.channel, f"PS: If you need help just send \"!help\"")

    def on_pubmsg(self, connection, event):
        message = event.arguments[0]
        if message.startswith("!help"):
            connection.privmsg(self.channel, "Available commands: !weather, !joke")
        if message.startswith("!joke"):
            connection.privmsg(self.channel, f"Tell me to tell you a joke ;0 : \"{self.nickname} tell me a joke\"")
        if message.startswith(self.nickname + " tell me a joke"):
            response = requests.get('https://official-joke-api.appspot.com/random_joke')
            if response.status_code == 200:
                joke = response.json()
                connection.privmsg(self.channel, joke['setup'])
                time.sleep(self.joke_interval)
                connection.privmsg(self.channel, joke['punchline'])
                time.sleep(self.joke_interval)
        if message.startswith("!weather"):
            params = message.split(" ")

            if (len(params) == 1):
                connection.privmsg(self.channel, f"Please provide me a city name: \"!weather <city>\"")
            else:
                city = params[1]
                response = requests.get(f"https://wttr.in/{city}?format=%C %t")
                connection.privmsg(self.channel, f"Weather in {city}: {response.text}")

channel = "#chan"
nickname = "Joe"
server = "localhost"
port = 5544
password = "toto"
joke_interval = 5 # seconds
bot = Bot(channel, nickname, server, port, password, joke_interval)
bot.start()
