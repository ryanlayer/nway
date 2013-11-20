#!/usr/bin/env python
import sys
import numpy as np

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-g",
    "--genome_file",
    dest="genome_file",
    help="Genome file")

parser.add_option("-b",
    "--bed_files",
    dest="bed_files",
    help="commona sep list of BED files")


(options, args) = parser.parse_args()

if not options.genome_file:
    parser.error('Genome file not given')

if not options.bed_files:
    parser.error('BED files not given')

offsets = {}

offset = 0;
f = open(options.genome_file,'r')
for l in f:
    a = l.rstrip().split('\t')
    offsets[a[0]] = offset
    offset += int(a[1])

for file_name in options.bed_files.split(','):
    f = open(file_name,'r')

    line = []
    for l in f:
        a = l.rstrip().split('\t')
        offset = offsets[a[0]]
        line.append([int(a[1]),int(a[2])])
    l_s = sorted(line)
    print "\t".join( \
        [str(x[0]) + " " + str(x[1]) for x in l_s])
