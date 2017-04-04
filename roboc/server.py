# -*- coding: utf-8 -*-

from maps import Maps
from maze import Maze
import os
from os.path import join as opj
import socket
import select
import json
import time
from interface import game_options
from messageHandler import MessageHandler, construct_message


class RobocServer:
    """
    Class to implement server side based on socket
    """

    def __init__(self, host="", port=12800, auto=True, **kwargs):
        """
        Initialise the server
        :param host: name of host server
        :param port: port to connect too
        :param kwargs: optional argument
        """

        self.players = {}

        self.maze = None
        self.max_players = 2
        self.server = None
        self.active = False
        self.game_active = False
        self.cmd_list = {
            "Bind":
                {
                    "id": 1,
                    "operation": self.bind_clients,
                },
            "Refresh":
                {
                    "id": 2,
                    "operation": self.refresh_maps,
                },
            "Action":
                {
                    "id": 3,
                    "operation": self.process_action,
                }
        }

        if kwargs.get("host") is not None:
            self.host = kwargs["host"]
        else:
            self.host = host

        if kwargs.get("port") is not None:
            self.port = kwargs["port"]
        else:
            self.port = port

        if auto:
            self.start_server()

    def start_server(self):
        """
        Start the server
        """
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((self.host, self.port))
        self.server.listen(5)
        self.active = True
        print("Server connected on {} with port {}".format(self.host, self.port))

    def stop_server(self):
        """
        Stop the server
        """
        if self.server is None:
            return

        self.server.close()
        self.server = None
        self.active = False

    def ask_for_action(self, player):
        """
        Ask the current player an action
        """
        msg = construct_message("Action")
        self.send(msg, player)

    def send(self, msg, recipient, tries=5, **kwargs):
        """
        Send a message thought the socket
        :param msg : message to be sent
        :param recipient: socket to the recipient
        """

        if tries == 0:
            print("Impossible to send message")
            return

        if self.server is None:
            print("Socket not opened...")
            return

        if not self.active:
            self.connect()
            return self.send(msg, tries=tries-1, **kwargs)

        recipient.send(msg.encode())

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

        if self.game_active:
            return

        while len(self.players) < self.max_players:
            rlist, wlist, xlist = select.select(client_connected, [], [])
            for read in rlist:
                if len(self.players) < self.max_players:
                    try:
                        msg = json.loads(read.recv(1024))
                    except json.decoder.JSONDecodeError as e:
                        print(e)
                        pass

                    self.players[msg["Bind"]["args"]] = read

        print("All players are here :")
        for player in self.players.keys():
            print(player)

        self.game_active = True

    def wait_on_message(self):
        """
        Receive incoming message and process requested operation
        """

        socket_list = list(self.players.values())

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

    def choose_map(self):
        """
        Function to choose the map to play and initialise it.
        Must be called when players are all here
        """

        labyrinths = Maps()
        labyrinths.load_map(opj(os.getcwd(), "Maps"))

        self.maze = Maze(game_options["S"]["cmd"](labyrinths), list(self.players.keys()))
        self.cmd_list["Refresh"]["content"] = self.maze.map
        self.refresh_maps()

    def refresh_maps(self):
        """
        Send update maze map to all player
        """

        msg = construct_message("Refresh", args=self.maze.map)

        for player_socket in self.players.values():
            self.send(msg,player_socket)

    def process_action(self):
        pass

def main():
    test = RobocServer()
    test.wait_for_clients()
    thread = MessageHandler(test)
    thread.start()

    while not test.game_active:
        time.sleep(5)

    test.choose_map()
    while True:
        test.ask_for_action(test.players[test.maze.current_player])

if __name__ == "__main__":
    main()