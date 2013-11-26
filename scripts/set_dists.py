#!/usr/bin/env python
import sys
import numpy as np

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-s",
    "--set_file",
    dest="set_file",
    help="Set file")

(options, args) = parser.parse_args()

if not options.set_file:
    parser.error('Set file not given')

f = open(options.set_file,'r')

for l in f:
    A = l.rstrip().split('\t')
    for i in range(len(A)):
        c_start,c_end = [int(x) for x in A[i].split(' ')]

        if i < (len(A) - 1):
            n_start,n_end = [int(x) for x in A[i+1].split(' ')]
            dist = n_start - c_end
            print dist
f.close()
