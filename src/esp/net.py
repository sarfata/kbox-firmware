import socket
import network
import pyb

class TCPServer:
    def __init__(self, port):
        serverip = network.WLAN().ifconfig()[0]
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((serverip, port))
        self.server.setblocking(0)
        self.server.listen(5)

        self.clients = []

        print("Listening on {}:{}".format(serverip, port))

    def acceptClients(self):
        try:
            (clientsocket, address) = self.server.accept()
            self.clients.append(clientsocket)
            print("New client: {}".format(address))
        except OSError as e:
            print("error {}".format(e))

    def writeAll(self, message):
        for c in self.clients:
            c.write(message)

    def loop(self):
        self.acceptClients()


server = TCPServer(2000)

while True:
    server.acceptClients()
    server.writeAll("Hello World!")
    pyb.delay(500)

