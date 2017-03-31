# -*- coding: utf-8 -*-

from maps import Maps
from maze import Maze
import os
from os.path import join as opj
import socket
from select import select
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

        while len(self.players) < self.max_players:
            client_requests, wlist, xlist = select([self.server], [], [])
            print("Clients wait for connection {}".format(client_requests))

            for request in client_requests:
                print("1 {}".format(request))
                if len(self.players) < self.max_players:
                    clients_connection, infos_connections = request.accept()
                    print("2 {}".format(clients_connection))
                    self.players[infos_connections] = clients_connection

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

if __name__ == "__main__":
    main()