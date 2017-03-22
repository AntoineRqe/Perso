# encoding utf-8

import operation as op

game_options = {
    "S": {
        "cmd" : op.start_game,
        "desc": "Start a new game"
    },
    "L": {
        "cmd": op.load_game,
        "desc": "Load a game"
    },
    "E": {
        "cmd": op.edit_map,
        "desc": "Edit a map"
    }
}