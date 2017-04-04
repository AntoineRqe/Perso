from threading import Thread
import sys

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
        sys.stdout.write("MessageHandler on {}\r\n".format(self.client))
        sys.stdout.flush()

        while True:
            self.client.wait_on_message()

