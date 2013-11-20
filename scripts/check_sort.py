#!/usr/bin/env python
import sys

f = open(sys.argv[1], 'r')

for l in f:
    A = l.rstrip().split("\t")
    last = 0
    for a in A:
        d = [int(x) for x in a.split(" ")]
        if d[0] < last:
            print d
        last = d[0]
