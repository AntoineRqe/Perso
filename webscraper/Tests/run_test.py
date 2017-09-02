# -*- coding: utf-8 -*-

import sys
import subprocess


def print_and_call(cmd):
    print("> {}".format(" ".join(cmd)))
    sys.stdout.flush()
    subprocess.check_call(cmd)


def run_test():

    cmd = ["coverage", "run", "-m", "unittest"]
    print_and_call(cmd)
    cmd = ["coverage", "report", "-m"]
    print_and_call(cmd)
    cmd = ["coverage", "html"]
    print_and_call(cmd)

if __name__ == '__main__':
    run_test()
