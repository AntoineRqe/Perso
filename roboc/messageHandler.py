from threading import Thread
import json
import select


class MessageHandler(Thread):
    """
    Thread to control incoming message and process them
    """

    def __init__(self, link):
        Thread.__init__(self)
        self.link = link

    def run(self):
        """
        Waiting for incoming message    
        """

        while True:
            self.wait_on_message()

    def wait_on_message(self):
        """
        Receive incoming message and process requested operation
        """

        if hasattr(self.link, "players"):
            socket_list = list(self.link.players.values())
        else:
            socket_list = list()
            socket_list.append(self.link.server)

        try:
            rlist, wlist, xlist = select.select(socket_list, [], [])
        except select.error:
            pass

        for read in rlist:
            try:
                raw_msg = read.recv(1024).decode()
            except ConnectionError:
                print("Connection broken!")

            if len(raw_msg) == 0:
                continue

            print("Received message :\n\r" + raw_msg)

            try:
                msg_dict = json.loads(raw_msg)
            except json.decoder.JSONDecodeError:
                print("Impossible to decode JSON")
                return

            cmd_key = list(msg_dict.keys())[0]
            cmd_value = list(msg_dict.values())[0]

            if "args" in cmd_value:
                args = cmd_value["args"]
            else:
                args = None

            if cmd_key in list(self.link.cmd_list.keys()):
                if cmd_value["id"] == self.link.cmd_list[cmd_key]["id"]:
                    self.link.cmd_list[cmd_key]["operation"](args)
                else:
                    print("command ID dismatch, discard message")
            else:
                print("{} not found, discard message".format(cmd_key))



generic_commands = dict()
generic_commands["Bind"] = {"id": 1}
generic_commands["Refresh"] = {"id": 2}
generic_commands["Action"] = {"id": 3}
generic_commands["Intro"] = {"id": 4}


def construct_message(cmd_name, *args, **kwargs):
    """
    Function to build JSON to be send
    :param cmd_name: name of the command to send
    :return: a JSON object ready to be sent
    """

    cmd_arg = generic_commands.get(cmd_name)

    if cmd_arg is None:
        print("{} doesn't exist, no message built")
        return cmd_arg

    msg = dict({cmd_name: {}})
    msg[cmd_name]["id"] = cmd_arg["id"]

    if "args" in kwargs:
        msg[cmd_name]["args"] = kwargs["args"]

    msg = json.dumps(msg, indent=4)
    # print("Make : {}".format(msg))

    return msg
