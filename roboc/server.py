# -*- coding: utf-8 -*-

from maps import Maps
from maze import Maze
import os
from os.path import join as opj
import socket
import select
import json
from interface import game_options


class RobocServer:
    """
    Class to implement server side based on socket
    """

    def __init__(self, host="", port=12800, **kwargs):
        """
        Initialise the server
        :param host: name of host server
        :param port: port to connect too
        :param kwargs: optional argument
        """

        self.players = {}

        self.maze = None
        self.max_players = 2
        self.active = False
        self.cmd_list = {"Bind": {
            "id": 1
        }}
        self.reply_list = {"Bind": {
            "id": 1,
        }}

        if kwargs.get("host") is not None:
            self.host = kwargs["host"]
        else:
            self.host = host

        if kwargs.get("port") is not None:
            self.port = kwargs["port"]
        else:
            self.port = port

        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((self.host, self.port))
        self.server.listen(5)
        print("Server connected on {} with port {}".format(self.host, self.port))

    def wait_for_clients(self):
        """
        Wait for client to connect for timeout
        :param timeout time to wait for client in second 
        """

        client_connected = []

        while len(client_connected) < self.max_players:
            # Monitor server side for incoming connections
            client_requests, wlist, xlist = select.select([self.server], [], [])

            for client_request in client_requests:
                # Accept connection to server if limit is not reached
                if len(client_connected) < self.max_players:
                    open_sockets, infos_connections = client_request.accept()
                    client_connected.append(open_sockets)

        self.bind_clients(client_connected)

    def bind_clients(self, client_connected):
        """
        Bind client to server providing extras information
        :return: 
        """

        for client in client_connected:
            msg = self.make("Bind")
            client.send(msg.encode())

        while len(self.players) < self.max_players:
            rlist, wlist, xlist = select.select(client_connected, [], [])
            for read in rlist:
                if len(self.players) < self.max_players:
                    msg = json.loads(read.recv(1024))
                    self.players[msg["content"]] = read

        print("All players are here : {}".format(self.players))

    def make(self, cmd_str):
        """
        Function to build JSON to be send
        :param cmd_str: name of the command to send
        :return: a JSON object ready to be sent
        """

        msg = self.cmd_list.get(cmd_str)
        if msg is None:
            print("{} doesn't exist, no message built")
            return msg

        return json.dumps({cmd_str: msg})

    def wait_on_message(self):
        """
        Wait for receiving messages from player
        :return: 
        """

    def choose_map(self):
        """
        Function to choose the map to play and initialise it.
        Must be called when players are all here
        :return: 
        """

        labyrinths = Maps()
        labyrinths.load_map(opj(os.getcwd(), "Maps"))

        self.maze = Maze(game_options["S"]["cmd"](labyrinths), list(self.players.keys()))
        print(self.maze)


def main():
    test = RobocServer()
    test.wait_for_clients()
    test.choose_map()
    while True:
        pass

if __name__ == "__main__":
    main()