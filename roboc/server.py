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

    def __init__(self, host="", port=12800, auto=True, max=2, **kwargs):
        """
        Initialise the server
        :param host: name of host server
        :param port: port to connect too
        :param kwargs: optional argument
        """

        self.players = OrderedDict()
        self.maze = None
        self.server = None
        self.active = False
        self.game_active = False
        self.MsgHandler = None
        self.wait_action = threading.Event()
        self.cmd_list = {
            "Bind":
                {
                    "id": 1
                },
            "Refresh":
                {
                    "id": 2
                },
            "Action":
                {
                    "id": 3,
                    "operation": self.process_action,
                },
            "Intro":
                {
                    "id": 4
                },
            "Wait":
                {
                    "id": 5
                }
        }

        try:
            self.host = kwargs["host"]
        except KeyError:
            self.host = host

        try:
            self.port = kwargs["port"]
        except KeyError:
            self.port = port

        try:
            self.max_players = kwargs["max"]
        except KeyError:
            self.max_players = max

        if auto:
            self.start_server()

    def start_server(self):
        """
        Start the server
        """
        print("Start the server...")
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.bind((self.host, self.port))
        self.server.listen(5)
        self.active = True
        print("Server connected on localhost with port {}".format(self.host, self.port))
        self.wait_for_clients()

    def stop_server(self):
        """
        Stop the server
        """
        if self.server is None:
            print("Server not initialised, nothing to stop...")
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

    def cmd_usage(self, player):
        """
        Send robot instructions to player
         :param : name of the player
        """

        instruction = self.maze.cmd_usage()
        self.players[player].send(instruction.encode())

    def ask_for_action(self, player):
        """
        Ask the current player an action
        :param player: name of the player to send the message to
        """

        for name, link in self.players.items():
            if name == player:
                msg = construct_message("Action")
            else:
                msg = construct_message("Wait", args="Waiting for {} to play...".format(player))

            self.send(msg, link)

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

        print("Waiting for client to connect...")
        client_connected = []

        while len(client_connected) < self.max_players:
            # Monitor server side for incoming connections
            client_requests, wlist, xlist = select.select([self.server], [], [])

            for client_request in client_requests:
                # Accept connection to server if limit is not reached
                if len(client_connected) < self.max_players:
                    open_sockets, infos_connections = client_request.accept()
                    client_connected.append(open_sockets)
                    print("{} players are connected on the server".format(len(client_connected)))

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
                    except ConnectionError:
                        pass

                    except json.decoder.JSONDecodeError as e:
                        print(e)
                        pass

                    self.players[msg["Bind"]["args"]] = read

        sum_up_str = "All players are here :\n\r"
        for player in self.players.keys():
            sum_up_str += "\t{}\n\r".format(player)

        print(sum_up_str)

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
        is_valid_command = self.maze.is_command_valid(action)
        if not is_valid_command:
            self.cmd_usage(self.maze.current_player)
            self.ask_for_action(self.maze.current_player)
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

    while not test.game_active:
        time.sleep(1)

    test.choose_map()
    time.sleep(0.1)
    test.introduction()
    time.sleep(0.1)
    test.refresh_maps()
    time.sleep(0.1)

    while True:
        test.ask_for_action(test.maze.current_player)
        test.wait_action.wait()
        if test.maze.is_maze_resolved():
            test.stop_server()

if __name__ == "__main__":
    main()
