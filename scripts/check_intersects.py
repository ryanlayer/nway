#!/usr/bin/env python
import sys
import numpy as np

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-i",
    "--intersect_file",
    dest="intersect_file",
    help="Data file")

parser.add_option("-s",
    "--sets_file",
    dest="sets_file",
    help="Data file")

(options, args) = parser.parse_args()

if not options.intersect_file:
    parser.error('intersect file not given')

if not options.sets_file:
    parser.error('sets file not given')


f = open(options.sets_file,'r')

I = []
max_v = 0
for l in f:
    A = l.rstrip().split('\t')
    i = []
    for a in A:
        start =  int(a.split(' ')[0])
        end = int(a.split(' ')[1])
        if end > max_v:
            max_v = end
        i.append([start,end])
    I.append(i)
f.close()


f = open(options.intersect_file,'r')

for l in f:
    curr_min = 0
    curr_max = max_v
    A = l.rstrip().split('\t')
    for i in range(len(A)):
        start = I[i][int(A[i])][0]
        end = I[i][int(A[i])][1]
        curr_min = max(curr_min, start)
        curr_max = min(curr_max, end)
    print curr_min,curr_max,curr_max - curr_min
f.close()

