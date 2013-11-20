#!/usr/bin/env python
import sys

f = open(sys.argv[1], 'r')

s_id = 0
for l in f:
    print s_id
    A = l.rstrip().split("\t")
    last = 0
    for a in A:
        d = [int(x) for x in a.split(" ")]
        if last == 0:
            last = d
        elif (last[1] > d[0]):
            print last,d
        last = d
    s_id += 1
