#!/usr/bin/env python
import sys
import glob
import numpy as np

from optparse import OptionParser

def bsearch(v,l):
    hi = len(l)
    lo = -1

    while (hi - lo > 1):
        mid = (hi + lo) / 2
        if ( l[mid] < v ):
            lo = mid
        else:
            hi = mid
    return hi


parser = OptionParser()

parser.add_option("-g",
    "--genome_file",
    dest="genome_file",
    help="Genome file")

parser.add_option("-s",
    "--set_files",
    dest="set_files",
    help="Set files")


(options, args) = parser.parse_args()

if not options.genome_file:
    parser.error('Genome file not given')

if not options.set_files:
    parser.error('Set files not given')

offsets = {}

offset = 0;
f = open(options.genome_file,'r')
for l in f:
    a = l.rstrip().split('\t')
    offsets[a[0]] = offset
    offset += int(a[1])

O = {}

for chrm in offsets:
    O[offsets[chrm]] = chrm

O_keys = sorted(O.keys())


for file_name in glob.glob(options.set_files):
    f = open(file_name,'r')

    line = []
    for l in f:
        for a in l.rstrip().split('\t'):
            start,end = [int(x) for x in a.split(' ')]
            offset = bsearch(start,O_keys)
            if start < O_keys[offset]:
                offset -= 1
            chrm = O[O_keys[offset]]
            b_start = start - O_keys[offset]
            b_end = end - O_keys[offset] + 1
            print "\t".join([chrm, str(b_start), str(b_end)])

#        offset = offsets[a[0]]
#        line.append([int(a[1])+offset,int(a[2])+offset-1])
#    l_s = sorted(line)
#    print "\t".join( \
#        [str(x[0]) + " " + str(x[1]) for x in l_s])

#for file_name in options.bed_files.split(','):
#    f = open(file_name,'r')
#
#    line = []
#    for l in f:
#        a = l.rstrip().split('\t')
#        offset = offsets[a[0]]
#        print str(int(a[1])+offset) + '\t' + str(int(a[2])+offset) + '\t' + \
#                a[0] + '\t' + a[1] + '\t' +  a[2]
#
