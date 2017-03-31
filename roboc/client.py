# -*- coding: utf-8 -*-

import socket

class RobocClient:
    """
    Class to define the roboc player on the server
    """

    def __init__(self, host="localhost", port=12800, name="antoine", **kwargs):

        if kwargs.get("host") is not None:
            self.host = kwargs["host"]
        else:
            self.host = host

        if kwargs.get("port") is not None:
            self.port = kwargs["port"]
        else:
            self.port = port

        if kwargs.get("name") is not None:
            self.name = kwargs["name"]
        else:
            self.name = name

        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect((self.host, self.port))
        print("{} is connected to server with port {}".format(self.name, self.port))

    def wait_on_message(self):
        """
        Receive incoming message
        """

        message = self.client.recv(1024)
        print(message.decode())

def main():
    test1 = RobocClient(name="antoine")
    test2 = RobocClient(name="remi")

if __name__ == "__main__":
    main()
