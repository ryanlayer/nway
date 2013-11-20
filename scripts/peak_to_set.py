#!/usr/bin/env python
import sys

offsets = {}

offset = 0;
f = open(sys.argv[1], 'r')
for l in f:
    a = l.rstrip().split('\t')
    offsets[a[0]] = offset
    offset += int(a[1])

for file_name in sys.argv[2:]:
    f = open(file_name,'r')

    line = []
    for l in f:
        a = l.rstrip().split('\t')
        #if (a[0] == 'chr1') or (a[0] == 'chr2'):
        #if (a[0] == 'chr2'):
        offset = offsets[a[0]]
        line.append([int(a[1]),int(a[2])])
    l_s = sorted(line)
    print "\t".join( \
        [str(x[0]) + " " + str(x[1]) for x in l_s])
