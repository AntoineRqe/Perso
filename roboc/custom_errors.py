# encoding utf-8


class WrongCoordinate(Exception):
    """
    Exception raised when coordinates out of maze range
    """

    def __init__(self, coordinates):
        self.coordinates = coordinates

    def __str__(self):
        return "your coordinates {} are not correct".format(self.coodinates)


class NumericalOptions(Exception):
    """
    Exception raised when options contains a numeral value
    """

    def __init__(self, option):
        self.option = option

    def __str__(self):
        return "your options {} contains a numerical values".format(self.option)


class EmptyOptions(Exception):
    """
    Exception raised when options are empty
    """

    def __init__(self, option):
        self.option = option

    def __str__(self):
        return "Empty option given".format(self.option)


class OverSizedOptions(Exception):
    """
    Exception raised when options too long
    """

    def __init__(self, option):
        self.option = option

    def __str__(self):
        return "your option {} of size {} is too long".format(self.option, len(self.option))


class WrongOptions(Exception):
    """
    Exception raised when options are not available
    """

    def __init__(self, option):
        self.option = option

    def __str__(self):
        return "Option {} is not available".format(self.option)


class InvalidCommands(Exception):
    """
    Exception raised when invalid commands are send to robot
    """

    def __init__(self, command):
        self.command = command

    def __str__(self):
            return "Command {} is not available".format(self.command)


class CoordinateOutOfRange(Exception):
    """
    Exception raised when robot coordinates are out of maze range
    """

    def __init__(self, coordinate):
        self.coordinate = coordinate

    def __str__(self):
        return "coordinates {} are out of maze range".format(self.coordinate)


class OverrideRobot(Exception):
    """
    Exception raised when an other robot is already on the spot
    """

    def __init__(self, coordinate, robot_name):
        self.coordinate = coordinate
        self.robot_name = robot_name

    def __str__(self):
        return "coordinates {} are already occupied by {}".format(self.coordinate, self.robot_name)


class EncounterObstacle(Exception):
    """
    Exception raised when robot coordinates are out of maze range
    """

    def __init__(self, itinerary):
        self.itinerary = itinerary

    def __str__(self):
        return "Itinerary {} encounters an obstacle".format(self.itinerary)