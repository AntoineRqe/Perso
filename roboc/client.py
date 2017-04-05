# -*- coding: utf-8 -*-

import socket
import json
import select
import time
from messageHandler import MessageHandler, construct_message


class RobocClient:
    """
    Class to define the roboc player on the server
    """

    def __init__(self, host="localhost", port=12800, name="antoine", auto=True, **kwargs):

        self.channel = "close"
        self.socket = None

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

        self.cmd_list = {
            "Bind":
                {
                    "id": 1,
                    "operation": self.bind
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
                }
        }

        if auto:
            self.connect()

    def connect(self):
        """
        Function to connect to given server
        And bind the connection
        """
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))
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
        if self.socket is not None and self.channel == "open":
            self.socket.close()

        self.socket = None
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

        if self.socket is None:
            print("Socket not opened...")
            return

        if self.channel == "close":
            self.connect()
            return self.send(msg, tries=tries-1, **kwargs)

        self.socket.send(msg.encode())

    def bind(self, *args, **kwargs):
        """
        Function to send name to server
        """

        msg = construct_message("Bind", args=self.name)
        self.socket.send(msg.encode())

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

    def wait_on_message(self):
        """
        Receive incoming message and process requested operation
        """

        socket_list = list()
        socket_list.append(self.socket)

        try:
            rlist, wlist, xlist = select.select(socket_list, [], [])
        except select.error:
            pass

        for read in rlist:
            raw_msg = read.recv(1024).decode()
            if len(raw_msg) == 0:
                continue

            print("Received message :\n\r" + raw_msg)

            msg_dict = json.loads(raw_msg)
            cmd_key = list(msg_dict.keys())[0]
            cmd_value = list(msg_dict.values())[0]

            if "args" in cmd_value:
                args = cmd_value["args"]
            else:
                args = None

            if cmd_key in list(self.cmd_list.keys()):
                if cmd_value["id"] == self.cmd_list[cmd_key]["id"]:
                    self.cmd_list[cmd_key]["operation"](args)
                else:
                    print("command ID dismatch, discard message")
            else:
                print("{} not found, discard message".format(cmd_key))


def main():
    test1 = RobocClient(name="antoine")
    test2 = RobocClient(name="remi")

    thread1 = MessageHandler(test1)
    thread2 = MessageHandler(test2)

    thread1.start()
    thread2.start()

    while True:
        time.sleep(5)

if __name__ == "__main__":
    main()
