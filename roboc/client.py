# -*- coding: utf-8 -*-

import socket
import time
from messageHandler import MessageHandler, construct_message


class RobocClient:
    """
    Class to define the roboc player on the server
    """

    def __init__(self, host="localhost", port=12800, name="antoine", auto=True, **kwargs):

        self.channel = "close"
        self.server = None
        self.MsgHandler = None

        try:
            self.host = kwargs["host"]
        except KeyError:
            self.host = host

        try:
            self.port = kwargs["port"]
        except KeyError:
            self.port = port

        try:
            self.name = kwargs["name"]
        except KeyError:
            self.name = name

        self.cmd_list = {
            "Bind":
                {
                    "id": 1
                },
            "Refresh":
                {
                    "id": 2,
                    "operation": self.refresh
                },
            "Action":
                {
                    "id": 3,
                    "operation": self.action
                },
            "Intro":
                {
                    "id": 4,
                    "operation": self.introduction
                },
            "Wait":
                {
                    "id": 5,
                    "operation": self.wait_other_player_action
                }
        }

        if auto:
            self.connect()
            self.MsgHandler = MessageHandler(self)
            self.MsgHandler.start()

    def connect(self):
        """
        Function to connect to given server
        And bind the connection
        """
        try:
            self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server.connect((self.host, self.port))
        except ConnectionRefusedError as e:
            print(e)
            return False

        print("{} is connected to server with port {}".format(self.name, self.port))
        self.bind()
        self.channel = "open"
        return True

    def disconnect(self):
        """
        Function to disconnected from the server
        """
        if self.server is not None and self.channel == "open":
            self.server.close()

        self.MsgHandler.terminate()
        self.MsgHandler.join()
        self.server = None
        self.channel = "close"

    def send(self, msg, tries=5, **kwargs):
        """
        Function to send the message through the socket
        :param msg: message to be send
        :param tries: number of tries before and
        """

        if tries == 0:
            print("Impossible to send message")
            return

        if self.server is None:
            print("Socket not opened...")
            return

        if self.channel == "close":
            self.connect()
            return self.send(msg, tries=tries-1, **kwargs)

        self.server.send(msg.encode())

    def bind(self, *args, **kwargs):
        """
        Function to send name to server
        """

        msg = construct_message("Bind", args=self.name)
        self.server.send(msg.encode())

    @staticmethod
    def introduction(intro=str()):
        """
        Print welcome message
        :param intro: string to print
        """
        print(intro)

    @staticmethod
    def wait_other_player_action(wait_msg=str()):
        """
        Print wait message
        """
        print(wait_msg)

    @staticmethod
    def refresh(game_map=[], *args, **kwargs):
        """
        Print the refreshed map on client side
        :param game_map : The map to be printed
        """

        map_str = str()
        for map_line in game_map:
            map_str += map_line + "\r\n"
        print(map_str)

    def action(self, *args, **kwargs):
        """
        Ask user a command for the robot
        """

        cmd = input("{}, What move do you want to do?\r\n".format(self.name))
        msg = construct_message("Action", args=cmd)
        self.send(msg)


def main():
    test1 = RobocClient(name="antoine")
    test2 = RobocClient(name="remi")

    while True:
        time.sleep(5)

if __name__ == "__main__":
    main()
