import sys

for l in sys.stdin:
    A = l.rstrip().split('\t')
    for a in A:
        print a
