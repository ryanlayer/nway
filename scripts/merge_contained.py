#!/usr/bin/env python
import sys
import numpy as np

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-s",
    "--sets_file",
    dest="sets_file",
    help="Data file")

(options, args) = parser.parse_args()

if not options.sets_file:
    parser.error('sets file not given')


f = open(options.sets_file,'r')

S = []
max_v = 0
for l in f:
    A = l.rstrip().split('\t')
    s = []
    for a in A:
        start =  int(a.split(' ')[0])
        end = int(a.split(' ')[1])
        s.append([start,end])
    S.append(s)
f.close()

NS=[]
for s in S:
    n = []
    in_context = 0
    for i in s:

        if in_context == 0:
            in_context = i
        elif in_context[1] < i[0]:
            n.append(in_context)
            in_context = i
        else:
            in_context[1] = max(in_context[1],i[1])

    n.append(in_context)
    print "\t".join(str(x[0]) + " " + str(x[1]) for x in n)
