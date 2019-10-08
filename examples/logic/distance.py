#!/usr/bin/python3

import sys
import math

def distance(x1, y1, x2, y2):
    return math.sqrt( (x1 + x2)**2 + (y1 + y2)**2 )


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("USAGE: distance.py X1 Y1 X2 Y2")
        sys.exit(1)

    x1 = float(sys.argv[1])
    y1 = float(sys.argv[2])
    x2 = float(sys.argv[3])
    y2 = float(sys.argv[4])

    d = distance(x1, y1, x2, y2)

    print("The distance from ({:.2f},{:.2f}) to ({:.2f}, {:.2f}) is {:.2f}".format(x1, y1, x2, y2, d))
