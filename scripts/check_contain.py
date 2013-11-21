#!/usr/bin/env python
import sys
import numpy as np

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-b",
    "--bed_file",
    dest="bed_file",
    help="BED file")

(options, args) = parser.parse_args()

if not options.bed_file:
    parser.error('BED file not given')

f = open(options.bed_file,'r')

s_id = 0
for l in f:
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
f.close()
