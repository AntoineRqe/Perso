# encoding utf-8


class WrongCoordinate(Exception):
    """
    Exception raised when coordinates out of maze range
    """

    def __init__(self, axis_x, axis_y):
        self.x = axis_x
        self.y = axis_y

    def __repr__(self):
        return "({},{})".format(self.x, self.y)
