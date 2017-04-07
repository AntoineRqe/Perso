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

        is_Server = True

        try:
            socket_list = list(self.link.players.values())
        except AttributeError:
            socket_list = list()
            socket_list.append(self.link.server)
            is_Server = False

        try:
            rlist, wlist, xlist = select.select(socket_list, [], [])
        except select.error:
            pass

        for read in rlist:
            try:
                raw_msg = read.recv(1024).decode()
            except ConnectionError:
                print("Connection broken!")
                return

            if len(raw_msg) == 0:
                continue

            # print("Received message :\n\r" + raw_msg)

            try:
                msg_dict = json.loads(raw_msg)
            except json.decoder.JSONDecodeError:
                print("Impossible to decode JSON")
                return

            cmd_key = list(msg_dict.keys())[0]
            cmd_value = list(msg_dict.values())[0]

            try:
                args = cmd_value["args"]
            except KeyError:
                args = None

            if cmd_key in list(self.link.cmd_list.keys()):
                self.link.cmd_list[cmd_key]["operation"](args)
            else:
                print("{} not found, discard message".format(cmd_key))


all_commands = ["Bind", "Refresh", "Action", "Intro", "Wait"]


def construct_message(cmd_name, *args, **kwargs):
    """
    Function to build JSON to be send
    :param cmd_name: name of the command to send
    :return: a JSON object ready to be sent
    """

    if cmd_name not in all_commands:
        print("{} doesn't exist, discard message".format(cmd_name))
        return

    msg = dict({cmd_name: {}})

    try:
        msg[cmd_name]["args"] = kwargs["args"]
    except KeyError:
        pass

    msg = json.dumps(msg, indent=4)
    return msg
