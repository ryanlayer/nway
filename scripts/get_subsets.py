#!/usr/bin/env python
import sys

w=840

S=[]
for file_name in sys.argv[2:]:
    f = open(file_name,'r')
    s = []
    for l in f:
        a = l.rstrip().split('\t')
        if 'chr1' in l:
            s.append([int(a[1]),int(a[2])])
    S.append(s)

I=[]
f = open(sys.argv[1], 'r')
for l in f:
    a = l.rstrip().split('\t')
    I.append([int(x) for x in a])

for i in I:
    for j in range(0,len(i)):
        line = []
        #for k in range(i[j]-3,i[j]+w-103):
        #for k in range(i[j]-w,i[j]+10):
        #for k in range(i[j],i[j]+1):
            #line.append(str(S[j][k][0]) + \
                        #' ' + \
                        #str(S[j][k][1]))
        print S[j][i[j]]
        #f 
        #print '\t'.join(line)
        #start = S[j][i[j]][0]
        #end = S[j][i[j]][1]
