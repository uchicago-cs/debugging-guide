#!/usr/bin/python3

import sys

def foo(a, b):
    return a // b

def bar(a, b):
    return foo(a, b)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("USAGE: runtime-error.py NUM1 NUM2")
        sys.exit(1)

    a = int(sys.argv[1])
    b = int(sys.argv[2])

    x = bar(a, b)

    print("a / b = {}".format(x))