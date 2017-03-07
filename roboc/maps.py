# encoding utf-8

from toolbox import *
import os

class Maps:
    """
    Class to identify the map.
    """
    def __init__(self):
        self.drawings = dict()
        self.raw = dict()
        self.names = list()

    def load_map(self, path_to_maps):
        """
        Load all map available in the folder
        """

        names_list = find_file_extension(path_to_maps, "txt")
        for name in names_list:
            self.decode_map(path_to_maps, name)
            self.names.append(remove_file_extension(name))

    def decode_map(self, path_to_maps, map_full_name):
        """
        :param map_full_name: name of the file containing the map
        :return: map is formed of a list of string
        """

        map_lines = list()
        with open(os.path.join(path_to_maps, map_full_name)) as f:
            for line in f:
                line = line.rstrip()
                map_lines.append(line)

        map_name = remove_file_extension(map_full_name)
        self.drawings[map_name] = map_lines


