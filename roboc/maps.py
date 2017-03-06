# encoding utf-8

from toolbox import *
import os

map_path = os.path.join(os.getcwd(), "Maps")


class Maps:
    """
    Class to identify the map.
    """
    def __init__(self):
        self.map_catalog = dict()
        self.map_catalog_str = list()

    def load_map(self):
        """
        Load all map available in the folder
        """

        carte_list = find_file_extension(map_path, "txt")
        print(carte_list)
        for carte in carte_list:
            self.decode_map(carte)
            self.map_catalog_str.append(carte.split(".")[0])

    def decode_map(self, map_name):
        """
        :param map_name: name of the map to decode
        :return: map is formed of a list of string
        """

        map_lines = list()
        with open(os.path.join(map_path, map_name)) as f:
            for line in f:
                line = line.rstrip()
                map_lines.append(line)

        self.map_catalog[map_name] = map_lines

maps = Maps()
maps.load_map()


