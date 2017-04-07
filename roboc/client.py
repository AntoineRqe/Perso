# -*- coding: utf-8 -*-

import socket
import time
from messageHandler import MessageHandler, construct_message, send


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
            "Refresh":
                {
                    "operation": self.refresh
                },
            "Action":
                {
                    "operation": self.action
                },
            "Intro":
                {
                    "operation": self.introduction
                },
            "Wait":
                {
                    "operation": self.wait_other_player_action
                },
            "Usage":
                {
                    "operation": self.cmd_usage
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

    def bind(self, *args, **kwargs):
        """
        Function to send name to server
        """

        msg = construct_message("Bind", args=self.name)
        send(msg, self.server)

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
    def refresh(game_map=[]):
        """
        Print the refreshed map on client side
        :param game_map : The map to be printed
        """

        map_str = str()
        for map_line in game_map:
            map_str += map_line + "\r\n"
        print(map_str)

    @staticmethod
    def cmd_usage(usage=str()):
        """
        Print usage command
        """
        print(usage)

    def action(self, *args, **kwargs):
        """
        Ask user a command for the robot
        """

        cmd = input("{}, What move do you want to do?\r\n".format(self.name))
        msg = construct_message("Action", args=cmd)
        send(msg, self.server)


def main():
    test1 = RobocClient(name="antoine")
    test2 = RobocClient(name="remi")

    while True:
        time.sleep(5)

if __name__ == "__main__":
    main()
