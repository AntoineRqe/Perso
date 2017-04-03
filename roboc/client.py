# -*- coding: utf-8 -*-

import socket
import json
import select

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

        self.reply_list = {"Bind": {
            "id": 1,
            "operation": self.send_presentation
        }}
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((self.host, self.port))
        print("{} is connected to server with port {}".format(self.name, self.port))

    def send_presentation(self, *args, **kwargs):
        """
        Function to send name to server
        """

        msg = json.dumps({"id": 1, "content": self.name})
        print("{} send presentation {}".format(self.name, msg))
        self.socket.send(msg.encode())

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
            raw_msg = read.recv(1024)
            print("raw message : {}".format(raw_msg))
            if len(raw_msg) == 0:
                continue

            msg_dict = json.loads(raw_msg)
            cmd_key = list(msg_dict.keys())[0]
            cmd_value = list(msg_dict.values())[0]

            if cmd_key in list(self.reply_list.keys()):
                if cmd_value["id"] == self.reply_list[cmd_key]["id"]:
                    self.reply_list[cmd_key]["operation"]()
                else:
                    print("command ID dismatch, discard message")
            else:
                print("{} not found, discard message".format(cmd_key))


def main():
    test1 = RobocClient(name="antoine")
    test2 = RobocClient(name="remi")
    while True:
        test1.wait_on_message()
        test2.wait_on_message()

if __name__ == "__main__":
    main()
