# encoding utf-8


class WrongCoordinate(Exception):
    """
    Exception raised when coordinates out of maze range
    """

    def __init__(self, *args, **kwargs):
        Exception.__init__(self, *args, **kwargs)
        self.message = "your coordinates {} are not correct\r\n".format(args)


class NumericalOptions(Exception):
    """
    Exception raised when options contains a numeral value
    """

    def __init__(self, *args, **kwargs):
        Exception.__init__(self, *args, **kwargs)
        self.message = "your option contains a numerical values\r\n"


class EmptyOptions(Exception):
    """
    Exception raised when options are empty
    """

    def __init__(self, *args, **kwargs):
        Exception.__init__(self, *args, **kwargs)
        self.message = "your option is empty\r\n"


class OverSizedOptions(Exception):
    """
    Exception raised when options are empty
    """

    def __init__(self, *args, **kwargs):
        Exception.__init__(self, *args, **kwargs)
        self.message = "your option is too long\r\n"


class WrongOptions(Exception):
    """
    Exception raised when options are empty
    """

    def __init__(self, *args, **kwargs):
        Exception.__init__(self, *args, **kwargs)
        self.message = "your option {} is not available\r\n".format(args)


class InvalidCommands(Exception):
    """
    Exception raised when invalid commands are send to robot
    """

    def __init__(self, *args, **kwargs):
        Exception.__init__(self, *args, **kwargs)
        self.message = "your command {} is not valid\r\n".format(args)
