# encoding utf-8

"""
Maps are string representing the maze viewed from above
X = walls
O = gates
R = robot
E = entrance
U = exit
"""

map_catalog = dict()
map_catalog_str = list()

easy = [
    "XXEXXXXXXX",
    "X        X",
    "X        X",
    "X        X",
    "X    U   X",
    "X        X",
    "X        X",
    "X        X",
    "XXXXXXXXXX"
]

medium = [
    "XXEXXXXXXX",
    "X        X",
    "XXXXXXXX X",
    "X        X",
    "X XXXXXXXX",
    "X        X",
    "XXXXXXXX X",
    "X   U    X",
    "XXXXXXXXXX"
]

map_catalog.update({"easy": easy})
map_catalog.update({"medium": medium})

for name in map_catalog.keys():
    map_catalog_str.append(str(name))


