from threading import Thread
import json


class MessageHandler(Thread):
    """
    Thread to control incoming message and treat them
    """

    def __init__(self, client):
        Thread.__init__(self)
        self.client = client

    def run(self):
        """
        Waiting for incoming message    
        """

        while True:
            self.client.wait_on_message()

generic_commands = dict()
generic_commands["Bind"] = {"id": 1}
generic_commands["Refresh"] = {"id": 2}
generic_commands["Action"] = {"id": 3}


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
