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
from collections import OrderedDict
from messageHandler import MessageHandler, construct_message
import threading


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

        self.players = OrderedDict()
        self.maze = None
        self.max_players = 2
        self.server = None
        self.active = False
        self.game_active = False
        self.MsgHandler = None
        self.wait_action = threading.Event()
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
                },
            "Intro":
                {
                    "id": 4,
                    "operation": self.introduction
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

        self.game_active = False

        self.MsgHandler.terminate()
        self.MsgHandler.join()

        for player_socket in self.players.values():
            player_socket.close

        self.server.close()
        self.server = None
        self.active = False

    def introduction(self):
        """
        Send introduction message to all players
        """

        welcome = self.initialisation_string()
        for name, link in self.players.items():
            new_welcome = welcome + "You are robot {}!\n\r".format(self.maze.players[name])
            msg = construct_message("Intro", args=new_welcome)
            link.send(msg.encode())

    def cmd_usage(self, player_socket):
        """
        Send robot instructions to player
         :param : socket to be used to send data
        """

        instruction = self.maze.cmd_usage()
        player_socket.send(instruction.encode())

    def ask_for_action(self, player):
        """
        Ask the current player an action
        """
        msg = construct_message("Action")
        self.send(msg, player)
        if self.wait_action.is_set():
            self.wait_action.clear()

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

        self.MsgHandler = MessageHandler(self)
        self.MsgHandler.start()

        self.game_active = True

    def choose_map(self):
        """
        Function to choose the map to play and initialise it.
        Must be called when players are all here
        """

        labyrinths = Maps()
        labyrinths.load_map(opj(os.getcwd(), "Maps"))

        self.maze = Maze(game_options["S"]["cmd"](labyrinths), list(self.players.keys()))
        self.cmd_list["Refresh"]["content"] = self.maze.map

    def refresh_maps(self):
        """
        Send update maze map to all player
        """

        msg = construct_message("Refresh", args=self.maze.map)

        for player_socket in self.players.values():
            self.send(msg,player_socket)

    def process_action(self, action):
        """
        Process the given action by client
        """

        action = action.upper()
        print("Process action {}".format(action))
        is_valid_command = self.maze.is_command_valid(action)
        if not is_valid_command:
            self.cmd_usage(self.players[self.maze.current_player])
            self.ask_for_action(self.players[self.maze.current_player])
            return

        self.maze.robot_commands[action[0]]["cmd"](action)
        self.refresh_maps()
        self.wait_action.set()

    def initialisation_string(self):
        """
        Usage for user
        """
        return "---------------------------------------------------------------------\r\n" + \
               "Welcome in the labyrinth game.\r\n" + \
               "We have {} players in the game.\r\n".format(self.max_players) + \
               "With your robot, you have to exit the maze\r\n" + \
               "On the map is displayed\r\n" + \
               "\tO : Wall\r\n" + \
               "\t1 : Robot\r\n" + \
               "\t. : Door\r\n" + \
               "\tU : Exit\r\n" + \
               "You can move the robot using a direction(N,W,S,E,M,P) and number of steps\r\n" + \
               "Example of use:\r\n" + \
               "\tE5 : 5 steps to east direction\r\n" + \
               "\tN3 : 3 steps to north direction\r\n" + \
               "\tMN : Transform door into wall in north direction\r\n" + \
               "---------------------------------------------------------------------\r\n"


def main():
    test = RobocServer()
    test.wait_for_clients()

    while not test.game_active:
        time.sleep(1)

    test.choose_map()
    time.sleep(0.1)
    test.introduction()
    time.sleep(0.1)
    test.refresh_maps()
    time.sleep(0.1)

    while True:
        test.ask_for_action(test.players[test.maze.current_player])
        test.wait_action.wait()
        if test.maze.is_maze_resolved():
            test.stop_server()

if __name__ == "__main__":
    main()
