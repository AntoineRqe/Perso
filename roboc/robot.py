# encoding utf-8


class Robot:
    """
    Define the class robot which will move across the maze
    """

    def __init__(self, x, y):
        self.axis_x = x
        self.axis_y = y

    def __repr__(self):
        return "My coordinates are {}:{}".format(self.axis_x, self.axis_y)
