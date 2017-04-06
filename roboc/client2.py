# -*- coding: utf-8 -*-

import time
from client import RobocClient


def main():
    RobocClient(name="antoine")
    while True:
        time.sleep(0.5)

if __name__ == '__main__':
    main()
